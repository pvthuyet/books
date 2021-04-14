#include <boost/asio.hpp>
#include <iostream>

using namespace boost;

int main()
{
	// Step 1. Assume that the client application has already
	// obtained the IP-address and the protocal port number
	std::string raw_ip_address = "172.0.0.1";
	unsigned short port_number = 3333;

	boost::system::error_code ec;

	// Step2:
	asio::ip::address ip_address = asio::ip::address::from_string(raw_ip_address, ec);
	if (ec.value() != 0) {
		std::cout << "Failed to parse the IP address. Error code = "
			<< ec.value() << ". Message: "
			<< ec.message();
		return ec.value();
	}

	// step3
	asio::ip::tcp::endpoint ep(ip_address, port_number);

	// step4

	return EXIT_SUCCESS;
}