#include "echoserver.hpp"
#include "hive.hpp"
#include <boost/make_shared.hpp>

SG_BEGIN
echo_server::echo_server()
{
	auto hiv = boost::make_shared<hive>();
}
SG_END