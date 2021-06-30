#include <Windows.h>
#include <zhelpers.hpp>
#include <fmt/format.h>
#include <fmt/printf.h>
#include <charconv>
#include <thread>

#define NBR_CLIENTS 10
#define NBR_WORKERS 3
#define WORKER_READY "\001"

static std::string self;
class client_task
{
private:
	zmq::context_t& ctx_;
	zmq::socket_t sock_;
	std::string id_;
	std::string port_;

public:
	client_task(zmq::context_t& ctx, std::string id, std::string port) :
		ctx_(ctx),
		sock_(ctx_, ZMQ_REQ),
		id_{id},
		port_{port}
	{
		sock_.set(zmq::sockopt::routing_id, id_);
	}

	void start()
	{
		try {
			fmt::print("Client {} starts connecting host {}\n", id_, port_);
			sock_.connect(fmt::format("tcp://localhost:{}", port_));
			int idx = 0;
			while (true) {
				// send message
				auto req = fmt::format("Hello {}", ++idx);
				s_send(sock_, req);
				auto reply = s_recv(sock_);
				fmt::print("req: {} => {}\n", req, reply);
				s_sleep(1000);
			}
		}
		catch (std::exception const& ex) {
			fmt::fprintf(std::cerr, "[ERROR] {}", ex.what());
		}
	}
};

int main(int argc, char* argv[])
{
	if (argc < 3) {
		fmt::print("Using peering2_client [client_id] [port]\n");
		return -1;
	}

	std::string svnum(argv[1]);
	//int sz{};
	//auto [p, ec] = std::from_chars(std::data(svnum), std::data(svnum) + std::size(svnum), sz);

	zmq::context_t ctx(1);
	std::vector<std::unique_ptr<std::jthread>> threads;
	//threads.reserve(sz);
	std::string port = argv[2];	

	for (int i = 0; i < 1; ++i) {
		threads.push_back(std::make_unique<std::jthread>([&ctx, id = svnum, port]() {
			client_task client(ctx, id, port);
			client.start();
			}));
	}

	fmt::print("Press ENTER to exit\n");
	std::cin.get();

	return EXIT_SUCCESS;
}