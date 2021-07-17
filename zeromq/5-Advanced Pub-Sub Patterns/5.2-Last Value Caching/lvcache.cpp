#include <fmt/core.h>
#include <zmqpp/zmqpp.hpp>
#include <gsl/gsl_assert>
#include <thread>
#include <chrono>
#include <sstream>
#include <random>
#include <unordered_map>
#include <string>
#include <span>

int gen_num(int a, int b)
{
	std::random_device rd;  //Will be used to obtain a seed for the random number engine
	std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
	std::uniform_int_distribution<> distrib(a, b);
	return distrib(gen);
}

int main(int argc, char* argv[])
{
	using namespace std::string_literals;
	zmqpp::context_t ctx{};
	zmqpp::socket_t frontend(ctx, zmqpp::socket_type::sub);
	zmqpp::socket_t backend(ctx, zmqpp::socket_type::xpub);

	frontend.connect("tcp://localhost:5557");
	backend.bind("tcp://*:5558");

	// subscribe to every single topic from publisher
	frontend.set(zmqpp::socket_option::subscribe, ""s);

	// store last instance of each topic in a cache
	std::unordered_map<std::string, std::string> cache_map;
	zmqpp::poller poller{};
	poller.add(frontend);
	poller.add(backend);

	while (1) {
		poller.poll(1000);

		if (poller.events(frontend) == zmqpp::poller_t::poll_in) {
			zmqpp::message_t msg;
			frontend.receive(msg);
			std::string topic, data;
			msg >> topic >> data;
			Expects(!topic.empty());
			cache_map[topic] = data;
			backend.send(msg);
		}

		// when we get a new subscription, we pull data from the cache
		if (poller.events(backend) == zmqpp::poller_t::poll_in) {
			zmqpp::message_t msg;
			backend.receive(msg);
			Expects(msg.parts());

			// Event is 1 byte: 0=unsub or 1=sub, followed by topic
			std::span<const uint8_t> sp(msg.get<const uint8_t*>(0), msg.size(0));
			if (sp.front() == 1) {
				std::string topic(sp.begin() + 1, sp.end());
				auto found = cache_map.find(topic);
				if (found != std::cend(cache_map)) {
					zmqpp::message_t lastmsg;
					lastmsg << topic << found->second;
					backend.send(lastmsg);
				}
			}
		}
	}
	return 0;
}