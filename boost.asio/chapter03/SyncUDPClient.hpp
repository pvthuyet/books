#pragma once
#include <boost/asio.hpp>
class SyncUDFClient
{
private:
	boost::asio::io_service m_ios;
	boost::asio::ip::udp::socket m_sock;

public:
	SyncUDFClient();
	static void start();
	std::string enulateLongComputationOp(unsigned int duration_sec, std::string_view rawip, unsigned short port);

private:
	void sendRequest(const boost::asio::ip::udp::endpoint& ep, std::string_view request);
	std::string reveiveResponse(boost::asio::ip::udp::endpoint& ep);
};