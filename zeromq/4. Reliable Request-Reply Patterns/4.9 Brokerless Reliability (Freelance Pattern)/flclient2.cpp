#include <Windows.h>
#include <fmt/core.h>
#include <vld.h>
#include <zhelpers.hpp>
#include <zmsg.hpp>
#include <memory>
#include <thread>

#define REQUEST_TIMEOUT     100000
#define MAX_RETRIES         3       //  Before we abandon
//  If not a single service replies within this time, give up
#define GLOBAL_TIMEOUT 2500

class client
{
private:
	zmq::context_t ctx_;
	std::vector<std::string> servers_;
	zmq::socket_t sock_;
public:
	client() :
		ctx_(1),
		sock_(ctx_, zmq::socket_type::dealer)
	{}

	void add(std::string_view addr)
	{
		servers_.emplace_back(addr);
	}

	void connect()
	{
		for (auto& ep : servers_) {
			sock_.connect(ep.c_str());
		}
	}

	void send(std::string_view text, int maxwait)
	{
		static int seq{};
		zmsg req(text.data());
		req.wrap(fmt::format("{}", ++seq).c_str(), nullptr);
		req.wrap("", nullptr);

		// Blast the request to all connected servers
		for (int i = 0; i < servers_.size(); ++i) {
			zmsg msg(req);
			msg.send(sock_);
		}

		// wait for matching reply to arrive from anywhere
		// since we can poll several times, calculate each one
		while (--maxwait) {
			std::vector<zmq::pollitem_t> items = { {sock_, 0, ZMQ_POLLIN, 0} };
			zmq::poll(items, 1000);
			if (items[0].revents & ZMQ_POLLIN) {
				// reply is [empty][sequence][OK]
				zmsg reply(sock_);
				reply.unwrap2();
				auto num = reply.unwrap2();
				if (atoi(num.c_str()) == seq) {
					reply.dump();
					//fmt::print("{}\n", reply.body());
					break;
				}
			}
		}
	}

private:
	
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

	cli.connect();

	for (int i = 0; i < 10; ++i) {
		cli.send(fmt::format("hello world {}", i), 10);
		s_sleep(1000);
	}
}