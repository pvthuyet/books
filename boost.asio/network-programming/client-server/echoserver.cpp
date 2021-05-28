#include "echoserver.hpp"
#include "hive.hpp"
#include "echo_server_acceptor.hpp"
#include "echo_server_connection.hpp"
#include <boost/make_shared.hpp>
#include <conio.h>

SG_BEGIN

void echo_server::start()
{
	auto hiv = boost::make_shared<hive>();
	auto acpt = boost::make_shared<EchoSeverAcceptor>(hiv);
	acpt->Listen("localhost", 4444);
	auto conn = boost::make_shared<EchoServerConnection>(hiv);
	acpt->Accept(conn);

	while (!_kbhit()) {
		hiv->poll();
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	}
	hiv->stop();
}

SG_END