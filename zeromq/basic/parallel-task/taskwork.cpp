//
//  Task worker in C++
//  Connects PULL socket to tcp://localhost:5557
//  Collects workloads from ventilator via that socket
//  Connects PUSH socket to tcp://localhost:5558
//  Sends results to sink via that socket
//

#if (defined (WIN32))
#include <Windows.h>
#include <zhelpers.hpp>
#endif

#include <string>

int main()
{
	zmq::context_t ctx(1);

	//  Socket to receive messages on
	zmq::socket_t receiver(ctx, ZMQ_PULL);
	receiver.connect("tcp://localhost:5557");

	//  Socket to send messages to
	zmq::socket_t sender(ctx, ZMQ_PUSH);
	sender.connect("tcp://localhost:5558");

	while (1) {
		zmq::message_t msg;
		int workload;
		receiver.recv(&msg);
		std::string str(static_cast<char*>(msg.data()));

		std::istringstream iss(str);
		iss >> workload;

		// do work
		s_sleep(workload);

		//  Send results to sink
		msg.rebuild();
		sender.send(msg);

		//  Simple progress indicator for the viewer
		std::cout << "." << std::flush;
	}
	return 0;
}