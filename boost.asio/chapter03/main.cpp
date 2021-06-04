#include <iostream>
#include "SyncTCPClient.hpp"
#include "SyncTCPServer.hpp"
#include "SyncUDPClient.hpp"
#include "AsyncTcpClient.hpp"
int main()
{
	//SyncTCPClient::start();
	//SyncTCPServer{}.start();
	//SyncUDFClient::start();
	AsyncTcpClient::start();
	return EXIT_SUCCESS;
}