#include <Windows.h>
#include <fmt/core.h>
#include <vld.h>
#include <zhelpers.hpp>
#include <zmsg.hpp>
#include <memory>
#include <thread>

class server
{
private:
	zmq::context_t ctx_;
	std::unique_ptr<zmq::socket_t> socket_;
	std::unique_ptr<std::jthread> thread_;

public:
	server() : ctx_(1)
	{}

	void start(zmq::socket_type type, std::string_view endpoint)
	{
		socket_ = std::make_unique<zmq::socket_t>(ctx_, type);
		socket_->bind(endpoint.data());

		thread_ = std::make_unique<std::jthread>([this](std::stop_token stk) {
			this->run(stk);
			});
		fmt::print("I: echo service is ready at {}\n", endpoint);
	}

	void stop()
	{
		if (thread_ && thread_->joinable()) {
			thread_->request_stop();
			thread_->join();
		}
	}

private:
	void run(std::stop_token stk)
	{
		fmt::print("server START\n");
		while (!stk.stop_requested()) {
			std::vector<zmq::pollitem_t> items = {
				{*socket_, 0, ZMQ_POLLIN, 0}
			};
			zmq::poll(items, 1000);
			if (items[0].revents & ZMQ_POLLIN) {
				auto msg = zmsg(*socket_);
				msg.send(*socket_);
			}
		}
		fmt::print("server END\n");
	}
};

int main(int argc, char* argv[])
{
	if (argc < 2) {
		fmt::print("Usage: flserver1 [endpoint]\n");
		return 0;
	}

	server srv;
	srv.start(zmq::socket_type::rep, "tcp://*:5555");

	fmt::print("Press ENTER to stop server\n");
	std::cin.get();
	srv.stop();

	return 0;
}