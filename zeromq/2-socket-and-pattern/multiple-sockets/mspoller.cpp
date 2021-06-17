#include <zmq.hpp>
#if (defined (WIN32))
#include <Windows.h>
#include <zhelpers.hpp>
#endif

int main(int argc, char* argv[])
{
	zmq::context_t ctx;

	//  Connect to task ventilator
	zmq::socket_t receiver(ctx, ZMQ_PULL);
	receiver.connect("tcp://localhost:5557");

	//  Connect to weather server
	zmq::socket_t subcriber(ctx, ZMQ_SUB);
	subcriber.connect("tcp://localhost:5556");
	subcriber.setsockopt(ZMQ_SUBSCRIBE, "10001 ", 6);

	//  Initialize poll set
	zmq::pollitem_t items[] = {
		{static_cast<void*>(receiver), 0, ZMQ_POLLIN, 0},
		{static_cast<void*>(subcriber), 0, ZMQ_POLLIN, 0},
	};

	//  Process messages from both sockets
	while (1) {
		zmq::message_t msg;
		zmq::poll(&items[0], 2, -1);

		if (items[0].revents & ZMQ_POLLIN) {
			receiver.recv(&msg);
			// process task
			std::cout << "5557 : " << msg.data() << std::endl;
		}

		if (items[1].revents & ZMQ_POLLIN) {
			subcriber.recv(&msg);
			std::cout << "5556 : " << msg.data() << std::endl;
		}
	}
	return 0;
}