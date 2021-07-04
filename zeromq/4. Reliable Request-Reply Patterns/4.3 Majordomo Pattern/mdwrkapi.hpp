#pragma once

#include <iostream>
#include <fmt/core.h>
#include <fmt/color.h>
#include <Windows.h>
#include <zmsg.hpp>
#include "mdp.hpp"
#include <sstream>
#include <memory>
#include <charconv>
#include <vld.h>
#include <string>
#include <gsl/gsl_assert>

// Reliability parameters
#define HEARTBEAT_LIVENESS 3 // 3-5 is reasonable

// Structure of our class
// We access these properties only via class methods
class mdwrk
{
private:
	std::string broker_;
	std::string service_;
	int id_;
	int verbose_{ 1 };
	zmq::context_t ctx_{1};
	std::unique_ptr<zmq::socket_t> worker_; // socket to broker

	// Heartbeat management
	std::chrono::steady_clock::time_point heartbeat_at_; // when to send HEARTBEAT
	size_t liveness_; // how many attempts left
	int heartbeat_{2500}; // heartbeat delay, msec
	int reconnect_{ 2500 }; // reconnect delay, msec

	// internal state
	bool expect_reply_{false}; // zero only at start

	// return address, if any
	std::string replay_to_;

public:
	mdwrk(std::string_view broker, std::string_view service, int id, int verbose)
	{
		broker_ = broker;
		service_ = service;
		id_ = id;
		verbose_ = verbose;
		connect_to_broker();
	}

	void connect_to_broker()
	{
		worker_ = std::make_unique<zmq::socket_t>(ctx_, zmq::socket_type::dealer);
		worker_->set(zmq::sockopt::linger, 0);
		s_set_id(*worker_, id_);
		worker_->connect(broker_);

		if (verbose_) {
			fmt::print("[info] connecting to broker at {} ...\n", broker_);
		}

		// Register service with broker
		send_to_broker(MDPW_READY, service_, nullptr);

		// if liveness hits zero, queue is considered disconnected
		liveness_ = HEARTBEAT_LIVENESS;
		heartbeat_at_ = std::chrono::steady_clock::now();
	}

	void send_to_broker(std::string_view command, std::string_view option, std::unique_ptr<zmsg> inMsg)
	{
		auto msg = inMsg ? std::make_unique<zmsg>(*inMsg) : std::make_unique<zmsg>();

		// stack protocal envelope to start of message
		if (!option.empty()) {
			msg->wrap(option.data(), nullptr);
		}

		msg->wrap(command.data(), nullptr);
		msg->wrap(MDPW_WORKER, nullptr);
		msg->wrap("", nullptr);

		if (verbose_) {
			fmt::print("[info] sending {} to broker\n", mdps_commands[static_cast<int>(*command.data())]);
			msg->dump();
		}
		msg->send(*worker_);
	}

	std::unique_ptr<zmsg> recv(std::unique_ptr<zmsg> reply)
	{
		Ensures(reply || !expect_reply_);
		if (reply) {
			Ensures(replay_to_.size() > 0);
			reply->wrap(replay_to_.c_str(), "");
			replay_to_ = "";
			send_to_broker(MDPW_REPLY, "", std::move(reply));
		}
		expect_reply_ = true;

		while (!s_interrupted) {
			std::vector<zmq::pollitem_t> items = {
				{*worker_, 0, ZMQ_POLLIN, 0}
			};
			zmq::poll(items, heartbeat_);

			if (items[0].revents & ZMQ_POLLIN) {
				auto msg = std::make_unique<zmsg>(*worker_);
				if (verbose_) {
					fmt::print("[info] received message from broker:\n");
					msg->dump();
				}
				liveness_ = HEARTBEAT_LIVENESS;

				// don't try to handle error, just assert noisily
				Expects(msg->parts() >= 3);

				auto empty = msg->pop_front();
				Expects(empty.empty());

				auto header = msg->pop_front();
				Expects(0 == header.compare(reinterpret_cast<const unsigned char*>(MDPW_WORKER)));

				auto command = msg->pop_front();
				if (0 == command.compare(reinterpret_cast<const unsigned char*>(MDPW_REQUEST))) {
					// we should pop and save as many addresses as there are
					// up to a null part, but for now, just save one...
					replay_to_ = msg->unwrap2();
					return msg; // We have a request to proccess
				}
				else if (0 == command.compare(reinterpret_cast<const unsigned char*>(MDPW_HEARTBEAT))) {
					// Do nothing for heartbeats
				}
				else if (0 == command.compare(reinterpret_cast<const unsigned char*>(MDPW_DISCONNECT))) {
					connect_to_broker();
				}
				else {
					fmt::print("[err] invalid input message {}\n", mdps_commands[(int)*(command.c_str())]);
					msg->dump();
				}
			}
			else if (--liveness_ == 0) {
				if (verbose_) {
					fmt::print("[warn] disconnected from broker - retrying...\n");
				}
				s_sleep(reconnect_);
				connect_to_broker();
			}

			// send HEARTBEAT if it's time
			auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - heartbeat_at_);
			if (diff.count() > heartbeat_) {
				send_to_broker(MDPW_HEARTBEAT, "", nullptr);
				heartbeat_at_ = std::chrono::steady_clock::now();
			}
		}

		if (s_interrupted) {
			fmt::print("[warn] interrupt received, killing worker...\n");
		}
		return {};
	}
};