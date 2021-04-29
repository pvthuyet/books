#include <boost/asio.hpp>
#include <iostream>

using namespace boost;
int main()
{
	// step 1
	std::string host = "google.com";
	std::string port_num = "8888";

	asio::io_service ios;
	// creating a resolver's query
	asio::ip::tcp::resolver::query resolver_query(host,
		port_num,
		asio::ip::tcp::resolver::query::numeric_service
		);

	// creating a resolver
	asio::ip::tcp::resolver resolver(ios);

	try
	{
		// step 2
		asio::ip::tcp::resolver::iterator it = resolver.resolve(resolver_query);

		// step 3
		asio::ip::tcp::socket sock(ios);

		// step 4
		asio::connect(sock, it);
	}
	catch (system::system_error const& e) {
		std::cout << "Error occured! Error code = " << e.code()
			<< ". Message: " << e.what();
		return e.code().value();
	}

	return EXIT_SUCCESS;
}