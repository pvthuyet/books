//
//   Request-reply service in C++
//   Connects REP socket to tcp://localhost:5560
//   Expects "Hello" from client, replies with "World"
//

#include <Windows.h>
#include <zhelpers.hpp>

int main()
{
	zmq::context_t ctx(1);
	zmq::socket_t responder(ctx, ZMQ_REP);
	responder.connect("tcp://localhost:5560");

	while (1) {
		std::string str = s_recv(responder);
		std::cout << "Received request: " << str << std::endl;
		s_sleep(1);

		// send reply back
		s_send(responder, str + std::string("world"));
	}
	return 0;
}