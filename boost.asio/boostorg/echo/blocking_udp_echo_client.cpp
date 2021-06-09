#include <cstdlib>
#include <cstring>
#include <iostream>
#include <boost/asio.hpp>

using boost::asio::ip::udp;
enum { max_length = 1024 };

int main()
{
	try {
		boost::asio::io_context io_context;
		udp::socket s(io_context, udp::endpoint(udp::v4(), 0));
		udp::resolver resolver(io_context);
		udp::resolver::results_type endpoints =
			resolver.resolve(udp::v4(), "localhost", "3333");

		while (1) {
			std::cout << "Enter message: ";
			char request[max_length];
			std::cin.getline(request, max_length);
			size_t request_length = std::strlen(request);
			s.send_to(boost::asio::buffer(request, request_length), *endpoints.begin());

			char reply[max_length];
			udp::endpoint sender_endpoint;
			size_t reply_length = s.receive_from(
				boost::asio::buffer(reply, max_length), sender_endpoint);

			std::cout << "Reply from " << sender_endpoint.address() << ": ";
			std::cout.write(reply, reply_length);
			std::cout << "\n";
			//std::cin.get();
		}
	}
	catch (std::exception const& ex) {
		std::cerr << ex.what() << std::endl;
	}
}