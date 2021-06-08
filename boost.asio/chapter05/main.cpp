#include <iostream>
#include "http_client.hpp"
#include "http_server.hpp"
#include "sync_ssl_client.hpp"

int main()
{
	//HTTPClient::start_client();
	//async_server::start_server();
	SyncSSLClient::statrt_client();
	return EXIT_SUCCESS;
}