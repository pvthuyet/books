#include <thread>
#include <memory>
#include <fmt/core.h>
#include <fstream>
#include <boost/algorithm/string.hpp>

#include "zmqpp/zmqpp.hpp"

constexpr size_t CHUNK_SIZE = 8;

class client
{
private:
	zmqpp::context_t& ctx_;
	std::unique_ptr<zmqpp::actor> actor_;

public:
	client(zmqpp::context_t& ctx) :
		ctx_{ ctx }
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
		dealer.set(zmqpp::socket_option::receive_high_water_mark, 1);
		dealer.connect("tcp://localhost:6000");
		size_t total{};
		size_t chunks{};

		while (1) {
			zmqpp::message_t req;
			req << "fetch";
			req << total;
			req << CHUNK_SIZE;
			dealer.send(req);

			zmqpp::message_t chunk;
			dealer.receive(chunk);
			if (chunk.parts() == 0) break; // shutting down, quit

			size_t size{};
			chunk >> size;
			if (size == 0) break;

			chunks++;
			total += size + 1;
			auto str = chunk.get<std::string>(1);
			fmt::print("{}", str);
			if (size < CHUNK_SIZE) break; // last chunk received, exit
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
		ctx_{ ctx }
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
		router.set(zmqpp::socket_option::send_high_water_mark, 1);
		router.bind("tcp://*:6000");

		std::ifstream ifs("D:\\test\\testdata.txt", std::ios::in | std::ios::ate);
		size_t size = ifs.tellg();
		ifs.seekg(0);

		while (1) {
			zmqpp::message_t req;
			router.receive(req);
			if (req.parts() == 0) break; // shutting down, quit

			// identity
			std::string identity{};
			req >> identity;

			std::string command;
			req >> command;

			size_t offset{};
			req >> offset;

			size_t chunksize{};
			req >> chunksize;

			if (boost::iequals(command, "fetch"s)) {
				if (offset >= size) {
					zmqpp::message_t reply{};
					size_t tmp = 0;
					reply << identity;
					reply << tmp;
					reply << "";
					router.send(reply);
					break;
				}
				else {
					ifs.seekg(offset);
					//size_t maxchunk = //std::min<size_t>(size - offset, CHUNK_SIZE);
					std::string str(CHUNK_SIZE, '\0');
					ifs.read(&str[0], CHUNK_SIZE);
					// do something with str
					zmqpp::message_t reply{};
					reply << identity;
					reply << str.length();
					reply.add<std::string>(str);
					router.send(reply);
				}
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