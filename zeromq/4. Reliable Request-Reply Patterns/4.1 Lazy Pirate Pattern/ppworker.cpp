//
//  Paranoid Pirate worker
//
//
//     Andreas Hoelzlwimmer <andreas.hoelzlwimmer@fh-hagenberg.at>
//

#include <Windows.h>
#include <zmsg.hpp>
#include <iomanip>
#include <memory>
#include <random>
#include <fmt/core.h>
#include <charconv>

#define HEARTBEAT_LIVENESS  3       //  3-5 is reasonable
#define HEARTBEAT_INTERVAL  1000    //  msecs
#define INTERVAL_INIT       1000    //  Initial reconnect
#define INTERVAL_MAX       32000    //  After exponential backoff
constexpr const int NUM_RETRIES = 10;

using namespace std::string_literals;
using namespace std::string_view_literals;
int gen_num(int a, int b)
{
	std::random_device rd;  //Will be used to obtain a seed for the random number engine
	std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
	std::uniform_int_distribution<> distrib(a, b);
	return distrib(gen);
}

//  Helper function that returns a new configured socket
//  connected to the Hello World server
//
std::string identity;
static std::unique_ptr<zmq::socket_t> s_worker_socket(zmq::context_t& ctx, int id)
{
	auto worker = std::make_unique<zmq::socket_t>(ctx, zmq::socket_type::dealer);
	// set random identity to make tracing easier
	identity = s_set_id(*worker, id);
	worker->connect("tcp://localhost:5556");

	// Configure socket to not wait at close time
	worker->set(zmq::sockopt::linger, 0);

	// tell qeueue we're ready for work
	fmt::print("[info] {} worker ready\n", identity);
	s_send(*worker, "READY"s);
	return worker;
}

int main(int argc, char* argv[])
{
	if (argc < 2) {
		fmt::print("Usage: ppworker [id]\n");
		return 0;
	}
	zmq::context_t ctx(1);
	std::string_view sid(argv[1]);
	int id{};
	auto [p, ec] = std::from_chars(std::data(sid), std::data(sid) + std::size(sid), id);
	auto worker = s_worker_socket(ctx, id);

	// If liveness hits zero, queue is considered disconnected
	size_t liveness = HEARTBEAT_LIVENESS;
	size_t interval = INTERVAL_INIT;

	// Send out heartbeat at regular intervals
	auto heartbeat_at = std::chrono::steady_clock::now();
	int cycles{};
	while (1) {
		std::vector<zmq::pollitem_t> items = {
			{*worker, 0, ZMQ_POLLIN, 0}
		};
		zmq::poll(items, HEARTBEAT_INTERVAL);

		if (items[0].revents & ZMQ_POLLIN) {
			//  Get message
			//  - 3-part envelope + content -> request
			//  - 1-part "HEARTBEAT" -> heartbeat
			zmsg msg(*worker);
			
			if (msg.parts() == 3) {
				// simulate various problems, after a few cycles
				cycles++;
				if (cycles > NUM_RETRIES && gen_num(0, NUM_RETRIES) == 0) {
					fmt::print("[info] ({}) simulating a crash\n", identity);
					msg.clear();
					break;
				}
				else {
					if (cycles > NUM_RETRIES && gen_num(0, NUM_RETRIES) == 0) {
						fmt::print("[info] ({}) simulating CPU overload\n", identity);
						s_sleep(1);
					}
				}
				fmt::print("[info] ({}) normal reply - {}\n", identity, msg.body());
				msg.send(*worker);
				liveness = HEARTBEAT_LIVENESS;
				s_sleep(1);
			}
			else {
				std::string_view svbody(msg.body());
				if (msg.parts() == 1 && svbody == "HEARTBEAT"sv) {
					liveness = HEARTBEAT_LIVENESS;
				}
				else {
					fmt::print("[err] ({}) invalid message\n", identity);
					msg.dump();
				}
			}
			interval = INTERVAL_INIT;
		}
		else if (--liveness == 0) {
			fmt::print("[warn] ({}) heartbeat failure, can't reach queue\n", identity);
			fmt::print("[warn] ({}) reconnecting in {}\n", identity, interval);
			s_sleep(interval);

			if (interval < INTERVAL_MAX) {
				interval <<= 2;
			}
			worker = s_worker_socket(ctx, id);
			liveness = HEARTBEAT_LIVENESS;
		}

		// Send heartbeat to queue if it's time
		auto expiry = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - heartbeat_at);
		if (expiry.count() > HEARTBEAT_INTERVAL) {
			heartbeat_at = std::chrono::steady_clock::now();
			fmt::print("[info] ({}) worker heartbeat\n", identity);
			s_send(*worker, "HEARTBEAT"s);
		}
	}
	return 0;
}