#include <iostream>
#include <zmq.hpp>
#include <thread>
#include <Windows.h>
#include <zhelpers.hpp>

void worker_routine(zmq::context_t& ctx)
{
	zmq::socket_t socket(ctx, ZMQ_REP);
	socket.connect("inproc://workers");

	while (true) {
		std::string req = s_recv(socket);
		std::cout << std::this_thread::get_id() << " Received request: " << req << std::endl;
		// do some work
		s_sleep(1);

		// send reply back to client
		s_send(socket, req + std::string("world"));
	}
}

int main()
{
	zmq::context_t ctx(1);
	zmq::socket_t clients(ctx, ZMQ_ROUTER);
	clients.bind("tcp://*:5559");

	zmq::socket_t workers(ctx, ZMQ_DEALER);
	workers.bind("inproc://workers");

	//  Launch pool of worker threads
	constexpr int num = 5;
	std::vector<std::thread> threads;
	threads.reserve(num);
	for (int i = 0; i < num; ++i) {
		threads.push_back(std::thread(worker_routine, std::ref(ctx)));
	}

	zmq::proxy(clients, workers);

	// wait on worker thread
	for (auto& t : threads) t.join();

	// done
	return 0;
}