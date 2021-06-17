//
//  Simple message queuing broker in C++
//  Same as request-reply broker but using QUEUE device
//

#include <Windows.h>
#include <zhelpers.hpp>

int main()
{
	zmq::context_t ctx(1);
	zmq::socket_t frontend(ctx, ZMQ_ROUTER);
	frontend.bind("tcp://*:5559");

	// socket facing services
	zmq::socket_t backend(ctx, ZMQ_DEALER);
	backend.bind("tcp://*:5560");

	// start the proxy
	zmq::proxy(frontend, backend);
}