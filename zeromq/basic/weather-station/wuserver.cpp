//
//  Weather update server in C++
//  Binds PUB socket to tcp://*:5556
//  Publishes random weather updates
//
#include <zmq.hpp>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <format>
#include <thread>
#include <random>

#if (defined (WIN32))
#include <Windows.h>
#include <zhelpers.hpp>
#endif

#define within(num) (int) ((float) num * random () / (RAND_MAX + 1.0))
int gen_num(int a, int b)
{
	std::random_device rd;  //Will be used to obtain a seed for the random number engine
	std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
	std::uniform_int_distribution<> distrib(a, b);
	return distrib(gen);
}

int main()
{
	zmq::context_t ctx(1);
	zmq::socket_t pub(ctx, ZMQ_PUB);
	pub.bind("tcp://*:5556");
	//pub.bind("ipc://weather.ipc");

	srandom((unsigned)time(NULL));
	while (1) {
		int zipcode, temperature, relhumidity;
		zipcode = gen_num(10001, 10005);
		temperature = within(215) - 80;
		relhumidity = within(50) + 10;

		//  Send message to all subscribers
		//zipcode = 10001;
		char buf[20] = {};
		auto rs = std::format_to_n(buf, sizeof buf, "{:5d} {} {}", zipcode, temperature, relhumidity);
		zmq::message_t msg(buf, rs.size);
		std::cout << buf << std::endl;
		pub.send(msg);
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
	return EXIT_SUCCESS;
}