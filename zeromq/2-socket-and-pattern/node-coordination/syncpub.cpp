#include <Windows.h>
#include <zhelpers.hpp>

//  We wait for 10 subscribers
#define SUBSCRIBERS_EXPECTED  3

int main()
{
	using namespace std::string_literals;
	zmq::context_t ctx(1);

	// socket to talk to clients
	zmq::socket_t publisher(ctx, ZMQ_PUB);
	publisher.set(zmq::sockopt::sndhwm, 0);
	publisher.bind("tcp://*:5561");

	// socket to receive signals
	zmq::socket_t syncservice(ctx, ZMQ_REP);
	syncservice.bind("tcp://*:5562");

	//  Get synchronization from subscribers
	int numsub = 0;
	while (numsub < SUBSCRIBERS_EXPECTED) {
		//  - wait for synchronization request
		s_recv(syncservice);

		//  - send synchronization reply
		s_send(syncservice, ""s);

		numsub++;
	}

	//  Now broadcast exactly 1M updates followed by END
	for (int i = 0; i < 1000; ++i) {
		s_send(publisher, "Rhubard"s);
	}
	s_send(publisher, "END"s);

	s_sleep(1);//  Give 0MQ time to flush output
	return 0;
}