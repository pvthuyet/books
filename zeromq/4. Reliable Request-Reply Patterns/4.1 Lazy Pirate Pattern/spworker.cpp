//
//  Simple Pirate worker
//  Connects REQ socket to tcp://*:5556
//  Implements worker part of LRU queueing
//
//  Andreas Hoelzlwimmer <andreas.hoelzlwimmer@fh-hagenberg.at>

#include <Windows.h>
#include <zmsg.hpp>
#include <random>
#include <fmt/core.h>
#include <charconv>

using namespace std::string_literals;
using namespace std::string_view_literals;

int gen_num(int a, int b)
{
	std::random_device rd;  //Will be used to obtain a seed for the random number engine
	std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
	std::uniform_int_distribution<> distrib(a, b);
	return distrib(gen);
}


int main(int argc, char* argv[])
{
	using namespace std::string_literals;
	using namespace std::string_view_literals;

	if (argc < 2) {
		fmt::print("Usage: spworker [id]\n");
		return 0;
	}
	zmq::context_t ctx(1);
	zmq::socket_t worker(ctx, zmq::socket_type::req);

	// set random identity to make tracing easier
	int id{};
	std::string_view strid(argv[1]);
	auto [p, ec] = std::from_chars(std::data(strid), std::data(strid) + std::size(strid), id);
	auto ident = s_set_id(worker, id);
	worker.connect("tcp://localhost:5556");

	// Tell queue we're ready for work
	fmt::print("[info] ({}) worker ready\n", ident);
	s_send(worker, "READY"s);

	int cycles = 0;
	while (1) {
		zmsg zm(worker);
		// simulate various problems, after a few cycles
		cycles++;
		if (cycles > 3 && gen_num(0, 20) == 0) {
			fmt::print("[info] ({}) simulating a crash\n", ident);
			zm.clear();
			break;
		}
		else {
			if (cycles > 3 && gen_num(0, 20) == 0) {
				fmt::print("[info] ({}) simulating CPU overload\n", ident);
				s_sleep(5);
			}
		}

		fmt::print("[info] ({}) normal reply - {}\n", ident, zm.body());
		s_sleep(1);
		zm.send(worker);
	}
	return 0;
}