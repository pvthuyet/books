//
//  Reading from multiple sockets in C++
//  This version uses a simple recv loop
//

#include <zmq.hpp>
#if (defined (WIN32))
#include <Windows.h>
#include <zhelpers.hpp>
#endif

int main(int argc, char* argv[])
{
	//  Prepare our context and sockets
	zmq::context_t ctx(1);
	//  Connect to task ventilator
	zmq::socket_t receiver(ctx, ZMQ_PULL);
	receiver.connect("tcp://localhost:5557");

	//  Connect to weather server
	zmq::socket_t subscriber(ctx, ZMQ_SUB);
	subscriber.connect("tcp://localhost:5556");
	subscriber.setsockopt(ZMQ_SUBSCRIBE, "10001 ", 6);

	//  Process messages from both sockets
	//  We prioritize traffic from the task ventilator

	while (1) {
		//  Process any waiting tasks
		bool rc{};
		do {
			zmq::message_t task;
			if ((rc = receiver.recv(&task, ZMQ_DONTWAIT)) == true) {
				// process task;
				std::cout << "5557: " << task.data() << std::endl;
			}
		} while (rc == true);

		//  Process any waiting weather updates
		do {
			zmq::message_t update;
			if ((rc = subscriber.recv(&update, ZMQ_DONTWAIT)) == true) {
				std::cout << "5556: " << update.data() << std::endl;
			}

		} while (rc == true);
		//  No activity, so sleep for 1 msec
		s_sleep(1);
	}
	return 0;
}