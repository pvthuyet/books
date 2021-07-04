//
//  Majordomo Protocol broker
//  A minimal implementation of http://rfc.zeromq.org/spec:7 and spec:8
//
//     Andreas Hoelzlwimmer <andreas.hoelzlwimmer@fh-hagenberg.at>
//

#include <Windows.h>
#include "zmsg.hpp"
#include "mdp.hpp"

#include <map>
#include <set>
#include <deque>
#include <list>
#include <chrono>
#include <fmt/core.h>
#include <gsl/gsl_assert>
#include <ranges>

//  We'd normally pull these from config data

#define HEARTBEAT_LIVENESS  3       //  3-5 is reasonable
#define HEARTBEAT_INTERVAL  2500    //  msecs
#define HEARTBEAT_EXPIRY    HEARTBEAT_INTERVAL * HEARTBEAT_LIVENESS

struct service;

// this defines one worker, idle or active
struct worker
{
	std::string identity_;
	std::weak_ptr<service> service_;
	std::chrono::steady_clock::time_point expiry_;
	worker(std::string_view ident, 
		std::shared_ptr<service> service = {}, 
		std::chrono::steady_clock::time_point expiry = std::chrono::steady_clock::now())
	{
		identity_ = ident;
		service_ = service;
		expiry_ = expiry;
	}
};

// this defines a single service
struct service
{
	std::string name_;
	std::deque<std::unique_ptr<zmsg>> requests_;
	std::list<std::weak_ptr<worker>> waiting_;
	size_t workers_{};

	service(std::string_view name) :
		name_(name)
	{}
};

class broker
{
private:
	zmq::context_t ctx_;
	zmq::socket_t socket_;
	int verbose_{1};
	std::string endpoint_;
	std::map<std::string, std::shared_ptr<service>> services_;
	std::map<std::string, std::shared_ptr<worker>> workers_;
	std::set<std::shared_ptr<worker>> waiting_;

public:
	broker(int verbose) :
		ctx_(1),
		socket_(ctx_, zmq::socket_type::router),
		verbose_(verbose)
	{}

	void bind(std::string_view endpoint)
	{
		endpoint_ = endpoint;
		socket_.bind(endpoint_);
		fmt::print("[info] MDP broker/0.1.1 is active at {}\n", endpoint_);
	}

	void start_brokering()
	{
		using sc = std::chrono::steady_clock;
		auto heartbeat_at = std::chrono::steady_clock::now();
		while (!s_interrupted) {
			std::vector<zmq::pollitem_t> items = {
				{socket_, 0, ZMQ_POLLIN, 0 }
			};

			zmq::poll(items, HEARTBEAT_INTERVAL);

			// Process next input message, if any
			if (items[0].revents & ZMQ_POLLIN) {
				auto msg = std::make_unique<zmsg>(socket_);
				if (verbose_) {
					fmt::print("[info] received message:\n");
					msg->dump();
				}
				auto sender = msg->unwrap2();
				std::string header = reinterpret_cast<const char*>(msg->pop_front().c_str());
				if (0 == header.compare(MDPC_CLIENT)) {
					client_process(sender, std::move(msg));
				}
				else if (0 == header.compare(MDPW_WORKER)) {
					worker_process(sender, std::move(msg));
				}
				else {
					fmt::print("[err] invalid message:\n");
					msg->dump();
				}
			}

			// Disconnect and delete any expired workers
			// Send heartbeat to idle workers if needed
			auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(sc::now() - heartbeat_at);
			if (diff.count() > HEARTBEAT_INTERVAL) {
				purge_workers();
				for (auto& w : waiting_) {
					worker_send(w, MDPW_HEARTBEAT, "", nullptr);
				}
				heartbeat_at = sc::now();
			}
		}
	}

private:
	void purge_workers()
	{
		using sc = std::chrono::steady_clock;
		std::deque<std::shared_ptr<worker>> to_cull;
		auto now = sc::now();
		for (auto& wrk : waiting_) {
			auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(now - wrk->expiry_);
			if (diff.count() > HEARTBEAT_EXPIRY) {
				to_cull.push_back(wrk);
			}
		}

		for (auto& wkr : to_cull) {
			if (verbose_) {
				fmt::print("[info] deleting expired worker: {}\n", wkr->identity_);
			}
			worker_delete(wkr, 0);
		}
	}

	void worker_delete(std::shared_ptr<worker> wrk, int disconnect)
	{
		Ensures(wrk);
		if (disconnect) {
			worker_send(wrk, MDPW_DISCONNECT, "", nullptr);
		}

		auto service = wrk->service_.lock();
		if (service) {
			std::erase_if(service->waiting_, [&wrk, &service](auto& item) {
				if (wrk == item.lock()) {
					service->workers_--;
					return true;
				}
				return false;
				});
		}

		waiting_.erase(wrk);
		workers_.erase(wrk->identity_);
	}

	void worker_send(std::shared_ptr<worker> wrk, std::string_view command, std::string_view option, std::unique_ptr<zmsg> msg)
	{
		if (!msg) {
			msg = std::make_unique<zmsg>();
		}

		if (!option.empty()) {
			msg->wrap(option.data(), nullptr);
		}

		msg->wrap(command.data(), nullptr);
		msg->wrap(MDPW_WORKER, nullptr);

		// stack routing envelope to start of message
		msg->wrap(wrk->identity_.c_str(), "");

		if (verbose_) {
			fmt::print("[info] sending {} to worker\n", mdps_commands[(int)*command.data()]);
			msg->dump();
		}
		msg->send(socket_);
	}

	std::shared_ptr<service> service_require(std::string const& name)
	{
		Ensures(!name.empty());
		if (services_.count(name)) {
			return services_.at(name);
		}

		auto srv = std::make_shared<service>(name);
		services_.insert(std::make_pair(name, srv));
		if (verbose_) {
			fmt::print("[info] received message\n");
		}
		return srv;
	}

	void service_dispatch(std::shared_ptr<service> srv, std::unique_ptr<zmsg> inmsg)
	{
		Ensures(srv);
		if (inmsg) { // Queue message if any
			srv->requests_.push_back(std::move(inmsg));
		}
		purge_workers();

		while (!srv->waiting_.empty() && !srv->requests_.empty()) {
			// choose the most recently seen idle worker; others might be about to expire
			auto recentwrk = std::ranges::max_element(srv->waiting_, [](auto const& a, auto const& b) {
				return a.lock()->expiry_ < b.lock()->expiry_;
				});
			
			std::unique_ptr<zmsg> msg{};
			std::swap(msg, srv->requests_.front());
			srv->requests_.pop_front();

			auto shwrk = recentwrk->lock();
			worker_send(shwrk, MDPW_REQUEST, "", std::move(msg));
			waiting_.erase(shwrk);
			srv->waiting_.erase(recentwrk);
		}
	}

	void service_internal(std::string_view service_name, std::unique_ptr<zmsg> inmsg)
	{
		using namespace std::string_view_literals;
		if (service_name == "mmi.service"sv) {
			auto srv = services_.at(inmsg->body());
			if (srv && srv->workers_) {
				inmsg->body_set("200");
			}
			else {
				inmsg->body_set("404");
			}
		}
		else {
			inmsg->body_set("501");
		}

		// remove and save client return envelope and insert the 
		// protocal header and service name, then rewrap envelope
		auto client = inmsg->unwrap2();
		inmsg->wrap(MDPC_CLIENT, service_name.data());
		inmsg->wrap(client.c_str(), "");
		inmsg->send(socket_);
	}

	std::shared_ptr<worker> worker_require(std::string const& identity)
	{
		Ensures(!identity.empty());
		if (workers_.count(identity)) {
			return workers_.at(identity);
		}
		else {
			auto wrk = std::make_shared<worker>(identity);
			workers_.insert(std::make_pair(identity, wrk));
			fmt::print("[info] registering new worker: {}\n", identity);
			return wrk;
		}
	}

	void worker_process(std::string const& sender, std::unique_ptr<zmsg> inmsg)
	{
		Ensures(inmsg && inmsg->parts() >= 1);
		std::string command = reinterpret_cast<const char*>(inmsg->pop_front().c_str());
		bool worker_ready = workers_.count(sender) > 0;
		auto wrk = worker_require(sender);

		if (command == MDPW_READY) {
			if (worker_ready) {
				worker_delete(wrk, 1);
			}
			else {
				if (sender.size() >= 4 && 0 == sender.find_first_of("mmi.")) {
					worker_delete(wrk, 1);
				}
				else {
					// attach worker to service and mark as idle
					std::string service_name = reinterpret_cast<const char*>(inmsg->pop_front().c_str());
					wrk->service_ = service_require(service_name);
					wrk->service_.lock()->workers_++;
					worker_waiting(wrk);
				}
			}
		}
		else {
			if (command == MDPW_REPLY) {
				if (worker_ready) {
					// Remove & save client return envelope and insert the 
					// protocal header and service name, then rewrap envelop
					auto srv = wrk->service_.lock();
					Expects(srv);
					auto client = inmsg->unwrap2();
					inmsg->wrap(MDPC_CLIENT, srv->name_.c_str());
					inmsg->wrap(client.c_str(), "");
					inmsg->send(socket_);
					worker_waiting(wrk);
				}
				else {
					worker_delete(wrk, 1);
				}
			}
			else {
				if (command == MDPW_HEARTBEAT) {
					if (worker_ready) {
						wrk->expiry_ = std::chrono::steady_clock::now();
					}
					else {
						worker_delete(wrk, 1);
					}
				}
				else {
					if (command == MDPW_DISCONNECT) {
						worker_delete(wrk, 1);
					}
					else {
						fmt::print("[err] invalid input message ({})\n", (int)*command.c_str());
						inmsg->dump();
					}
				}
			}
		}
	}

	void worker_waiting(std::shared_ptr<worker> worker)
	{
		Ensures(worker);
		// queue to broker and service waiting lists
		auto srv = worker->service_.lock();
		Ensures(srv);
		waiting_.insert(worker);
		srv->waiting_.push_back(worker);
		worker->expiry_ = std::chrono::steady_clock::now();
		// attempt to process outstading requests
		service_dispatch(srv, 0);
	}

	void client_process(std::string const& sender, std::unique_ptr<zmsg> inmsg)
	{
		Ensures(inmsg && inmsg->parts() >= 2);
		std::string service_name = reinterpret_cast<const char*>(inmsg->pop_front().c_str());
		auto srv = service_require(service_name);
		// set reply return address to client sender
		inmsg->wrap(sender.c_str(), "");
		if (service_name.length() >= 4 && service_name.find_first_of("mmi.") == 0) {
			service_internal(service_name, std::move(inmsg));
		}
		else {
			service_dispatch(srv, std::move(inmsg));
		}
	}
};


int main(int argc, char* argv[])
{
	broker brk(1);
	brk.bind("tcp://*:5555");

	brk.start_brokering();
	if (s_interrupted) {
		fmt::print("[warn] interrupt received, shutting down...\n");
	}
	return 0;
}