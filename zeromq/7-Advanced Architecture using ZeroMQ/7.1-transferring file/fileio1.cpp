#include <thread>
#include <memory>
#include <fmt/core.h>
#include <fstream>
#include <boost/algorithm/string.hpp>

#include "zmqpp/zmqpp.hpp"

constexpr int CHUNK_SIZE = 7;

class client
{
private:
	zmqpp::context_t& ctx_;
	std::unique_ptr<zmqpp::actor> actor_;

public:
	client(zmqpp::context_t& ctx) :
		ctx_{ctx}
	{
	}

	void start()
	{
		actor_ = std::make_unique<zmqpp::actor>([this](zmqpp::socket_t* pipe) {
			return run(pipe);
			});
	}

	void wait()
	{
		zmqpp::message_t msg;
		actor_->pipe()->receive(msg);
		std::string ok;
		msg >> ok;
		fmt::print("client::wait: {}\n", ok);
	}

private:
	bool run(zmqpp::socket_t* pipe)
	{
		pipe->send(zmqpp::signal::ok);
		zmqpp::socket_t dealer(ctx_, zmqpp::socket_type::dealer);
		dealer.set(zmqpp::socket_option::identity, "cli1");
		dealer.connect("tcp://localhost:6000");
		dealer.send("fetch");

		uint64_t total{};
		uint64_t chunks{};
		while (1) {
			zmqpp::message_t msg;
			dealer.receive(msg);
			chunks++;

			int size{};
			msg >> size;
			total += size;
			if (size == 0) break;

			auto str = msg.get<std::string>(1);
			fmt::print("{}", str);
		}
		fmt::print("\n{} chunks received, {} bytes\n", chunks, total);
		pipe->send("OK");
		return true;
	}
};

class server
{
	zmqpp::context_t& ctx_;
	std::unique_ptr<std::thread> thread_;

public:
	server(zmqpp::context_t& ctx) :
		ctx_{ctx}
	{}

	void start()
	{
		thread_ = std::make_unique<std::thread>([this]() {
			run();
			});
	}

	void wait()
	{
		if (thread_ && thread_->joinable()) {
			thread_->join();
		}
	}

private:
	void run()
	{
		using namespace std::string_literals;
		zmqpp::socket_t router(ctx_, zmqpp::socket_type::router);
		router.set(zmqpp::socket_option::send_high_water_mark, 1000);
		router.bind("tcp://*:6000");
		while (1) {
			zmqpp::message_t msg;
			router.receive(msg);

			// identity
			std::string identity{};
			msg >> identity;

			std::string command;
			msg >> command;
			if (boost::iequals(command, "fetch"s)) {
				std::ifstream ifs("D:\\test\\testdata.txt", std::ios::in | std::ios::ate);
				auto size = ifs.tellg();
				ifs.seekg(0);

				size_t remain = size;
				int maxchunk = std::min<int>(remain, CHUNK_SIZE);
				std::string str(maxchunk, '\0');
				do {
					ifs.read(&str[0], maxchunk);
					// do something with str
					zmqpp::message_t reply{};
					reply << identity;
					reply << maxchunk;
					reply << str;
					router.send(reply);

					// reset str
					str.assign(maxchunk, '\0');
					remain -= maxchunk;
					maxchunk = std::min<size_t>(remain, CHUNK_SIZE);
				} while (remain > 0);

				zmqpp::message_t done;
				done << identity;
				done << 0;
				router.send(done);
				break;
			}
		}
	}
};

int main()
{
	zmqpp::context_t ctx;

	// server
	server srv(ctx);
	srv.start();

	client cli{ ctx };
	cli.start();
	cli.wait();
	srv.wait();
}