#include <thread>
#include <unordered_map>
#include "kvmsg.hpp"
#include <random>
#include <format>
#include <boost/algorithm/string.hpp>
#include <fmt/core.h>
#include <zmqpp/zmqpp.hpp>
#include "bstar.hpp"

class clonesrv6
{
private:
	zmqpp::context_t ctx_;
	std::unordered_map<std::string, kvmsg> kvmap_;
	bstar	bstar_;
public:
	clonesrv6() :
		bstar_(true, "a", "b")
	{
	}
};

int main()
{
	clonesrv6 srv{};
}