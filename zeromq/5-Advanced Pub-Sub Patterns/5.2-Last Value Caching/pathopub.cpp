#include <fmt/core.h>
#include <zmqpp/zmqpp.hpp>
#include <thread>
#include <chrono>
#include <sstream>
#include <random>

int gen_num(int a, int b)
{
	std::random_device rd;  //Will be used to obtain a seed for the random number engine
	std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
	std::uniform_int_distribution<> distrib(a, b);
	return distrib(gen);
}

int main(int argc, char* argv[])
{
	constexpr int limit = 3;
	zmqpp::context_t ctx;
	zmqpp::socket_t publisher(ctx, zmqpp::socket_type::pub);

	if (argc < 2)
		publisher.bind("tcp://*:5556");
	else
		publisher.bind(fmt::format("tcp://*:{}", argv[1]));

	std::this_thread::sleep_for(std::chrono::seconds(1));

	// send out all 1k topic messages
	for (int i = 0; i <= limit; ++i) {
		std::stringstream ss;
		ss << std::dec << std::setw(3) << std::setfill('0') << i;
		zmqpp::message_t msg{};
		msg << ss.str() << "Save Roger";
		publisher.send(msg);
	}

	// send one random update per second
	while (1) {
		std::this_thread::sleep_for(std::chrono::seconds(1));
		std::stringstream ss;
		ss << std::dec << std::setw(3) << std::setfill('0') << gen_num(0, limit);
		zmqpp::message_t msg;
		msg << ss.str() << "Off with his head!";
		publisher.send(msg);
	}
	return 0;
}