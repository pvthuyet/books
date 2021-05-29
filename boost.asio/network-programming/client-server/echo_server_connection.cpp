#include "echo_server_connection.hpp"
#include "logger.hpp"

using namespace std::literals;

SG_BEGIN
EchoServerConnection::EchoServerConnection(boost::shared_ptr<hive> hiv) :
	Connection(hiv)
{}

void EchoServerConnection::OnAccept(const std::string& host, boost::uint16_t port)
{
	LOGENTER;
	LOGINFO(BOOST_CURRENT_LOCATION, host, ":", port);
	Recv();
	LOGEND;
}

void EchoServerConnection::OnConnect(const std::string& host, boost::uint16_t port)
{
	LOGENTER;
	LOGINFO(BOOST_CURRENT_LOCATION, host, ":", port);
	Recv();
	LOGEND;
}

void EchoServerConnection::OnSend(const std::vector<boost::uint8_t>& buffer)
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

void EchoServerConnection::OnRecv(std::vector<boost::uint8_t>& buffer)
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

	// Echo the data back
	Send(buffer);
	LOGEND;
}

void EchoServerConnection::OnTimer(const boost::posix_time::time_duration& delta)
{
	return;
	LOGENTER;
	LOGINFO(BOOST_CURRENT_LOCATION, delta);
	LOGEND;
}

void EchoServerConnection::OnError(const boost::system::error_code& error)
{
	LOGENTER;
	LOGINFO(BOOST_CURRENT_LOCATION, error);
	LOGEND;
}

SG_END