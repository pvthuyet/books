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

class mdcli
{
private:
	std::string broker_;
	bool verbose_{ 1 };
	size_t timeout_{ 2500 }; //msec
	int retries_{ 3 };
	zmq::context_t ctx_{1};
	std::unique_ptr<zmq::socket_t> client_{};
	int id_{};

public:
	mdcli(std::string_view broker, int id, bool verbose = true) :
		broker_{ broker },
		id_{id},
		verbose_{verbose}
	{
		connect_to_broker();
	}

	void connect_to_broker()
	{
		// connect or reconnect to broker
		client_ = std::make_unique<zmq::socket_t>(ctx_, zmq::socket_type::req);
		s_set_id(*client_, id_);
		client_->set(zmq::sockopt::linger, 0);
		client_->connect(broker_);
		if (verbose_) {
			fmt::print("[info] connecting to broker at {}...\n", broker_);
		}
	}

	//  .split send request and wait for reply
	//  Here is the {{send}} method. It sends a request to the broker and gets
	//  a reply even if it has to retry several times. It takes ownership of 
	//  the request message, and destroys it when sent. It returns the reply
	//  message, or NULL if there was no reply after multiple attempts:
	std::unique_ptr<zmsg> send(std::string_view service, std::unique_ptr<zmsg> request)
	{
		//  Prefix request with protocol frames
		//  Frame 1: "MDPCxy" (six bytes, MDP/Client x.y)
		//  Frame 2: Service name (printable string)
		request->wrap(service.data(), nullptr);
		request->wrap(MDPC_CLIENT, nullptr);
		if (verbose_) {
			fmt::print("[info] send request to {} service:\n", service);
			request->dump();
		}

		int retries_left = retries_;
		while (retries_left && !s_interrupted) {
			auto msg = std::make_unique<zmsg>(*request);
			msg->send(*client_);
			
			while (!s_interrupted) {
				// Poll socket for a reply, with timeout
				std::vector<zmq::pollitem_t> items = {
					{*client_, 0, ZMQ_POLLIN, 0}
				};
				zmq::poll(items, timeout_);

				// if we got reply, process it
				if (items[0].revents & ZMQ_POLLIN) {
					auto recv_msg = std::make_unique<zmsg>(*client_);
					if (verbose_) {
						fmt::print("[info] received reply: \n");
						recv_msg->dump();
					}
					// Don't try to handle errors, just assert noisily
					Ensures(recv_msg->parts() >= 3);

					auto header = recv_msg->pop_front();
					Ensures(0 == header.compare(reinterpret_cast<const unsigned char*>(MDPC_CLIENT)));

					auto reply_service = recv_msg->pop_front();
					Ensures(0 == reply_service.compare(reinterpret_cast<const unsigned char*>(service.data())));

					return recv_msg; // Success
				}
				else {
					if (--retries_left) {
						if (verbose_) {
							fmt::print("[warn] no reply, reconnecting...\n");
						}
						// Reconnect, and resend message
						connect_to_broker();
						zmsg msg(*request);
						msg.send(*client_);
					}
					else {
						if (verbose_) {
							fmt::print("[warn] permanent error, abandoning request\n");
						}
						break; // give up
					}
				}
			}
		}

		if (s_interrupted) {
			fmt::print("[warn] interrupt received, killing client...\n");
		}
		return {};
	}
};