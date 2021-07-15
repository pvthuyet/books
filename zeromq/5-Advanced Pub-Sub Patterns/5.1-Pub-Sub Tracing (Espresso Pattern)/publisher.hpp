#pragma once
#include <zmqpp/zmqpp.hpp>
#include <fmt/core.h>
#include <string>
#include <thread>
#include <memory>

class publisher
{
private:
	zmqpp::context_t& ctx_;
	zmqpp::socket_t sock_;
	std::unique_ptr<std::jthread> thread_;

public:
	publisher(zmqpp::context_t& ctx, const std::string& addr) :
		ctx_{ctx},
		sock_(ctx_, zmqpp::socket_type::pub)
	{
		sock_.bind(addr);
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
			int i{};
			while (!tok.stop_requested()) {
				zmqpp::message_t msg;
				msg << fmt::format("{} - hello {}", "A", ++i);
				sock_.send(msg);
				std::this_thread::sleep_for(std::chrono::milliseconds(1000));
			}
			});
	}
};