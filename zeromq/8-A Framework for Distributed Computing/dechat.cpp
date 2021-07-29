#include <thread>
#include <memory>
#include <fmt/core.h>
#include <fstream>
#include <boost/algorithm/string.hpp>

#include "zmqpp/zmqpp.hpp"

class dechat
{
private:
	zmqpp::context_t& ctx_;
	std::unique_ptr<zmqpp::actor> actor_;

public:
	dechat(zmqpp::context_t& ctx) :
		ctx_{ctx}
	{}

	void start()
	{
		zmqpp::socket_t broadcaster(ctx_, zmqpp::socket_type::publish);
		broadcaster.bind(std::format("tcp://*:9000"));

		actor_ = std::make_unique<zmqpp::actor>([this](zmqpp::socket_t* pipe) -> bool {
			return listener(pipe);
			});

		while (1) {
			std::string line;
			std::cin >> line;
			if (line.empty()) break;
			broadcaster.send(std::format("{}:{}", "Joe", line));
		}
	}

private:
	bool listener(zmqpp::socket_t* pipe)
	{
		pipe->send(zmqpp::signal::ok);

		zmqpp::socket_t lis(ctx_, zmqpp::socket_type::subscribe);

		for (int addr = 1; addr < 255; ++addr) {
			lis.connect(std::format("tcp://192.168.1.{}:9000", addr));
		}

		lis.set(zmqpp::socket_option::subscribe, "");
		while (1) {
			std::string msg;
			lis.receive(msg);
			fmt::print("{}\n", msg);
		}
		return true;
	}
};

int main()
{
	zmqpp::context_t ctx;
	dechat chat(ctx);
	chat.start();
	return 1;
}