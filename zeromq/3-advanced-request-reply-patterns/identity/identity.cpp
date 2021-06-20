#include <Windows.h>
#include <zhelpers.hpp>
#include <thread>
#include <fmt/format.h>
#include <syncstream>

int main()
{
	using namespace std::string_literals;
	using namespace std::string_view_literals;
	constexpr const char* addr = "inproc://example";

	zmq::context_t ctx(1);

	zmq::socket_t sink(ctx, ZMQ_ROUTER);
	sink.bind(addr);
	
	//  First allow 0MQ to set the identity
	zmq::socket_t anonymous(ctx, ZMQ_REQ);
	anonymous.connect(addr);

	s_send(anonymous, "ROUTER uses a generated 5 byte identity"s);
	s_dump(sink);

	//  Then set the identity ourselves
	zmq::socket_t identified(ctx, ZMQ_REQ);
	identified.set(zmq::sockopt::routing_id, "PEER2"sv);
	identified.connect(addr);

	s_send(identified, "ROUTER socket uses REQ's socket idientity"s);
	s_dump(sink);

	return 0;
}