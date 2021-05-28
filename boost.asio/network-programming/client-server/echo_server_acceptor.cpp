#include "echo_server_acceptor.hpp"
#include "logger.hpp"

SG_BEGIN
EchoSeverAcceptor::EchoSeverAcceptor(boost::shared_ptr<hive> hiv) :
	Acceptor(hiv)
{}

bool EchoSeverAcceptor::OnAccept(boost::shared_ptr<Connection> connection, const std::string& host, uint16_t port)
{
	LOGENTER;
	logger::get_inst().info(host, ":", port);
	LOGEND;
	return true;
}

void EchoSeverAcceptor::OnTimer(const boost::posix_time::time_duration& delta)
{
	LOGENTER;
	logger::get_inst().info(delta);
	LOGEND;
}

void EchoSeverAcceptor::OnError(const boost::system::error_code& error)
{
	LOGENTER;
	logger::get_inst().info(error);
	LOGEND;
}

SG_END