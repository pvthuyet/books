#include <thread>
#include <unordered_map>
#include "kvmsg.hpp"
#include <random>
#include <format>
#include <boost/algorithm/string.hpp>
#include <fmt/core.h>
#include <zmqpp/zmqpp.hpp>

class clonesrv5
{
private:
	zmqpp::context_t ctx_;
	std::unordered_map<std::string, kvmsg> kvmap_;
	zmqpp::loop loop_;
	int port_;
	int sequence_;
	zmqpp::socket_t snaphot_;
	zmqpp::socket_t publisher_;
	zmqpp::socket_t collector_;

public:
	clonesrv5() :
		ctx_{},
		kvmap_{},
		loop_{},
		port_{ 5556 },
		sequence_{ 0 },
		snaphot_(ctx_, zmqpp::socket_type::router),
		publisher_(ctx_, zmqpp::socket_type::publish),
		collector_(ctx_, zmqpp::socket_type::pull)
	{
		snaphot_.bind(std::format("tcp://*:{}", port_));
		publisher_.bind(std::format("tcp://*:{}", port_ + 1));
		collector_.bind(std::format("tcp://*:{}", port_ + 2));
	}

	void run()
	{
		loop_.add(snaphot_, [this]() -> bool {
			this->s_snapshots();
			return true;
			});
		loop_.add(collector_, [this]() -> bool {
			this->s_collector();
			return true;
			});
		loop_.add(std::chrono::milliseconds(1000), 0, [this]() -> bool {
			this->s_flush_ttl();
			return true;
			});
		loop_.start();
	}

private:
	int s_snapshots()
	{
		using namespace std::string_literals;
		zmqpp::message_t msg;
		snaphot_.receive(msg);

		// identity
		std::string identity;
		msg >> identity;

		if (!identity.empty()) {
			// Request is in second frame of message
			std::string req{};
			msg >> req;

			std::string subtree{};
			if (boost::iequals(req, "ICANHAZ?"s)) {
				msg >> subtree;
			}
			else {
				fmt::print("E: bad request, aborting\n");
			}

			if (!subtree.empty()) {
				// send state socket to client
				for (auto& [k, v] : kvmap_) {
					send_single(v, identity, subtree, snaphot_);
				}

				// Now send END message with get sequence number
				fmt::print("I: sending shapshot = {}\n", sequence_);
				kvmsg endmsg(sequence_);
				endmsg.setKey("KTHXBAI");
				endmsg.setBody(subtree);
				endmsg.send(snaphot_, identity);
			}
		}
		return 0;
	}

	int s_collector()
	{
		using namespace std::string_literals;
		kvmsg msg{0};
		msg.recv(collector_);
		msg.setSequence(++sequence_);
		msg.send(publisher_);

		auto sttl = msg.getProp("ttl");
		auto ttl = sttl.empty() ? 0ll : std::stoll(sttl);
		if (ttl > 0) {
			auto expiry = std::chrono::floor<std::chrono::milliseconds>(std::chrono::system_clock::now()) + std::chrono::milliseconds{ ttl*1000 };
			msg.setProp("ttl", std::format("{}", expiry.time_since_epoch().count()));
			msg.store(kvmap_);
			fmt::print("I: publishing update={}\n", sequence_);
		}
		return 0;
	}

	int s_flush_ttl()
	{
		for (auto& [k, v] : kvmap_) {
			flush_single(v);
		}
		return 0;
	}

	//  We call this function for each getKey-value pair in our hash table
	void send_single(kvmsg& msg, std::string const& identity, std::string const& subtree, zmqpp::socket_t& socket)
	{
		if (msg.getKey().starts_with(subtree)) {
			msg.send(socket, identity);
		}
	}

	//  If getKey-value pair has expired, delete it and publish the
	//  fact to listening clients.
	void flush_single(kvmsg& msg)
	{
		auto sttl = msg.getProp("ttl");
		auto ttl = sttl.empty() ? 0 : std::stoll(sttl);
		auto now = std::chrono::floor<std::chrono::milliseconds>(std::chrono::system_clock::now()).time_since_epoch().count();

		if (ttl > 0 && now >= ttl) {
			msg.setSequence(++sequence_);
			msg.setBody("");
			msg.send(publisher_);
			msg.store(kvmap_);
			fmt::print("I: publishing delete={}\n", sequence_);
		}
	}
};

int main()
{
	clonesrv5 srv;
	srv.run();
	return 0;
}