#include "flcapi.hpp"
#include <iostream>

int main()
{
	using namespace std::literals;
	std::string name = "hello 1";
	std::string ep = "tcp://127.0.0.1:5555";

	flcaip::free_lance_client client;
	client.connect(ep);

	zmqpp::message_t req;
	req << name;
	auto reply = client.request(req);
	if (reply) {
		std::cout << "Client recevied: " << reply->get<std::string>(0) << std::endl;
	}

	std::cin.get();
	return 0;
}