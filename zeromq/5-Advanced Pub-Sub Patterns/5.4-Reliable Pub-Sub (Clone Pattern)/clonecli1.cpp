#include <thread>
#include <unordered_map>
#include "kvsimple.hpp"
#include <random>
#include <format>
#include <fmt/core.h>
#include <limits>

int main()
{
	zmqpp::context_t ctx;
	zmqpp::socket_t sock(ctx, zmqpp::socket_type::sub);
	sock.connect("tcp://localhost:5556");
	sock.set(zmqpp::socket_option::subscribe, "");

	std::unordered_map<std::string, kvsimple> kvmap{};
	int seq{};

	while (true) {
		auto kvmsg = kvsimple::recv(sock);
		kvmap.insert(std::make_pair(kvmsg.key_, kvmsg));
		fmt::print("received: {} {} {}\n", kvmsg.key_, kvmsg.sequence_, kvmsg.body_);
		++seq;
	}
}