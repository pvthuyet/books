#include <windows.h>
#include <zhelpers.hpp>

int main()
{
	using namespace std::string_literals;
	zmq::context_t ctx(1);

	//  Socket to receive messages on
	zmq::socket_t receiver(ctx, ZMQ_PULL);
	receiver.bind("tcp://*:5558");

	//  Socket for worker control
	zmq::socket_t controller(ctx, ZMQ_PUB);
	controller.bind("tcp://*:5559");

	//  Wait for start of batch
	s_recv(receiver);

	//  Start our clock now
	auto start = std::chrono::steady_clock::now();

	//  Process 100 confirmations
	int task_nbr;
	for (task_nbr = 0; task_nbr < 100; task_nbr++) {
		s_recv(receiver);

		if (task_nbr % 10 == 0)
			std::cout << ":";
		else
			std::cout << ".";
	}
	//  Calculate and report duration of batch
	auto end = std::chrono::steady_clock::now();
	std::chrono::duration<double> elap = end - start;
	std::cout << "\nTotal elapsed time: " << elap.count() * 1000 << " msec\n" << std::endl;

	//  Send kill signal to workers
	s_send(controller, "KILL"s);

	// finish
	s_sleep(1);
	return 0;
}