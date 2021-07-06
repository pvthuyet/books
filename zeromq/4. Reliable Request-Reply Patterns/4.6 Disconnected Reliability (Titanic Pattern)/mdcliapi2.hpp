#pragma once
#include <Windows.h>
#include "zmsg.hpp"
#include "mdp.hpp"
#include <memory>
#include <fmt/core.h>
#include <gsl/gsl_assert>

class mdcli
{
private:
	zmq::context_t ctx_;
	std::unique_ptr<zmq::socket_t> client_;
	std::string broker_;
	int id_;
	int verbose_{};
	int timeout_{2500};

public:
	mdcli(std::string_view broker, int id, int verbose) :
		ctx_(1),
		client_{},
		broker_{broker},
		id_{id},
		verbose_{verbose}
	{
		connect_to_broker();
	}

	void connect_to_broker()
	{
		client_ = std::make_unique<zmq::socket_t>(ctx_, zmq::socket_type::dealer);
		client_->set(zmq::sockopt::linger, 0);
		s_set_id(*client_, id_);
		client_->connect(broker_);
		fmt::print("[info] connecting to broker at {}...\n", broker_);
	}

	int send(std::string const& service, std::unique_ptr<zmsg> req)
	{
		req->wrap(service.c_str(), nullptr);
		req->wrap(MDPC_CLIENT, nullptr);
		req->wrap("", nullptr);
		fmt::print("[info] send request to {} service\n", service);
		req->dump();
		req->send(*client_);
		return 0;
	}

	std::unique_ptr<zmsg> recv()
	{
		std::vector<zmq::pollitem_t> items = {
			{*client_, 0, ZMQ_POLLIN, 0}
		};
		zmq::poll(items, timeout_);

		if (items[0].revents & ZMQ_POLLIN) {
			auto msg = std::make_unique<zmsg>(*client_);
			fmt::print("[info] received reply:\n");
			msg->dump();

			Ensures(msg->parts() >= 4);
			Ensures(0 == msg->pop_front().length());

			auto header = msg->pop_front();
			auto service = msg->pop_front();

			return msg;
		}

		if (s_interrupted) {
			fmt::print("[warn] interrupt received, killing client...\n");
		}
		else {
			fmt::print("[warn] permanent error, abondoning rquest\n");
		}
		return {};
	}
};