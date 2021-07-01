//
// Lazy Pirate server
// Binds REQ socket to tcp://*:5555
// Like hwserver except:
// - echoes request as-is
// - randomly runs slowly, or exits to simulate a crash.
//

#include <iostream>
#include <fmt/core.h>
#include <fmt/color.h>
#include <Windows.h>
#include <zhelpers.hpp>
#include <sstream>
#include <memory>
#include <charconv>
#include <random>
#include <vld.h>

using namespace std::string_literals;
using namespace std::string_view_literals;

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
	zmq::socket_t server(ctx, zmq::socket_type::rep);
	server.bind("tcp://*:5555");

	int cycles = 0;
	while (1) {
		auto request = s_recv(server);
		cycles++;

		// Simulate various problems, after a few cycles
		if (cycles > 3 && 0 == gen_num(0, 3)) {
			fmt::print("[info] simulating a crash\n");
			break;
		}
		else {
			if (cycles > 3 && gen_num(0, 3) == 0) {
				std::cout << "[info] simulating CPU overload" << std::endl;
				s_sleep(2);
			}
		}
		fmt::print("[info] normal request ({})\n", request);
		s_sleep(1000);
		s_send(server, request);
	}

	return 0;
}
