#include <Windows.h>
#include <fmt/core.h>
#include <vld.h>
#include <zhelpers.hpp>
#include <zmsg.hpp>
#include <memory>
#include <thread>

#define REQUEST_TIMEOUT     100000
#define MAX_RETRIES         3       //  Before we abandon

class client
{
private:
	zmq::context_t ctx_;
	std::vector<std::string> servers_;
	std::unique_ptr<zmq::socket_t> sock_;
public:
	client() :
		ctx_(1)
	{
	}

	void add(std::string_view addr)
	{
		servers_.emplace_back(addr);
	}

	void send_request(std::string_view text)
	{
		static int seq = 0;
		zmsg msg(text.data());

		if (servers_.size() == 1) { // try 3 times if 1 server
			for (int i = 0; i < MAX_RETRIES; ++i) {
				auto reply = try_request(servers_[0], msg);
				if (reply) {
					reply->dump();
					return;
				}
				fmt::print("W: no response from {}. Retring...\n", servers_[0]);
			}
		}
		else { // try all servers
			for (auto& ep : servers_) {
				auto reply = try_request(ep, msg);
				if (reply) {
					reply->dump();
					return;
				}
				fmt::print("W: no response from {}. Retring...\n", ep);
			}
		}
		fmt::print("W: all servers are disable\n");
	}

private:
	std::unique_ptr<zmsg> try_request(std::string_view ep, zmsg& req)
	{
		fmt::print("I: trying echo service at {}\n", ep);
		sock_ = std::make_unique<zmq::socket_t>(ctx_, zmq::socket_type::dealer);
		sock_->connect(ep.data());
		// Configure socket to not wait at close time
		sock_->set(zmq::sockopt::linger, 0);

		// send request, wait safely for reply
		zmsg msg(req); // duplicate request
		static int seq = 0;
		msg.wrap(std::to_string(++seq).c_str(), nullptr);
		msg.wrap("", nullptr);

		msg.send(*sock_);

		std::vector<zmq::pollitem_t> items{
			{*sock_, 0, ZMQ_POLLIN, 0}
		};

		zmq::poll(items, REQUEST_TIMEOUT);
		if (items[0].revents & ZMQ_POLLIN) {
			return std::make_unique<zmsg>(*sock_);
		}

		return {};
	}
};

int main(int argc, char* argv[])
{
	if (argc < 2) {
		fmt::print("Usage: flclient2 [server]\n");
		return 0;
	}

	client cli;
	for (int i = 1; i < argc; ++i) {
		cli.add(argv[i]);
	}
	for (int i = 0; i < 10; ++i) {
		cli.send_request(fmt::format("hello world {}", i));
		s_sleep(1000);
	}
	return 0;
}