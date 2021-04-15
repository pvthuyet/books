#include <boost/asio.hpp>
#include <iostream>

int main()
{
	namespace asio = boost::asio;
	// step 1
	asio::io_service ios;
	// step 2
	asio::ip::tcp protocal = asio::ip::tcp::v6();
	// step 3
	asio::ip::tcp::acceptor acceptor(ios);
	boost::system::error_code ec;
	acceptor.open(protocal, ec);
	if (ec.value() != 0) {
		std::cout << "Failed to open the acceptor socket! Error code = "
			<< ec.value() << ". Message: " << ec.message() << std::endl;
		return ec.value();
	}
	return EXIT_SUCCESS;
}