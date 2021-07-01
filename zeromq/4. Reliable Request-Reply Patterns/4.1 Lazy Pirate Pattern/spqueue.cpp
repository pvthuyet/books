//
//  Simple Pirate queue
//  This is identical to the LRU pattern, with no reliability mechanisms
//  at all. It depends on the client for recovery. Runs forever.
//
//  Andreas Hoelzlwimmer <andreas.hoelzlwimmer@fh-hagenberg.at
#include <Windows.h>
#include <zhelpers.hpp>
#include <zmsg.hpp>
#include <queue>
#include <vector>
#include <gsl/gsl_assert>

constexpr const int MAX_WORKERS = 100;
int main()
{
	using namespace std::string_literals;
	using namespace std::string_view_literals;

	// Prepare our context and sockets
	zmq::context_t ctx(1);
	zmq::socket_t frontend(ctx, zmq::socket_type::router);
	zmq::socket_t backend(ctx, zmq::socket_type::router);
	frontend.bind("tcp://*:5555");
	backend.bind("tcp://*:5556");

	std::queue<std::string> worker_queue;
	while (1) {
		std::vector<zmq::pollitem_t> items = { 
			{backend, 0, ZMQ_POLLIN, 0},
			{frontend, 0, ZMQ_POLLIN, 0}
		};
		
		// Poll frontend only if we have available workers
		if (worker_queue.size()) {
			zmq::poll(items, -1);
		}
		else {
			zmq::poll(&items[0], 1, -1);
		}

		// Handle worker activity on backend
		if (items[0].revents & ZMQ_POLLIN) {
			zmsg zm(backend);
			// use worker address for LRU routing
			Ensures(worker_queue.size() < MAX_WORKERS);
			worker_queue.push(zm.unwrap());

			// Return reply to client if it's not a READY
			std::string_view addr(zm.address());
			if (addr == "READY"sv) {
				zm.clear();
			}
			else {
				zm.send(frontend);
			}
		}

		if (items[1].revents & ZMQ_POLLIN) {
			//  Now get next client request, route to next worker
			zmsg zm(frontend);
			//  REQ socket in worker needs an envelope delimiter
			zm.wrap(worker_queue.front().c_str(), "");
			zm.send(backend);

			// Dequeue and drop the next worker address
			worker_queue.pop();
		}
	}
	// We never exit the main loop
	return 0;
}