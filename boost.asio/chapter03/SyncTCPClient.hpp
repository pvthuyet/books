#pragma once
#include <boost/asio.hpp>

class SyncTCPClient
{
private:
	boost::asio::io_service m_ios;
	boost::asio::ip::tcp::endpoint m_ep;
	boost::asio::ip::tcp::socket m_sock;

public:
	SyncTCPClient(std::string_view rawip, unsigned short port);
	static void start();
	void connect();
	void close();
	std::string emulateLongComputationOp(unsigned int duration_sec);

private:
	void sendRequest(std::string_view request);
	std::string receiveResponse();
};