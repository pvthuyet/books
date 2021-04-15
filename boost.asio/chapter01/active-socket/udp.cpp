#include <boost/asio.hpp>
#include <iostream>

int main()
{
	namespace asio = boost::asio;
	// step 1
	asio::io_service ios;
	// step 2
	asio::ip::udp protocal = asio::ip::udp::v6();
	// step 3
	asio::ip::udp::socket sock(ios);
	boost::system::error_code ec;
	// step 4
	sock.open(protocal, ec);
	if (ec.value() != 0) {
		std::cout << "Failed to open the socket! Error code = "
			<< ec.value() << ". Message: " << ec.message() << std::endl;
		return ec.value();
	}

	return EXIT_SUCCESS;
}