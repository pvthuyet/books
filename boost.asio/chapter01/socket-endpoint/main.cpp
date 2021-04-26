#include <boost/asio.hpp>
#include <iostream>

int main()
{
	using namespace boost;
	// Step 1
	unsigned short port_num = 3333;
	// step 2
	asio::ip::tcp::endpoint ep(asio::ip::address_v4::any(), port_num);
	asio::io_service ios;

	// step 3
	asio::ip::tcp::acceptor acceptor(ios, ep.protocol());

	// step 4
	boost::system::error_code ec;
	acceptor.bind(ep, ec);
	
	if (ec.value() != 0) {
		std::cout << "Failed to binding the acceptor socket! Error code = "
			<< ec.value() << ". Message: " << ec.message() << std::endl;
		return ec.value();
	}

	return EXIT_SUCCESS;
}