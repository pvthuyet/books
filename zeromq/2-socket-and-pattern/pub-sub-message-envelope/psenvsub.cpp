#include <Windows.h>
#include <zhelpers.hpp>
#include <format>
#include <fmt/format.h>

int main()
{
	using namespace std::string_view_literals;
	//  Prepare our context and subscriber
	zmq::context_t ctx(1);
	zmq::socket_t subs(ctx, ZMQ_SUB);
	subs.connect("tcp://localhost:5563");
	subs.set(zmq::sockopt::subscribe, "B"sv);
	while (1) {
		//  Read envelope with address
		std::string address = s_recv(subs);
		//  Read message contents
		std::string contents = s_recv(subs);
		
		//std::cout << std::format("[{}] {}\n"sv, address, contents);
		fmt::print("[{}] {}\n", address, contents);
	}
	return 0;
}