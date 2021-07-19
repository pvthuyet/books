#include <thread>
#include <unordered_map>
#include "kvsimple.hpp"
#include <random>
#include <format>
#include <boost/algorithm/string.hpp>
#include <fmt/core.h>

int main()
{
	using namespace std::string_literals;
	zmqpp::context_t ctx;
	zmqpp::socket_t snapshot(ctx, zmqpp::socket_type::dealer);
	snapshot.connect("tcp://localhost:5556");

	zmqpp::socket_t subscriber(ctx, zmqpp::socket_type::sub);
	subscriber.set(zmqpp::socket_option::subscribe, "");
	subscriber.connect("tcp://localhost:5557");

	std::unordered_map<std::string, kvsimple> mp{};

	snapshot.send("ICANHAZ?");
	int seq{};
	while (1) {
		auto kvmsg = kvsimple::recv(snapshot);
		seq = kvmsg.sequence_;
		if (boost::iequals(kvmsg.key_, "KTHXBAI"s)) {
			fmt::print("Received snapshot = {}\n", kvmsg.sequence_);
			break;
		}

		fmt::print("Receiving = {}\n", kvmsg.sequence_);
		mp.insert_or_assign(kvmsg.key_, kvmsg);
	}

	// now apply pending updates, discard out-of-get sequence messages
	while (true) {
		auto kvmsg = kvsimple::recv(subscriber);
		if (kvmsg.sequence_ > seq) {
			seq = kvmsg.sequence_;
			fmt::print("Receiving = {}\n", kvmsg.sequence_);
			mp.insert_or_assign(kvmsg.key_, kvmsg);
		}
	}

	return 0;
}