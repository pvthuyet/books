#pragma once
#include <zmqpp/zmqpp.hpp>
#include <fmt/core.h>
#include <thread>

class listener
{
private:
	zmqpp::context_t& ctx_;
	zmqpp::socket_t sock_;
	std::unique_ptr<std::jthread> thread_;

public:
	listener(zmqpp::context_t& ctx, const std::string& addr) :
		ctx_{ctx},
		sock_(ctx_, zmqpp::socket_type::pair)
	{
		sock_.connect(addr);
	}

	void start()
	{
		thread_ = std::make_unique<std::jthread>([this](std::stop_token tok) {
			zmqpp::poller poller{};
			poller.add(sock_);

			while (!tok.stop_requested()) {
				poller.poll(1000);
				if (zmqpp::poller_t::poll_in == poller.events(sock_)) {
					zmqpp::message_t msg{};
					sock_.receive(msg);
					fmt::print("Listener received: {}\n", msg.get<std::string>(msg.parts()-1));
				}
			}
			});
	}

};