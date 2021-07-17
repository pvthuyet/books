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
	zmqpp::socket_t subscriber(ctx, zmqpp::socket_type::sub);

	if (argc < 2)
		subscriber.connect("tcp://localhost:5556");
	else 
		subscriber.connect(fmt::format("tcp://localhost:{}", argv[1]));

	std::stringstream ss;
	ss << std::dec << std::setw(3) << std::setfill('0') << gen_num(0, limit);
	fmt::print("topic: {}\n", ss.str());

	subscriber.set(zmqpp::socket_option::subscribe, ss.str());

	while (1) {
		zmqpp::message_t msg;
		subscriber.receive(msg);
		fmt::print("{} - {}\n", msg.get<std::string>(0), msg.get<std::string>(1));
	}
	return 0;
}