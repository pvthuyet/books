#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_set>
#include <random>
#include <exception>
#include <algorithm>
#include <date/date.h>
#include <Windows.h>
#include <zmsg.hpp>
#include <fmt/core.h>

//  If no server replies within this time, abandon request
#define GLOBAL_TIMEOUT  3000    //  msecs
//  PING interval for servers we think are alive
#define PING_INTERVAL   2000    //  msecs
//  Server considered dead if silent for this long
#define SERVER_TTL      6000    //  msecs

//  .split API structure
//  This API works in two halves, a common pattern for APIs that need to
//  run in the background. One half is an frontend object our application
//  creates and works with; the other half is a backend "agent" that runs
//  in a background thread. The frontend talks to the backend over an
//  inproc pipe socket:

//  Structure of our frontend class

int gen_num(int a, int b)
{
	std::random_device rd;  //Will be used to obtain a seed for the random number engine
	std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
	std::uniform_int_distribution<> distrib(a, b);
	return distrib(gen);
}

namespace flcaip {
class server
{
public:
	using tp = std::chrono::time_point<std::chrono::system_clock>;
	// simple class for one server we talk to

	// server identity/endpoint
	std::string endpoint_;

	// true if know to be alive
	bool alive_;

	// Next ping at this time
	tp pingat_;

	// Expires at this time
	tp expires_;

public:
	server(std::string_view ep) :
		endpoint_(ep)
	{
		refresh(true);
	}

	void refresh(bool alive)
	{
		namespace chr = std::chrono;
		alive_ = alive;
		if (alive_) {
			pingat_ = chr::floor<chr::milliseconds>(chr::system_clock::now()) + chr::milliseconds{ PING_INTERVAL };
			expires_ = chr::floor<chr::milliseconds>(chr::system_clock::now()) + chr::milliseconds{ SERVER_TTL };
		}
	}

	void ping(zmq::socket_t & sock)
	{
		namespace chr = std::chrono;
		auto now = chr::floor<chr::milliseconds>(chr::system_clock::now());
		if (now > pingat_) {
			zmsg msg("PING");
			msg.wrap(endpoint_.c_str(), nullptr);
			msg.send(sock);
			pingat_ = now + chr::milliseconds{ PING_INTERVAL };
		}
	}
};

class agent
{
	using tp = std::chrono::time_point<std::chrono::system_clock>;
public:
	// We build the agent as a class that's capable of processing messages
	// coming in from its various sockets:
	// 
	// Simple class for one background agent

	// socket to talk back to application
	zmq::socket_t& pipe_;

	// socket to talk to servers
	std::unique_ptr<zmq::socket_t> router_;

	// server we've connected to
	std::vector<server> servers_;

	// Server we know are alive
	std::vector<std::reference_wrapper<server>> actives_;

	// Number of request ever sent
	int sequence_;

	// current request if any
	std::unique_ptr<zmsg> request_;

	// Current reply if any
	// ...

	// Timeout for request/reply
	tp expires_;

public:
	agent(zmq::context_t& ctx, zmq::socket_t& pipe) :
		agent(ctx, pipe, "")
	{
		using namespace std::string_literals;
		std::string name = "CLIENT"s + std::to_string(gen_num(1, 1000));
		router_->set(zmq::sockopt::routing_id, name);
	}

	agent(zmq::context_t& ctx, zmq::socket_t& pipe, std::string_view name) :
		pipe_{ pipe }
	{
		router_ = std::make_unique<zmq::socket_t>(ctx, zmq::socket_type::router);
		if (!name.empty()) {
			router_->set(zmq::sockopt::routing_id, name);
		}
	}

	void control_message(zmsg& msg)
	{
		using namespace std::string_literals;
		// this method processes on message from our frontend class
		// it's going to be CONNECT or REQUEST
		std::string command = msg.unwrap2();
		if (command == "CONNECT") {
			auto endpoint = msg.unwrap2();
			fmt::print("I: connecting to {}...\n", endpoint);
			router_->connect(endpoint);

			servers_.push_back(server(endpoint));
			actives_.emplace_back(servers_.back());
		}
		else if (command == "REQUEST") {
			if (request_) {
				throw std::runtime_error("request-reply cycle");
			}

			// prefix request with sequence number and empty envelope
			msg.wrap(fmt::format("{}", ++sequence_).c_str(), nullptr);

			// take ownership of request message
			request_ = std::make_unique<zmsg>(msg);

			// request expires after global timeout
			expires_ = std::chrono::floor<std::chrono::milliseconds>(std::chrono::system_clock::now()) + std::chrono::milliseconds{ GLOBAL_TIMEOUT };
		}
	}

	void router_message(zmsg& reply)
	{
		// this method processes one message from a connected server

		// Frame 0 is server that replied
		auto endpoint = reply.unwrap2();
		auto found = std::ranges::find_if(servers_, [&endpoint](auto const& item) {
			return endpoint == item.endpoint_;
			});
		if (found != std::cend(servers_) && !found->alive_) {
			actives_.emplace_back(*found);
			found->refresh(true);
		}

		// Frame 1 may be sequence number for reply
		int seqnum{};
		auto seqstr = reply.unwrap2();
		auto [p, ec] = std::from_chars(std::data(seqstr), std::data(seqstr) + std::size(seqstr), seqnum);
		if (seqnum == sequence_) {
			reply.wrap("OK", nullptr);
			reply.send(pipe_);
		}
	}
};

class free_lance_client
{
public:
	zmq::context_t ctx_;

	// pipe through to flcliapi agent
	zmq::socket_t pipe_;

};
}