#include <boost/asio.hpp>
#include <iostream>

int main()
{
	namespace asio = boost::asio;
	// step 1
	std::string host = "samplehost.com";
	std::string port_num = "3333";
	// step 2
	asio::io_service ios;
	// step 3
	asio::ip::tcp::resolver::query resolver_query(
		host,
		port_num,
		asio::ip::tcp::resolver::query::numeric_service);
	// step 4
	asio::ip::tcp::resolver resolver(ios);

	boost::system::error_code ec;
	// step 5
	auto it = resolver.resolve(resolver_query, ec);

	if (ec.value() != 0) {
		std::cout << "Failed to open the acceptor socket! Error code = "
			<< ec.value() << ". Message: " << ec.message() << std::endl;
		return ec.value();
	}

	asio::ip::tcp::resolver::iterator it_end{};
	for (; it != it_end; ++it) {
		auto ep = it->endpoint();
		std::cout << "address: " << ep.address()
			<< "\t" << "port: " << ep.port()
			<< "\tfamily: " << ep.protocol().family()
			<< "\tprotocol: " << ep.protocol().protocol()
			<< "\ttype: " << ep.protocol().type()
			<< std::endl;
	}

	return EXIT_SUCCESS;
}