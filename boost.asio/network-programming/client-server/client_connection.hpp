#pragma once
#include "define.hpp"
#include "connection.hpp"

SG_BEGIN
class ClientConnection : public Connection
{
public:
	ClientConnection(boost::shared_ptr<hive> hiv);

private:
	void OnAccept(const std::string& host, boost::uint16_t port) final;
	void OnConnect(const std::string& host, boost::uint16_t port) final;
	void OnSend(const std::vector<boost::uint8_t>& buffer) final;
	void OnRecv(std::vector<boost::uint8_t>& buffer) final;
	void OnTimer(const boost::posix_time::time_duration& delta) final;
	void OnError(const boost::system::error_code& error) final;
};
SG_END