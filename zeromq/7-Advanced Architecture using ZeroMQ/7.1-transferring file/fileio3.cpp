#include <thread>
#include <memory>
#include <fmt/core.h>
#include <fstream>
#include <boost/algorithm/string.hpp>

#include "zmqpp/zmqpp.hpp"

constexpr int PIPELINE = 3;
constexpr int CHUNK_SIZE = 11;

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
		using namespace std::string_literals;
		pipe->send(zmqpp::signal::ok);
		zmqpp::socket_t dealer(ctx_, zmqpp::socket_type::dealer);
		dealer.set(zmqpp::socket_option::identity, "cli1");
		dealer.connect("tcp://localhost:6000");

		// up to this many chunks in transit
		size_t credit = PIPELINE;

		size_t total{0};
		size_t chunks{0};
		size_t offset{0};

		while (1) {
			while (credit > 0) {
				// ask for next chunk
				zmqpp::message_t req;
				req.add<std::string>("fetch");
				req.add<size_t>(offset);
				req.add<size_t>(CHUNK_SIZE);
				dealer.send(req);
				offset += CHUNK_SIZE + 1;
				credit--;
			}

			zmqpp::message_t recv;
			dealer.receive(recv);
			size_t size = recv.get<size_t>(0);
			if (0 == size) break;

			chunks++;
			credit++;
			auto str = recv.get<std::string>(1);
			fmt::print("{} - {}", chunks, str);

			total += size;
			//if (size < CHUNK_SIZE) break;
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
		router.set(zmqpp::socket_option::send_high_water_mark, PIPELINE);
		router.set(zmqpp::socket_option::receive_high_water_mark, PIPELINE);
		router.bind("tcp://*:6000");

		std::ifstream ifs("D:\\test\\testdata.txt", std::ios::in | std::ios::ate);
		size_t size = ifs.tellg();
		ifs.seekg(0);

		while (1) {
			zmqpp::message_t req;
			router.receive(req);
			if (req.parts() == 0) break; // shutting down, quit

			// identity
			auto identity	= req.get<std::string>(0);
			auto command	= req.get<std::string>(1);
			auto offset		= req.get<size_t>(2);
			auto chunksize	= req.get<size_t>(3);

			if (boost::iequals(command, "fetch"s)) {
				size_t len = 0;
				std::string str;
				zmqpp::message_t reply{};

				if (offset < size) {
					ifs.seekg(offset);
					str.resize(chunksize, '\0');
					ifs.read(&str[0], chunksize);
					len = std::min<size_t>(str.length(), size - offset);
				}

				reply.add<std::string>(identity);
				reply.add<size_t>(len);
				reply.add<std::string>(str);
				router.send(reply);
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