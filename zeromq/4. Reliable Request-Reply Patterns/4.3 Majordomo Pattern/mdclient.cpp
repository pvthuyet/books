//
//  Majordomo Protocol client example
//  Uses the mdcli API to hide all MDP aspects
//
//  Lets us 'build mdclient' and 'build all'
//
//     Andreas Hoelzlwimmer <andreas.hoelzlwimmer@fh-hagenberg.at>
//

#include "mdcliapi.hpp"
#include <charconv>
int main(int argc, char* argv[])
{
	if (argc < 2) {
		fmt::print("Usage: mdclient [id]");
		return 0;
	}

	int id{};
	std::string_view sid(argv[1]);
	auto [p, ec] = std::from_chars(std::data(sid), std::data(sid) + std::size(sid), id);
	mdcli session("tcp://localhost:5555", id);

	int cnt = 0;
	for (cnt = 0; cnt < 1000; ++cnt) {
		auto reply = session.send("echo", std::make_unique<zmsg>("hello world"));
		if (!reply) {
			fmt::print("[err] interrupt\n");
			break;
		}
		fmt::print("reply: {}\n", reply->body());
		s_sleep(1000);
	}
	fmt::print("{} requests/replies processed\n", cnt);
	return 0;
}