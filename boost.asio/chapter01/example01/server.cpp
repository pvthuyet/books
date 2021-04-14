#include <boost/asio.hpp>
#include <iostream>

using namespace boost;

int main()
{
	// step 1
	unsigned short port_number = 3333;
	
	// step 2
	asio::ip::address ip_address = asio::ip::address_v6::any();

	// step 3
	asio::ip::tcp::endpoint ep(ip_address, port_number);

	// step 4
	return EXIT_SUCCESS;
}