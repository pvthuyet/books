#include "SyncUDPClient.hpp"
#include <iostream>

SyncUDFClient::SyncUDFClient() :
	m_sock(m_ios)
{
	m_sock.open(boost::asio::ip::udp::v4());
}

void SyncUDFClient::start()
{
	const std::string server1 = "127.0.0.1";
	const unsigned short port = 3333;
	try {
		SyncUDFClient client;
		std::cout << "send to server 1:\n";
		auto res = client.enulateLongComputationOp(10, server1, port);
		std::cout << "response from server 1: " << res << std::endl;
	}
	catch (const std::exception& ex) {
		std::cout << ex.what() << std::endl;
	}
}

std::string SyncUDFClient::enulateLongComputationOp(unsigned int duration_sec, std::string_view rawip, unsigned short port)
{
	std::string request = "EMULATE_LONG_COMP_OP " + std::to_string(duration_sec) + "\n";
	boost::asio::ip::udp::endpoint ep(boost::asio::ip::address::from_string(rawip.data()), port);
	sendRequest(ep, request);
	return reveiveResponse(ep);
}

void SyncUDFClient::sendRequest(const boost::asio::ip::udp::endpoint& ep, std::string_view request)
{
	m_sock.send_to(boost::asio::buffer(request), ep);
}

std::string SyncUDFClient::reveiveResponse(boost::asio::ip::udp::endpoint& ep)
{
	char response[16] = { 0 };
	auto byte_received = m_sock.receive_from(boost::asio::buffer(response), ep);
	m_sock.shutdown(boost::asio::ip::udp::socket::shutdown_both);
	return std::string(response, byte_received);
}