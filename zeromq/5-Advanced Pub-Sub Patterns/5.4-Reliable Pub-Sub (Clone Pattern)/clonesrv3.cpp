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
	std::unordered_map<std::string, kvsimple> mp{};

	zmqpp::context_t ctx;
	zmqpp::socket_t snapshot(ctx, zmqpp::socket_type::router);
	snapshot.bind("tcp://*:5556");

	zmqpp::socket_t publisher(ctx, zmqpp::socket_type::pub);
	publisher.bind("tcp://*:5557");

	zmqpp::socket_t collector(ctx, zmqpp::socket_type::pull);
	collector.bind("tcp://*:5558");

	zmqpp::poller_t poller;
	poller.add(collector);
	poller.add(snapshot);

	int seq{};
	while (true) {
		poller.poll(1000);

		// apply state updates from main thread
		if (poller.events(collector) == zmqpp::poller_t::poll_in) {
			auto kvmsg = kvsimple::recv(collector);
			kvmsg.sequence_ = ++seq;
			kvmsg.send(publisher);
			mp.insert_or_assign(kvmsg.key_, kvmsg);
			fmt::print("I: publishing update {}\n", seq);
		}

		// execute state snapshot request
		if (poller.events(snapshot) == zmqpp::poller_t::poll_in) {
			zmqpp::message_t msg;
			snapshot.receive(msg);
			std::string identity;
			msg >> identity;

			std::string req;
			msg >> req;

			if (!boost::iequals(req, "ICANHAZ?"s)) {
				fmt::print("E: bad request, aborting\n");
				break;
			}

			for (auto& [k,v] : mp) {
				fmt::print("Sending message {}\n", v.sequence_);
				v.send(snapshot, identity);
			}

			// now send end message with sequence number
			fmt::print("Sending state snapshot {}\n", seq);
			kvsimple endmsg("KTHXBAI", seq, "");
			endmsg.send(snapshot, identity);
		}
	}

	return 0;
}