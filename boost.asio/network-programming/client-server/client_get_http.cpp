#include "client_get_http.hpp"
#include "hive.hpp"
#include "client_connection.hpp"
#include <boost/make_shared.hpp>
#include <conio.h>

SG_BEGIN
void client_get_http::start()
{
	auto hiv = boost::make_shared<hive>();
	auto conn = boost::make_shared<ClientConnection>(hiv);
	conn->Connect("www.packtpub.com", 80);
	while (!_kbhit())
	{
		hiv->poll();
		Sleep(1);
	}

	hiv->stop();
}

SG_END