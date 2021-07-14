#include "flcapi.hpp"
#include <iostream>

int main(int argc, char* argv[])
{
	using namespace std::literals;
	if (argc < 2) {
		fmt::print("Usage: flclient3 [port]");
		return 0;
	}

	flcaip::free_lance_client client;
	client.connect(fmt::format("tcp://127.0.0.1:{}", argv[1]));

	for (int i = 0; i < 10; ++i) {
		zmqpp::message_t req;
		req << fmt::format("hello {}", i);
		auto reply = client.request(req);
		if (reply) {
			std::cout << "Client recevied: " << reply->get<std::string>(0) << std::endl;
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	}
	std::cin.get();
	client.stop();
	return 0;
}