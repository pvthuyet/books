#pragma once
#include <zmqpp/zmqpp.hpp>
#include <string>

class kvsimple
{
public:
	std::string key_;
	int sequence_;
	std::string body_;

public:
	kvsimple(std::string key, int seq, std::string const& body) :
		key_{key},
		sequence_{seq},
		body_(body)
	{}

	auto operator<=>(kvsimple const& rhs) const = default;

	void send(zmqpp::socket_t& publisher, std::string identity = "")
	{
		zmqpp::message_t msg;
		if (!identity.empty()) {
			msg << identity;
		}
		msg << key_;
		msg << sequence_;
		msg << body_;
		publisher.send(msg);
	}

	static kvsimple recv(zmqpp::socket_t& updates)
	{
		zmqpp::message_t msg;
		updates.receive(msg);
		std::string key;
		msg >> key;
		int seq;
		msg >> seq;
		std::string body;
		msg >> body;
		return kvsimple(key, seq, body);
	}
};