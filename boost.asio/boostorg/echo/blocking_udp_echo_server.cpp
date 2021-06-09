#include <cstdlib>
#include <iostream>
#include <boost/asio.hpp>

using boost::asio::ip::udp;
enum { max_length = 1024 };

void server(boost::asio::io_context& io_context, unsigned short port)
{
	udp::socket sock(io_context, udp::endpoint(udp::v4(), port));
	for (;;) {
		char data[max_length] = { 0 };
		udp::endpoint sender_ep;
		auto len = sock.receive_from(
			boost::asio::buffer(data, max_length),
			sender_ep);
		std::cout << "Received from " << sender_ep.address() << ": " << data << std::endl;
		sock.send_to(boost::asio::buffer(data, len), sender_ep);
	}
}

int main()
{
	try {
		boost::asio::io_context ioc;
		server(ioc, 3333);
	}
	catch (std::exception const& ex) {
		std::cerr << ex.what() << std::endl;
	}
	return EXIT_SUCCESS;
}