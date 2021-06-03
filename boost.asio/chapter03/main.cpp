#include <iostream>
#include "SyncTCPClient.hpp"
#include "SyncTCPServer.hpp"

int main()
{
	SyncTCPClient::start();
	//SyncTCPServer{}.start();
	return EXIT_SUCCESS;
}