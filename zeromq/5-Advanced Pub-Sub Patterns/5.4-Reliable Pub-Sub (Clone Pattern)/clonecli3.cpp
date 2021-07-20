#include <thread>
#include <unordered_map>
#include "kvsimple.hpp"
#include <random>
#include <format>
#include <chrono>
#include <boost/algorithm/string.hpp>
#include <fmt/core.h>

int gen_num(int a, int b)
{
	std::random_device rd;  //Will be used to obtain a seed for the random number engine
	std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
	std::uniform_int_distribution<> distrib(a, b);
	return distrib(gen);
}

int main()
{
	using namespace std::string_literals;
	std::unordered_map<std::string, kvsimple> mp{};

	zmqpp::context_t ctx;
	zmqpp::socket_t snapshot(ctx, zmqpp::socket_type::dealer);
	snapshot.connect("tcp://localhost:5556");

	zmqpp::socket_t subscriber(ctx, zmqpp::socket_type::sub);
	subscriber.connect("tcp://localhost:5557");
	subscriber.subscribe("");

	zmqpp::socket_t push(ctx, zmqpp::socket_type::push);
	push.connect("tcp://localhost:5558");

	// get state snapshot
	int seq{};
	snapshot.send("ICANHAZ?");
	while (1) {
		auto kvmsg = kvsimple::recv(snapshot);
		seq = kvmsg.sequence_;
		if (boost::iequals("KTHXBAI"s, kvmsg.key_)) {
			fmt::print("Received snaphot {}\n", kvmsg.sequence_);
			break;
		}

		fmt::print("Receiving {}\n", kvmsg.sequence_);
		mp.insert_or_assign(kvmsg.key_, kvmsg);
	}

	zmqpp::poller_t poller;
	poller.add(subscriber);

	constexpr int INTERVAL = 2000;
	auto alarm = std::chrono::steady_clock::now();
	while (1) {
		poller.poll(1000);
		if (poller.events(subscriber) == zmqpp::poller_t::poll_in) {
			auto kvmsg = kvsimple::recv(subscriber);
			if (kvmsg.sequence_ > seq) {
				seq = kvmsg.sequence_;
				fmt::print("receiving {}\n", seq);
				mp.insert_or_assign(kvmsg.key_, kvmsg);
			}
		}

		auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - alarm).count();
		if (diff > INTERVAL) {
			auto key = std::format("{}", gen_num(0, 10000));
			auto body = std::format("{}", gen_num(0, 1000000));
			kvsimple kvmsg(key, 0, body);
			kvmsg.send(push);
			alarm = std::chrono::steady_clock::now();
		}
	}

	return 0;
}