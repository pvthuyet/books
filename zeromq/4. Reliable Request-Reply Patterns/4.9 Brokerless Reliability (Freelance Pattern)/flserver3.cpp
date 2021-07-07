//  Freelance server - Model 3
//  Uses an ROUTER/ROUTER socket but just one thread

#include <Windows.h>
#include <fmt/core.h>
#include <vld.h>
#include <zhelpers.hpp>
#include <zmsg.hpp>
#include <memory>
#include <thread>
#include <gsl/gsl_assert>

class server
{
private:
	zmq::context_t ctx_;
	std::unique_ptr<zmq::socket_t> socket_;
	std::unique_ptr<std::jthread> thread_;

public:
	server() : ctx_(1)
	{}

	void start(std::string_view port)
	{
		std::string bind_endpoint = fmt::format("tcp://*:{}", port);
		std::string connect_endpoint = fmt::format("tcp://localhost:{}", port);
		socket_ = std::make_unique<zmq::socket_t>(ctx_, zmq::socket_type::router);

		socket_->set(zmq::sockopt::routing_id, connect_endpoint);
		socket_->bind(bind_endpoint.data());

		thread_ = std::make_unique<std::jthread>([this](std::stop_token stk) {
			this->run(stk);
			});
		fmt::print("I: echo service is ready at {}\n", bind_endpoint);
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
			zmsg req(*socket_);
			fmt::print("\n****************** received request\n");
			req.dump();

			// Frame 0: identity of client
			// Frame 1: PING or client control frame
			// Frame 2: request body
			auto ident = req.unwrap2();
			auto control = req.unwrap2();

			zmsg reply{};
			if (control == "PING") {
				reply.append("PONG");
			}
			else {
				reply.append(control.c_str());
				reply.append("OK");
			}
			reply.wrap(ident.c_str(), nullptr);

			fmt::print("\n****************** reply\n");
			reply.dump();
			reply.send(*socket_);
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
	srv.start(argv[1]);

	fmt::print("Press ENTER to stop server\n");
	std::cin.get();
	srv.stop();

	return 0;
}