#include <Windows.h>
#include <zhelpers.hpp>
#include <fmt/format.h>
#include <fmt/printf.h>
#include <thread>
#include <vector>
#include <memory>
#include <gsl/gsl_assert>
#include <queue>
#include <random>

using namespace std::string_literals;
using namespace std::string_view_literals;

int gen_num(int a, int b)
{
	std::random_device rd;  //Will be used to obtain a seed for the random number engine
	std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
	std::uniform_int_distribution<> distrib(a, b);
	return distrib(gen);
}

class worker_task
{
private:
	zmq::context_t& ctx_;
	zmq::socket_t sock_;
	int id_;
	std::string port_;

public:
	worker_task(zmq::context_t& ctx, int id, std::string port) :
		ctx_{ctx},
		sock_(ctx, zmq::socket_type::req),
		id_{id},
		port_{port}
	{
		sock_.set(zmq::sockopt::routing_id, std::to_string(id));
	}

	void start()
	{
		// connect
		fmt::print("Worker {} connecting to host {}\n", id_, port_);
		try {
			// connect
			sock_.connect(fmt::format("tcp://localhost:{}", port_));

			// tell broker we're ready for work
			s_send(sock_, "READY"s);

			while (1) {
				auto addr = s_recv(sock_);
				{
					//auto empty = s_recv(sock_);
					//Expects(empty.empty());
				}
				auto req = s_recv(sock_);
				fmt::print("Worker {}: {} {}\n", id_, addr, req);

				s_sleep(1000);
				s_sendmore(sock_, addr);
				//s_sendmore(sock_, ""s);
				s_send(sock_, "OK"s);
			}
		}
		catch (std::exception const& ex) {
			fmt::print("[ERROR] {}\n", ex.what());
		}
	}
};

int main(int argc, char* agrv[])
{
	try {
		if (argc < 3) {
			fmt::print("use: peering2_server [me] [you]");
			return -1;
		}

		int meport{};
		std::string_view strme(agrv[1]);
		std::from_chars(std::data(strme), std::data(strme) + std::size(strme), meport);
		zmq::context_t ctx(1);

		// beetwen broker-broker
		zmq::socket_t cloudfe(ctx, zmq::socket_type::router);
		zmq::socket_t cloudbe(ctx, zmq::socket_type::router);

		// beetween client-server
		zmq::socket_t localfe(ctx, zmq::socket_type::router);
		zmq::socket_t localbe(ctx, zmq::socket_type::router);

		// bind cloud frontend to endpoint
		cloudfe.set(zmq::sockopt::routing_id, "c1_fe"sv);
		cloudfe.bind(fmt::format("tcp://127.0.0.1:{}", meport));

		// connect cloud backend to all peers
		cloudbe.set(zmq::sockopt::routing_id, "c1_be"sv);
		for (int i = 2; i < argc; ++i) {
			std::string peer = agrv[i];
			fmt::print("I: connecting to cloud frontend at {}\n", peer);
			cloudbe.connect(fmt::format("tcp://127.0.0.1:{}", peer));
		}

		// Prepare local frontend and backend
		int lcfeport = meport + 1;
		int lcbeport = meport + 2;
		localfe.bind(fmt::format("tcp://127.0.0.1:{}", lcfeport));
		localbe.bind(fmt::format("tcp://127.0.0.1:{}", lcbeport));

		// Get user to tell us whe we can start
		fmt::print("Press ENTER to start ...\n");
		std::cin.get();

		// start local workers
		std::vector<std::jthread> wkThreads;
		for (int i = 0; i < 1; ++i) {
			wkThreads.push_back(std::jthread([&ctx, i, lcbeport]() {
				worker_task wk(ctx, i, std::to_string(lcbeport));
				wk.start();
				}));
		}

		// Here, we handle the request-reply flow. We're using load-balancing
		// to poll workers at all times, and clients only when there are one
		// or more workers available.

		// Least recently used queue of available workers
		std::queue<std::string> workers_queue;
		zmq::pollitem_t items[] = {
			{localbe, 0, ZMQ_POLLIN, 0},
			{cloudbe, 0, ZMQ_POLLIN, 0}
		};

		while (1) {
			// If we have no workers, wait indefinitely
			if (workers_queue.size()) {
				//zmq::poll(&items[0], 2, -1);
			}
			else {
				//zmq::poll(&items[0], 1, -1);
			}
			zmq::poll(&items[0], 2, -1);

			std::string ident;
			std::string incomming;
			// handle reply from local worker
			if (items[0].revents & ZMQ_POLLIN) {
				// 1st frame is identity
				ident = s_recv(localbe);
				workers_queue.push(ident);
				{
					// 2nd frame is empty
					auto empty = s_recv(localbe);
					Expects(empty.empty());
				}

				// 3rd frame is READY or else a worker reply address
				incomming = s_recv(localbe);
				if (incomming == "READY"sv) {
					// do not thing
					incomming = "";
				}
			}
			else if (items[1].revents & ZMQ_POLLIN) {
				// We don't use peer broker identity for anything
				// TODO
			}

			// Route reply to cloud if it's addressed to a broker
			if (!incomming.empty()) {
				for (int i = 2; i < argc; ++i) {
					if (ident == std::string(agrv[i])) {
						s_sendmore(cloudfe, ident);
						s_send(cloudfe, incomming);
						incomming = "";
						break;
					}
				}
			}

			// Route reply to client if we still need to
			if (!incomming.empty()) {
				//auto empty = s_recv(localbe);
				//Expects(empty.empty());
				auto reply = s_recv(localbe);

				s_sendmore(localfe, incomming);
				s_sendmore(localfe, ""s);
				s_send(localfe, reply);
				incomming = "";
			}

			// Now we route as many client requests as we have worker capacity
			// for. We may reroute requests from our local frontend, but not from //
			// the cloud frontend. We reroute randomly now, just to test things
			// out. In the next version, we'll do this properly by calculating
			// cloud capacity://
			// TODO
			while (workers_queue.size()) {
				std::string feident;
				std::string feincoming;

				std::vector<zmq::pollitem_t> feitems = {
					{localfe, 0, ZMQ_POLLIN, 0},
					{cloudfe, 0, ZMQ_POLLIN, 0}
				};

				int reroutable{};
				zmq::poll(feitems, 1000);
				if (feitems[0].revents & ZMQ_POLLIN) {
					reroutable = 0;
					feident = s_recv(localfe);
					{
						Expects(s_recv(localfe) == ""s);
					}
					feincoming = s_recv(localfe);
				}
				else if (feitems[1].revents & ZMQ_POLLIN) {
					reroutable = 1;
					feident = s_recv(cloudfe);
					{
						Expects(s_recv(cloudfe) == ""s);
					}
					feincoming = s_recv(cloudfe);
				}

				if (!feincoming.empty()) {
					if (reroutable == 0) {
						// Route to local broker peer
						std::string peer = workers_queue.front();
						workers_queue.pop();
						s_sendmore(localbe, peer);
						s_sendmore(localbe, ""s);
						s_sendmore(localbe, feident);
						s_send(localbe, feincoming);
						feincoming = "";
					}
					else {
						// Route to random broker peer
						int peer = gen_num(0, argc-2) + 2;
						s_sendmore(cloudbe, std::string{ agrv[peer] });
						s_sendmore(cloudbe, "");
						s_send(cloudbe, feincoming);
						feincoming = "";
					}
				}
			}
		}


	}
	catch (const std::exception& ex) {
		fmt::print("[ERROR] {}\n", ex.what());
	}

	return EXIT_SUCCESS;
}