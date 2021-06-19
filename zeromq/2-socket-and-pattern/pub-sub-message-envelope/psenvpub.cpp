#include <Windows.h>
#include <zhelpers.hpp>

int main()
{
	using namespace std::string_literals;
	//  Prepare our context and publisher
	zmq::context_t ctx(1);
	zmq::socket_t publisher(ctx, ZMQ_PUB);
	publisher.bind("tcp://*:5563");

	while (1) {
		s_sendmore(publisher, "A");
		s_send(publisher, "We don't want to see this"s);
		s_sendmore(publisher, "B");
		s_send(publisher, "We would like to see this"s);
		s_sleep(1000);
	}
	return 0;
}