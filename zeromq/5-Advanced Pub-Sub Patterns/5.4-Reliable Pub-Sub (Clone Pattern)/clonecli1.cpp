#include <thread>
#include <unordered_map>
#include "kvsimple.hpp"
#include <random>
#include <format>
#include <fmt/core.h>
#include <limits>

template<typename... Ts>
void func(Ts... args)
{
	constexpr int size = sizeof...(args) + 2;
	int res[size] = { 1, args..., 2 };
	int dummy[sizeof...(Ts)] = {(std::cout << args, 0)...};
}

template<class T, class... Args>
void demo(T&& t, Args&&... args)
{
	std::cout << t;
	auto lb = [](auto && arg){ std::cout << ' ' << arg; };
	(..., lb(std::forward<Args>(args)));
}

template<class... Args>
void print(Args&&... args)
{
	(std::cout << ... << args) << std::endl;
}

int main()
{
	print(1u, 2.0, 'a');

	return 0;

	zmqpp::context_t ctx;
	zmqpp::socket_t sock(ctx, zmqpp::socket_type::sub);
	sock.connect("tcp://localhost:5556");
	sock.set(zmqpp::socket_option::subscribe, "");

	std::unordered_map<std::string, kvsimple> kvmap{};
	int seq{};

	while (true) {
		auto kvmsg = kvsimple::recv(sock);
		kvmap.insert(std::make_pair(kvmsg.key_, kvmsg));
		fmt::print("received: {} {} {}\n", kvmsg.key_, kvmsg.sequence_, kvmsg.body_);
		++seq;
	}
}