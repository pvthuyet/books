#include <zmq.hpp>
#include <string>
#include <iostream>

int main()
{
	zmq::context_t ctx(1);
	zmq::socket_t sock(ctx, ZMQ_REQ);
	std::cout << "Connecting to server ..." << std::endl;
	sock.connect("tcp://localhost:5555");

	// do 10 requests, waiting each time for a response
	for (int i = 0; i < 10; i++) {
		zmq::message_t req(5);
		memcpy_s(req.data(), 5, "hello", 5);
		std::cout << "Sending Hello " << i << " ... \n";
		sock.send(req);

		// get the reply
		zmq::message_t reply;
		sock.recv(&reply);
		std::cout << "Received: " << reply.to_string_view() << std::endl;
	}

	return EXIT_SUCCESS;
}