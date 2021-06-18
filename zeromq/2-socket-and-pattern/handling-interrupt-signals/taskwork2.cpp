#include <windows.h>
#include <zhelpers.hpp>

int main()
{
	using namespace std::string_view_literals;
	zmq::context_t ctx(1);

	//  Socket to receive messages on
	zmq::socket_t receiver(ctx, ZMQ_PULL);
	receiver.connect("tcp://localhost:5557");

	//  Socket to send messages to
	zmq::socket_t sender(ctx, ZMQ_PUSH);
	sender.connect("tcp://localhost:5558");

	//  Socket for control input
	zmq::socket_t controller(ctx, ZMQ_SUB);
	controller.connect("tcp://localhost:5559");
	controller.set(zmq::sockopt::subscribe, ""sv);

	//  Process messages from receiver and controller
	zmq::pollitem_t items[] = {
		{ static_cast<void*>(receiver), 0, ZMQ_POLLIN, 0 },
		{ static_cast<void*>(controller), 0, ZMQ_POLLIN, 0 }
	};

	//  Process messages from both sockets
	while (1) {
		zmq::message_t msg;
		zmq::poll(&items[0], 2, -1);

		if (items[0].revents & ZMQ_POLLIN) {
			receiver.recv(&msg);

			// process task
			int workload;
			std::string sdata(static_cast<char*>(msg.data()), msg.size());
			std::istringstream iss(sdata);
			iss >> workload;

			// do the work
			s_sleep(workload);

			// send results to sink
			msg.rebuild();
			sender.send(msg);

			//  Simple progress indicator for the viewer
			std::cout << '.' << std::flush;
		}

		//  Any waiting controller command acts as 'KILL'
		if (items[1].revents & ZMQ_POLLIN) {
			std::cout << std::endl;
			break;
		}
	}
	return 0;
}