#include "SyncTCPClient.hpp"
#include <iostream>

SyncTCPClient::SyncTCPClient(std::string_view rawip, unsigned short port) :
	m_ep(boost::asio::ip::address::from_string(rawip.data()), port),
	m_sock(m_ios)
{
	m_sock.open(m_ep.protocol());
}

void SyncTCPClient::start()
{
	std::string_view rawip = "127.0.0.1";
	const unsigned short port = 3333;
	try {
		SyncTCPClient client(rawip, port);
		client.connect();
		std::cout << "sending request to server...\n";
		auto response = client.emulateLongComputationOp(10);
		std::cout << "received: " << response << std::endl;
		client.close();
	}
	catch (const std::exception& ex) {
		std::cout << ex.what() << std::endl;
	}
}

void SyncTCPClient::connect()
{
	m_sock.connect(m_ep);
}

void SyncTCPClient::close()
{
	m_sock.shutdown(boost::asio::ip::tcp::socket::shutdown_both);
	m_sock.close();
}

std::string SyncTCPClient::emulateLongComputationOp(unsigned int duration_sec)
{
	std::string request = "EMULATE_LONG_COMP_OP " + std::to_string(duration_sec) + "\n";
	sendRequest(request);
	return receiveResponse();
}

void SyncTCPClient::sendRequest(std::string_view request)
{
	auto rs = boost::asio::write(m_sock, boost::asio::buffer(request));
	int xxx = 0;
}

std::string SyncTCPClient::receiveResponse()
{
	boost::asio::streambuf buf;
	boost::asio::read_until(m_sock, buf, '\n');
	std::istream input(&buf);
	std::string response;
	std::getline(input, response);
	return response;
}