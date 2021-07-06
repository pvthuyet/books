#include <vld.h>
#include "mdcliapi2.hpp"
#include <charconv>

int main(int argc, char* argv[])
{
	if (argc < 2) {
		fmt::print("Usage ticlient [id]\n");
		return 0;
	}
	int id{};
	std::string_view sid(argv[1]);
	auto [p, ec] = std::from_chars(std::data(sid), std::data(sid) + std::size(sid), id);
	mdcli session("tcp://localhost:5555", id, 1);

	// 1. send 'echo' request to Titanic
	auto req = std::make_unique<zmsg>("echo");
	req->append("Hello world");
	session.send("titanic.request", std::move(req));

	// 2. reply
	auto reply = session.recv();
	if (reply) {
		fmt::print("[info] titanic reply 'echo' service:\n");
		reply->dump();
	}
	else {
		fmt::print("[err] No response from broker\n");
	}

	return 0;
}