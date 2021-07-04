#include <vld.h>
#include "mdcliapi2.hpp"
#include <charconv>

int main(int argc, char* argv[])
{
	if (argc < 2) {
		fmt::print("Usage mdclient2 [id]\n");
		return 0;
	}
	int id{};
	std::string_view sid(argv[1]);
	auto [p, ec] = std::from_chars(std::data(sid), std::data(sid) + std::size(sid), id);
	mdcli session("tcp://localhost:5555", id, 1);

	int i = 0;
	for (i = 0; i < 100; ++i) {
		auto req = std::make_unique<zmsg>(fmt::format("hello world {}", i).c_str());
		session.send("echo", std::move(req));
	}

	for (i = 0; i < 100; ++i) {
		auto reply = session.recv();
		if (reply) {
			//fmt::print("reply => OK\n");
		}
		else break;
	}
	fmt::print("{} replies received\n", i);
	return 0;
}