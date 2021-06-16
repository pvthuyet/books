//
//  Task ventilator in C++
//  Binds PUSH socket to tcp://localhost:5557
//  Sends batch of tasks to workers via that socket
//

#include <zmq.hpp>
#include <stdlib.h>
#include <stdio.h>
//#include <unistd.h>
#include <iostream>
#include <format>

#if (defined (WIN32))
#include <Windows.h>
#include <zhelpers.hpp>
#endif

#define within(num) (int) ((float) num * random () / (RAND_MAX + 1.0))

int main()
{
	zmq::context_t ctx(1);
	zmq::socket_t sender(ctx, ZMQ_PUSH);
	sender.bind("tcp://*:5557");

	std::cout << "Press Enter when the workers are ready: " << std::endl;
	getchar();
	std::cout << "Sending tasks to workers...\n" << std::endl;

	//  The first message is "0" and signals start of batch
	zmq::socket_t sink(ctx, ZMQ_PUSH);
	sink.connect("tcp://localhost:5558");
	zmq::message_t msg(2);
	memcpy_s(msg.data(), 2, "0", 1);
	sink.send(msg);

	//  Initialize random number generator
	srandom((unsigned)time(NULL));

	//  Send 100 tasks
	int task_nbr;
	int total_msec = 0;     //  Total expected cost in msecs
	for (task_nbr = 0; task_nbr < 100; task_nbr++) {
		//  Random workload from 1 to 100msecs
		 int workload = within(100) + 1;
		 total_msec += workload;

		 msg.rebuild(10);
		 memset(msg.data(), '\0', 10);
		 auto rs = std::format_to_n(static_cast<char*>(msg.data()), 10, "{}", workload);
		 sender.send(msg);
	}
	std::cout << "Total expected cost: " << total_msec << " msec" << std::endl;
	Sleep(1);              //  Give 0MQ time to deliver

	return 0;
}