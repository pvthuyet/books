#pragma once

#include "hive.hpp"
#include "acceptor.hpp"
#include "define.hpp"

SG_BEGIN
class EchoSeverAcceptor : public Acceptor
{
private:
	bool OnAccept(boost::shared_ptr<Connection> connection, const std::string& host, uint16_t port) final;
	void OnTimer(const boost::posix_time::time_duration& delta) final;
	void OnError(const boost::system::error_code& error) final;

public:
	EchoSeverAcceptor(boost::shared_ptr<hive> hiv);
};

SG_END