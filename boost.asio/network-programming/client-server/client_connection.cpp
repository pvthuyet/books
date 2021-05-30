#include "client_connection.hpp"
#include "logger.hpp"

using namespace std::literals;
SG_BEGIN

ClientConnection::ClientConnection(boost::shared_ptr<hive> hiv) :
	Connection(hiv)
{
}

void ClientConnection::OnAccept(const std::string& host, boost::uint16_t port)
{
	LOGENTER;
	LOGINFO(BOOST_CURRENT_LOCATION, host, ":", port);
	Recv();
	LOGEND;
}

void ClientConnection::OnConnect(const std::string& host, boost::uint16_t port)
{
	LOGENTER;
	LOGINFO(BOOST_CURRENT_LOCATION, host, ":", port);
	Recv();
	std::string str = "GET / HTTP/1.0\r\n\r\n";
	std::vector<uint8_t> request;
	std::copy(str.begin(), str.end(), std::back_inserter(request));
	Send(request);
	LOGEND;
}

void ClientConnection::OnSend(const std::vector<boost::uint8_t>& buffer)
{
	LOGENTER;
	LOGINFO(BOOST_CURRENT_LOCATION, buffer.size(), "bytes");
	std::stringstream ss;
	for (int i = 0; i < buffer.size(); ++i) {
		ss << std::hex
			<< std::setfill('0')
			<< std::setw(2)
			<< (int)buffer[i];
		if ((i + 1) % 16 == 0) {
			LOGINFO(BOOST_CURRENT_LOCATION, ss.str());
			ss.clear();
		}
	}
	LOGINFO(BOOST_CURRENT_LOCATION, ss.str());
	LOGEND;
}

void ClientConnection::OnRecv(std::vector<boost::uint8_t>& buffer)
{
	LOGENTER;
	LOGINFO(BOOST_CURRENT_LOCATION, buffer.size(), "bytes");
	std::stringstream ss;
	for (int i = 0; i < buffer.size(); ++i) {
		ss << std::hex
			<< std::setfill('0')
			<< std::setw(2)
			<< (int)buffer[i];
		if ((i + 1) % 16 == 0) {
			LOGINFO(BOOST_CURRENT_LOCATION, ss.str());
			ss.clear();
		}
	}
	LOGINFO(BOOST_CURRENT_LOCATION, ss.str());
	// Start the next receive
	Recv();

	LOGEND;
}

void ClientConnection::OnTimer(const boost::posix_time::time_duration& delta)
{
	LOGENTER;
	LOGEND;
}

void ClientConnection::OnError(const boost::system::error_code& error)
{
	LOGENTER;
	LOGEND;
}
SG_END