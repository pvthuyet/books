#include <boost/asio.hpp>
#include <iostream>

using namespace boost;
int main()
{
	// step 1
	std::string raw_ip_address = "127.0.0.1";
	unsigned short port_num = 3333;

	try {
		// step 2
		asio::ip::tcp::endpoint ep(asio::ip::address::from_string(raw_ip_address), port_num);
		asio::io_service ios;
		
		// step 3
		asio::ip::tcp::socket sock(ios, ep.protocol());

		// step 4
		sock.connect(ep);

	}
	catch (system::system_error const& e) {
		std::cout << "Error occured! Error code = " << e.code()
			<< ". Message: " << e.what();
		return e.code().value();
	}
	return EXIT_SUCCESS;
}