#include "echo_server_connection.hpp"
#include "logger.hpp"

SG_BEGIN
EchoServerConnection::EchoServerConnection(boost::shared_ptr<hive> hiv) :
	Connection(hiv)
{}

void EchoServerConnection::OnAccept(const std::string& host, boost::uint16_t port)
{
	LOGENTER;
	LOGEND;
}

void EchoServerConnection::OnConnect(const std::string& host, boost::uint16_t port)
{
	LOGENTER;
	LOGEND;
}

void EchoServerConnection::OnSend(const std::vector<boost::uint8_t>& buffer)
{
	LOGENTER;
	LOGEND;
}

void EchoServerConnection::OnRecv(std::vector<boost::uint8_t>& buffer)
{
	LOGENTER;
	LOGEND;
}

void EchoServerConnection::OnTimer(const boost::posix_time::time_duration& delta)
{
	LOGENTER;
	LOGEND;
}

void EchoServerConnection::OnError(const boost::system::error_code& error)
{
	LOGENTER;
	LOGEND;
}

SG_END