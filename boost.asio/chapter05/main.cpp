#include <iostream>
#include "http_client.hpp"
#include "http_server.hpp"
#include "sync_ssl_client.hpp"
#include "sync_ssl_server.hpp"

int main()
{
	HTTPClient::start_client();
	//async_server::start_server();
	//SyncSSLClient::statrt_client();
	//ssl_server::start_server();
	return EXIT_SUCCESS;
}