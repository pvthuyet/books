//  Freelance server - Model 3
//  Uses an ROUTER/ROUTER socket but just one thread

//#include <Windows.h>
#include <fmt/core.h>
#include <vld.h>
//#include <zhelpers.hpp>
//#include <zmsg.hpp>
#include <memory>
#include <thread>
#include <gsl/gsl_assert>
#include <zmqpp/zmqpp.hpp>

class server
{
private:
	zmqpp::context_t ctx_;
	std::unique_ptr<zmqpp::socket_t> socket_;
	std::unique_ptr<std::jthread> thread_;

public:
	server() : ctx_{}
	{}

	void start(std::string_view port)
	{
		std::string bind_endpoint = fmt::format("tcp://*:{}", port);
		std::string connect_endpoint = fmt::format("tcp://127.0.0.1:{}", port);
		socket_ = std::make_unique<zmqpp::socket_t>(ctx_, zmqpp::socket_type::router);

		socket_->set(zmqpp::socket_option::identity, connect_endpoint);
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
		zmqpp::poller_t poller{};
		poller.add(*socket_);
		while (!stk.stop_requested()) {
			poller.poll(1000);
			if (poller.events(*socket_) & zmqpp::poller_t::poll_in) {
				zmqpp::message_t req{};
				socket_->receive(req);

				// Frame 0: identity of client
				// Frame 1: PING or client control frame
				// Frame 2: request body
				auto ident = req.get<std::string>(0);
				auto control = req.get<std::string>(1);

				zmqpp::message_t reply{};
				if (control == "PING") {
					reply << "PONG";
				}
				else {
					auto body = req.get<std::string>(2);
					reply << control;
					reply << fmt::format("{} => {}", body, "OK");
				}
				reply.push_front(ident);
				socket_->send(reply);
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
	srv.start(argv[1]);

	fmt::print("Press ENTER to stop server\n");
	std::cin.get();
	srv.stop();

	return 0;
}