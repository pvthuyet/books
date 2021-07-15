#pragma once
#include <zmqpp/zmqpp.hpp>
#include <string>
#include <thread>
#include <memory>
#include <fmt/core.h>

class subscriber
{
private:
	zmqpp::context_t& ctx_;
	zmqpp::socket_t sock_;
	std::unique_ptr<std::jthread> thread_;

public:
	subscriber(zmqpp::context_t& ctx, std::string const& addr) :
		ctx_{ ctx },
		sock_(ctx_, zmqpp::socket_type::sub)
	{
		sock_.connect(addr);
		sock_.subscribe("A");
		sock_.subscribe("B");
	}

	void stop()
	{
		if (thread_ && thread_->joinable()) {
			thread_->request_stop();
			thread_->join();
		}
	}

	void start()
	{
		thread_ = std::make_unique<std::jthread>([this](std::stop_token tok) {
			
			while (true) {
				zmqpp::message_t msg;
				sock_.receive(msg);
				fmt::print("subscriber received: {}\n", msg.get<std::string>(0));
			}
			});
	}
};