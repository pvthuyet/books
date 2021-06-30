#include <Windows.h>
#include <zhelpers.hpp>
#include <fmt/format.h>
#include <random>

int gen_num(int a, int b)
{
	std::random_device rd;  //Will be used to obtain a seed for the random number engine
	std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
	std::uniform_int_distribution<> distrib(a, b);
	return distrib(gen);
}

int main(int argc, char * argv[])
{
	using namespace std::string_literals;
	using namespace std::string_view_literals;

	if (argc < 2) {
		fmt::print("syntax: peering1 me [you]...\n");
		return 0;
	}

	std::string self = argv[1];
	fmt::print("I: preparing broker at {}...\n", self);
	zmq::context_t ctx;

	// Binding state backend to endpoint
	zmq::socket_t statebe(ctx, ZMQ_PUB);
	statebe.bind(fmt::format("tcp://*:{}", argv[1]));

	// connect statefe to all peers
	zmq::socket_t statefe(ctx, ZMQ_SUB);
	statefe.set(zmq::sockopt::subscribe, ""sv);

	for (int i = 2; i < argc; ++i) {
		std::string peer = argv[i];
		fmt::print("I: connecting to state backend at '{}'\n", peer);
		statefe.connect(fmt::format("tcp://localhost:{}", peer));
	}

	// The main loop sends out status messages to peers, and collects
	// status message back from peers. The zmq_pool timeout defines
	// our own heartbeat
	while (true) {
		// Poll for activity or 1 second timeout
		zmq::pollitem_t items[] = {
			{statefe, 0, ZMQ_POLLIN, 0}
		};
		int rc = zmq::poll(&items[0], 1, 1000);
		if (rc == -1) break; // Interrupted

		// Handle incomming status messages
		if (items[0].revents & ZMQ_POLLIN) {
			auto peer_name = s_recv(statefe);
			auto available = s_recv(statefe);
			fmt::print("{} - {} workers free\n", peer_name, available);
		}
		else {
			// Send random values for worker availability
			s_sendmore(statebe, self);
			s_send(statebe, fmt::format("{}", gen_num(0, 10)));
		}
	}
}