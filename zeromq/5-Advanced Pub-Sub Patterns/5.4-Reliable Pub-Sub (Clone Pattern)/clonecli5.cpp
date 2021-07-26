#include <thread>
#include <unordered_map>
#include "kvmsg.hpp"
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
	const std::string SUBTREE = "/client/";
	std::unordered_map<std::string, kvmsg> mp;

	zmqpp::context_t ctx;
	zmqpp::socket_t snapshot(ctx, zmqpp::socket_type::dealer);
	snapshot.connect("tcp://localhost:5556");

	zmqpp::socket_t subscriber(ctx, zmqpp::socket_type::subscribe);
	subscriber.connect("tcp://localhost:5557");
	subscriber.subscribe(SUBTREE);

	zmqpp::socket_t push(ctx, zmqpp::socket_type::push);
	push.connect("tcp://localhost:5558");

	// get state snapshot
	zmqpp::message_t snmsg;
	snmsg << "ICANHAZ?";
	snmsg << SUBTREE;
	snapshot.send(snmsg);

	int seq{};

	while (1) {
		kvmsg reply(1);
		reply.recv(snapshot);
		seq = reply.getSequence();

		if (boost::iequals(reply.getKey(), "KTHXBAI"s)) {
			fmt::print("Received snaphot {}\n", seq);
			break;
		}
		fmt::print("Receiving {}\n", seq);
		mp.insert_or_assign(reply.getKey(), reply);
	}

	zmqpp::poller_t poller;
	poller.add(subscriber);

	constexpr int INTERVAL = 2000;
	auto alarm = std::chrono::steady_clock::now();
	while (true) {
		poller.poll(1000);
		if (poller.events(subscriber) == zmqpp::poller_t::poll_in) {
			kvmsg reply2(0);
			reply2.recv(subscriber);

			if (reply2.getSequence() > seq) {
				fmt::print("Received snaphot {}\n", reply2.getSequence());
				mp.insert_or_assign(reply2.getKey(), reply2);
			}
		}

		auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - alarm).count();
		if (diff > INTERVAL) {
			auto key = std::format("{}{}", SUBTREE, gen_num(0, 10000));
			auto body = std::format("{}", gen_num(0, 1000000));

			kvmsg msg(0);
			msg.setKey(key);
			msg.setBody(body);
			msg.setProp("ttl", std::to_string(gen_num(1, 3)));
			msg.send(push);
			alarm = std::chrono::steady_clock::now();
		}
	}
	return 0;
}