#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_set>
#include <random>
#include <exception>
#include <algorithm>
#include <date/date.h>
#include <Windows.h>
//#include <zmsg.hpp>
#include <fmt/core.h>
#include <zmqpp/actor.hpp>
#include <zmqpp/poller.hpp>
#include <zmqpp/message.hpp>

//  If no server replies within this time, abandon request
#define GLOBAL_TIMEOUT  3000    //  msecs
//  PING interval for servers we think are alive
#define PING_INTERVAL   3000    //  msecs
//  Server considered dead if silent for this long
#define SERVER_TTL      6000    //  msecs

//  .split API structure
//  This API works in two halves, a common pattern for APIs that need to
//  run in the background. One half is an frontend object our application
//  creates and works with; the other half is a backend "agent" that runs
//  in a background thread. The frontend talks to the backend over an
//  inproc pipe socket:

//  Structure of our frontend class

int gen_num(int a, int b)
{
	std::random_device rd;  //Will be used to obtain a seed for the random number engine
	std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
	std::uniform_int_distribution<> distrib(a, b);
	return distrib(gen);
}

namespace flcaip {
class server
{
public:
	using tp = std::chrono::time_point<std::chrono::system_clock>;
	// simple class for one server we talk to

	// server identity/endpoint
	std::string endpoint_;

	// true if know to be alive
	bool alive_;

	// Next ping at this time
	tp pingat_;

	// Expires at this time
	tp expires_;

public:
	server(std::string_view ep) :
		endpoint_(ep)
	{
		refresh(true);
	}

	void refresh(bool alive)
	{
		namespace chr = std::chrono;
		alive_ = alive;
		if (alive_) {
			pingat_ = chr::floor<chr::milliseconds>(chr::system_clock::now()) + chr::milliseconds{ PING_INTERVAL };
			expires_ = chr::floor<chr::milliseconds>(chr::system_clock::now()) + chr::milliseconds{ SERVER_TTL };
		}
	}

	void ping(zmqpp::socket_t & sock)
	{
		namespace chr = std::chrono;
		auto now = chr::floor<chr::milliseconds>(chr::system_clock::now());
		if (now > pingat_) {
			zmqpp::message msg{};
			msg << "PING";
			msg.push_front(endpoint_);
			sock.send(msg);
			pingat_ = now + chr::milliseconds{ PING_INTERVAL };
		}
	}
};

class agent
{
public:
	using tp = std::chrono::time_point<std::chrono::system_clock>;
	// We build the agent as a class that's capable of processing messages
	// coming in from its various sockets:
	// 
	// Simple class for one background agent

	// socket to talk back to application
	zmqpp::socket_t& pipe_;

	// socket to talk to servers
	std::unique_ptr<zmqpp::socket_t> router_;

	// server we've connected to
	std::vector<server> servers_;

	// Server we know are alive
	std::vector<std::reference_wrapper<server>> actives_;

	// Number of request ever sent
	int sequence_{};

	// current request if any
	std::unique_ptr<zmqpp::message> request_;

	// Current reply if any
	// ...

	// Timeout for request/reply
	tp expires_;

public:
	agent(zmqpp::context_t& ctx, zmqpp::socket_t& pipe) :
		agent(ctx, pipe, "")
	{
		using namespace std::string_literals;
		std::string name = "CLIENT"s + std::to_string(gen_num(1, 1000));
		router_->set(zmqpp::socket_option::identity, name);
	}

	agent(zmqpp::context_t& ctx, zmqpp::socket_t& pipe, std::string_view name) :
		pipe_{ pipe }
	{
		router_ = std::make_unique<zmqpp::socket_t>(ctx, zmqpp::socket_type::router);
		if (!name.empty()) {
			router_->set(zmqpp::socket_option::identity, std::string{ name });
		}
	}

	void control_message(zmqpp::message& msg)
	{
		using namespace std::string_literals;
		// this method processes on message from our frontend class
		// it's going to be CONNECT or REQUEST
		auto command = msg.get<std::string>(0);
		if (command == "STOP") {
			throw std::runtime_error("Exit client");
		}

		else if (command == "CONNECT") {
			auto ep = msg.get<std::string>(1);
			fmt::print("I: connecting to {}...\n", ep);
			router_->connect(ep);

			servers_.push_back(server(ep));
			actives_.emplace_back(servers_.back());
		}
		else if (command == "REQUEST") {
			if (request_) {
				throw std::runtime_error("request-reply cycle");
			}
			msg.pop_front();
			msg.push_front(fmt::format("{}", ++sequence_));
			request_ = std::make_unique<zmqpp::message_t>();
			*request_ = msg.copy();
			// request expires after global timeout
			expires_ = std::chrono::floor<std::chrono::milliseconds>(std::chrono::system_clock::now()) + std::chrono::milliseconds{ GLOBAL_TIMEOUT };
		}
	}

	void router_message(zmqpp::message& reply)
	{
		// this method processes one message from a connected server

		// Frame 0 is server that replied
		auto ep = reply.get<std::string>(0);
		auto found = std::ranges::find_if(servers_, [&ep](auto const& item) {
			return ep == item.endpoint_;
			});
		if (found != std::cend(servers_) && !found->alive_) {
			actives_.emplace_back(*found);
			found->refresh(true);
		}

		// Frame 1 may be sequence number for reply
		int seqnum{};
		auto seqstr = reply.get<std::string>(1);
		auto [p, ec] = std::from_chars(std::data(seqstr), std::data(seqstr) + std::size(seqstr), seqnum);
		if (seqnum == sequence_) {
			reply.pop_front();
			reply.pop_front();
			reply.push_front("OK");
			pipe_.send(reply);
		}
	}
};

class free_lance_client
{
public:
	zmqpp::context_t ctx_;

	// pipe through to flcliapi agent
	std::unique_ptr<zmqpp::actor> actor_;
	free_lance_client()
	{
		actor_ = std::make_unique<zmqpp::actor>([this](auto sock) {
			return this->actor_routine(sock);
			});
	}

	void connect(std::string const& endpoint)
	{
		// To implement the connect method, the frontend object sends a multipart
		// message to the backend agent. The first part is a string "CONNECT", and
		// the second part is the endpoint. It waits 100msec for the connection to
		// come up, which isn't pretty, but saves us from sending all requests to a
		// single server, at startup time:
		zmqpp::message_t msg;
		msg << "CONNECT";
		msg << endpoint;
		actor_->pipe()->send(msg);
		std::this_thread::sleep_for(std::chrono::milliseconds(64)); // allow connection to come up
	}

	void stop()
	{
		zmqpp::message_t msg;
		msg << "STOP";
		actor_->pipe()->send(msg);
	}

	std::optional<zmqpp::message_t> request(zmqpp::message_t& req)
	{
		// To implement the request method, the frontend object sends a message
		// to the backend, specifying a command "REQUEST" and the request message:
		req.push_front("REQUEST");
		actor_->pipe()->send(req);

		zmqpp::message_t reply;
		if (actor_->pipe()->receive(reply)) {
			auto status = reply.get<std::string>(0);
			if (status == "FAILED") {
				fmt::print("REQUEST => FAILED\n");
				return {};
			}
		}
		return reply;
	}

	bool actor_routine(zmqpp::socket* pipe)
	{
		try {
			pipe->send(zmqpp::signal::ok);
			auto agt = std::make_unique<agent>(ctx_, *pipe);
			zmqpp::poller poller{};
			poller.add(*pipe);
			poller.add(*agt->router_);
			while (1) {
				auto ok = poller.poll(1000);
				if (!ok) {
					//fmt::print("[error] actor_routine\n");
					//break;
				}
				if (poller.events(*pipe) & zmqpp::poller::poll_in) {
					zmqpp::message_t msg;
					if (pipe->receive(msg, true)) {
						agt->control_message(msg);
					}
				}

				if (poller.events(*agt->router_) & zmqpp::poller::poll_in) {
					zmqpp::message_t msg;
					if (agt->router_->receive(msg, true)) {
						agt->router_message(msg);
					}
				}

				if (agt->request_) {
					auto now = std::chrono::floor<std::chrono::milliseconds>(std::chrono::system_clock::now());
					if (now > agt->expires_) {
						// request expired, kill it
						zmqpp::message_t msg("FAILED");
						agt->pipe_.send(msg);
						agt->request_ = nullptr;
					}
					else {
						// find server to talk to, remove any expired ones
						auto now = std::chrono::floor<std::chrono::milliseconds>(std::chrono::system_clock::now());
						std::erase_if(agt->actives_, [now](auto const& item) {
							return now > item.get().expires_;
							});
						if (agt->actives_.size() > 0) {
							server& s = agt->actives_.front().get();
							auto req = agt->request_->copy();
							req.push_front(s.endpoint_);
							agt->router_->send(req);
							agt->request_ = nullptr;
						}
					}
				}

				// disconnect and delete expired servers
				// send heartbeats to idle servers if needed
				for (auto& s : agt->servers_) {
					s.ping(*agt->router_);
				}
			}
		}
		catch (std::exception const& ex) {
			fmt::print("[ERR] {}\n", ex.what());
		}
		return true;
	}
};
}