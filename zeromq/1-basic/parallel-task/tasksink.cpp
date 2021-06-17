//
//  Task sink in C++
//  Binds PULL socket to tcp://localhost:5558
//  Collects results from workers via that socket
//

#include <zmq.hpp>
#include <time.h>
#include <iostream>
#include <chrono>

int main()
{
	//  Prepare our context and socket
	zmq::context_t ctx(1);
	zmq::socket_t receiver(ctx, ZMQ_PULL);
	receiver.bind("tcp://*:5558");

	//  Wait for start of batch
	zmq::message_t msg;
	receiver.recv(&msg);

	// current time
	auto start = std::chrono::steady_clock::now();

	//  Process 100 confirmations
	int task_nbr;
	int total_msec = 0;     //  Total calculated cost in msecs
	for (task_nbr = 0; task_nbr < 100; task_nbr++) {
		receiver.recv(&msg);
		if (task_nbr % 10 == 0)
			std::cout << ":" << std::flush;
		else 
			std::cout << "." << std::flush;
	}

	//  Calculate and report duration of batch
	auto end = std::chrono::steady_clock::now();
	std::chrono::duration<double> elap = end - start;
	std::cout << "\nTotal elapsed time: " << elap.count() * 1000 << " msec\n" << std::endl;
	return 0;
}