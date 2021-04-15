#include <boost/asio.hpp>
#include <iostream>

using namespace boost;
int main()
{
	// step 1
	asio::io_service ios;
	// step 2
	asio::ip::tcp protocal = asio::ip::tcp::v4();
	// step 3
	asio::ip::tcp::socket sock(ios);
	boost::system::error_code ec;
	// step 4
	sock.open(protocal, ec);
	if (ec.value() != 0) {
		// failed to open the socket
		std::cout << "Failed to open the socket! Error code = "
			<< ec.value() << ". Message: " << ec.message() << std::endl;
		return ec.value();
	}

	return EXIT_SUCCESS;
}