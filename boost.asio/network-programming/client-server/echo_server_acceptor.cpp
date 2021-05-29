#include "echo_server_acceptor.hpp"
#include "logger.hpp"
#include <string>

using namespace std::literals;
SG_BEGIN
EchoSeverAcceptor::EchoSeverAcceptor(boost::shared_ptr<hive> hiv) :
	Acceptor(hiv)
{}

bool EchoSeverAcceptor::OnAccept(boost::shared_ptr<Connection> connection, const std::string& host, uint16_t port)
{
	LOGENTER;
	LOGINFO(BOOST_CURRENT_LOCATION, host, ":", port);
	LOGEND;
	return true;
}

void EchoSeverAcceptor::OnTimer(const boost::posix_time::time_duration& delta)
{
	return;
	LOGENTER;
	LOGINFO(BOOST_CURRENT_LOCATION, delta);
	LOGEND;
}

void EchoSeverAcceptor::OnError(const boost::system::error_code& error)
{
	LOGENTER;
	LOGINFO(BOOST_CURRENT_LOCATION, error);
	LOGEND;
}

SG_END