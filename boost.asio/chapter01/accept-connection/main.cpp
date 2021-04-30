#include <boost/asio.hpp>
#include <iostream>

using namespace boost;
int main()
{
	const int BACKLOG_SIZE = 30;
	// step 1
	unsigned short port_num = 3333;
	// step 2
	asio::ip::tcp::endpoint ep(asio::ip::address_v4::any(), port_num);

	asio::io_service ios;
	try {
		// step 3
		asio::ip::tcp::acceptor acceptor(ios, ep.protocol());
		// step 4
		acceptor.bind(ep);
		// step 5
		acceptor.listen(BACKLOG_SIZE);
		// step 6
		asio::ip::tcp::socket sock(ios);
		// step 7
		acceptor.accept(sock);
		// send  receive data here....
	}
	catch (system::system_error const &e) {
		std::cout << "Error occured! Error code = " << e.code()
			<< ". Message: " << e.what();
		return e.code().value();
	}
	return EXIT_SUCCESS;
}