#if (defined (WIN32))
#include <Windows.h>
#include <zhelpers.hpp>
#endif

int main(int argc, char* argv[])
{
	using namespace std::literals;
	zmq::context_t ctx(1);

	//  This is where the weather server sits
	zmq::socket_t frontend(ctx, ZMQ_SUB);
	frontend.connect("tcp://localhost:5556");

	//  This is our public endpoint for subscribers
	zmq::socket_t backend(ctx, ZMQ_PUB);
	backend.bind("tcp://*:8100");

	//  Subscribe on everything
	//frontend.setsockopt(ZMQ_SUBSCRIBE, "", 0);
	frontend.set(zmq::sockopt::subscribe, ""sv);

	//  Shunt messages out to our own subscribers
	while (1) {
		while (1) {
			zmq::message_t msg;
			int more;
			size_t more_size = sizeof more;
			frontend.recv(msg);
			auto opt = frontend.get(zmq::sockopt::rcvmore);
			//frontend.getsockopt(ZMQ_RCVMORE, &more, &more_size);
			//assert(more == opt);
			backend.send(msg, opt ? ZMQ_SNDMORE : 0);
			if (!opt) break;
		}
	}
	return 0;
}