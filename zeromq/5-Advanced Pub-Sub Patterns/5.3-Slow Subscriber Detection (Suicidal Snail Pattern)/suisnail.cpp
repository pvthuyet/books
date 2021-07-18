#include <fmt/core.h>
#include <zmqpp/zmqpp.hpp>
#include <date/date.h>
#include <thread>
#include <chrono>
#include <sstream>
#include <random>
#include <vld.h>

#define MAX_ALLOWED_DELAY 1 // secs

namespace chrono = std::chrono;

int gen_num(int a, int b)
{
	std::random_device rd;  //Will be used to obtain a seed for the random number engine
	std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
	std::uniform_int_distribution<> distrib(a, b);
	return distrib(gen);
}

class subscriber
{
private:
	zmqpp::context_t& ctx_;
	zmqpp::socket_t sock_;

public:
	subscriber(zmqpp::context_t& ctx):
		ctx_{ctx},
		sock_(ctx_, zmqpp::socket_type::sub)
	{
		sock_.connect("tcp://localhost:5556");
		sock_.set(zmqpp::socket_option::subscribe, "");
	}

	void start(std::stop_token tok)
	{
		while (!tok.stop_requested()) {
			zmqpp::message_t msg;
			sock_.receive(msg);
			long long tp{};
			msg >> tp;

			auto now = date::floor<chrono::seconds>(chrono::system_clock::now()).time_since_epoch().count();
			if (now - tp > MAX_ALLOWED_DELAY) {
				fmt::print("E: subscriber cannot keep up, aborting.\n");
				break;
			}
			std::this_thread::sleep_for(chrono::milliseconds(1000 * (1 + gen_num(0, 2))));
		}
	}
};

class publisher
{
private:
	zmqpp::context_t& ctx_;
	zmqpp::socket_t sock_;

public:
	publisher(zmqpp::context_t& ctx) :
		ctx_{ctx},
		sock_{ctx, zmqpp::socket_type::pub}
	{
		sock_.bind("tcp://*:5556");
	}

	void start(std::stop_token tok)
	{
		zmqpp::message_t msg{};
		while (!tok.stop_requested()) {
			auto now = date::floor<chrono::seconds>(chrono::system_clock::now()).time_since_epoch().count();
			msg << now;
			sock_.send(msg);
			std::this_thread::sleep_for(chrono::milliseconds(1));
		}
	}
};

int main()
{
	zmqpp::context_t ctx;
	std::jthread server([&ctx](std::stop_token tok) {
		publisher pub(ctx);
		pub.start(tok);
		});

	std::jthread client([&ctx](std::stop_token tok) {
		subscriber sub(ctx);
		sub.start(tok);
		});

	std::cin.get();

	return 0;
}