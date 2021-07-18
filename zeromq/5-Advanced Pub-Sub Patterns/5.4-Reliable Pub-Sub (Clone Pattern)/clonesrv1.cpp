#include <thread>
#include <unordered_map>
#include "kvsimple.hpp"
#include <random>
#include <format>

int gen_num(int a, int b)
{
	std::random_device rd;  //Will be used to obtain a seed for the random number engine
	std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
	std::uniform_int_distribution<> distrib(a, b);
	return distrib(gen);
}

int main()
{
	zmqpp::context_t ctx;
	zmqpp::socket_t sock(ctx, zmqpp::socket_type::pub);
	sock.bind("tcp://*:5556");
	std::this_thread::sleep_for(std::chrono::milliseconds(200));

	int seq{};
	while (true) {
		int curseq = ++seq;
		auto key = std::format("{}", gen_num(0, 10000));
		auto body = std::format("{}", gen_num(0, 1000000));
		kvsimple kvmsg(key, curseq, body);
		kvmsg.send(sock);
	}

	return 0;
}