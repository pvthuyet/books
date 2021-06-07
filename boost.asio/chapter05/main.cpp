#include <iostream>
#include "http_client.hpp"
#include "http_server.hpp"

int main()
{
	HTTPClient::start_client();
	//async_server::start_server();
	return EXIT_SUCCESS;
}