#include <thread>
#include <unordered_map>
#include "kvsimple.hpp"
#include <random>
#include <format>
#include <boost/algorithm/string.hpp>
#include <fmt/core.h>

int gen_num(int a, int b)
{
	std::random_device rd;  //Will be used to obtain a seed for the random number engine
	std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
	std::uniform_int_distribution<> distrib(a, b);
	return distrib(gen);
}

class state_manager
{
private:
	std::unordered_map<std::string, kvsimple> mp;
	zmqpp::context_t& ctx_;
	std::unique_ptr<zmqpp::actor> actor_;

public:
	state_manager(zmqpp::context_t& ctx) :
		mp{},
		ctx_{ctx}
	{
		actor_ = std::make_unique<zmqpp::actor>([this](zmqpp::socket_t* pipe) -> bool {
			return run(pipe);
			});
	}

	zmqpp::socket_t* get_pipe() { return actor_->pipe(); }

private:
	bool run(zmqpp::socket_t* pipe)
	{
		using namespace std::string_literals;
		pipe->send(zmqpp::signal::ok);

		zmqpp::socket_t snapshot(ctx_, zmqpp::socket_type::router);
		snapshot.bind("tcp://*:5556");
		fmt::print("Start listening at port 5556\n");

		zmqpp::poller_t poller;
		poller.add(*pipe);
		poller.add(snapshot);

		int state_seq{};
		while (true) {
			poller.poll(1000);

			// apply state updates from main thread
			if (poller.events(*pipe) == zmqpp::poller_t::poll_in) {
				kvsimple kvmsg = kvsimple::recv(*pipe);
				this->mp.insert_or_assign(kvmsg.key_, kvmsg);
				state_seq = kvmsg.sequence_;
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

				for (auto&& e : mp) {
					fmt::print("sending message: {} {} {}\n", e.second.key_, e.second.sequence_, e.second.body_);
					e.second.send(snapshot, identity);
				}

				// now send end message with get_sequence number
				fmt::print("Sending state snapshot = {}\n", state_seq);
				kvsimple msgstate("KTHXBAI", state_seq, "");
				msgstate.send(snapshot, identity);
			}
		}
		return true;
	}
};

int main()
{
	zmqpp::context_t ctx;
	zmqpp::socket_t publisher(ctx, zmqpp::socket_type::pub);
	publisher.bind("tcp://*:5557");

	state_manager smgr(ctx);
	int seq{};
	fmt::print("Start listening at port 5557\n");
	while (1) {
		int curseq = ++seq;
		auto key = std::format("{}", gen_num(0, 10000));
		auto body = std::format("{}", gen_num(0, 1000000));

		kvsimple kvmsg(key, curseq, body);
		kvmsg.send(publisher);
		kvmsg.send(*smgr.get_pipe());
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	}

	return 0;
}