#include "mdwrkapi.hpp"
#include <charconv>
int main(int argc, char* argv[])
{
	if (argc < 2) {
		fmt::print("Usage: mdworker [id]\n");
		return 0;
	}

	int id{};
	std::string_view sid(argv[1]);
	auto [p, ec] = std::from_chars(std::data(sid), std::data(sid) + std::size(sid), id);
	mdwrk session("tcp://localhost:5555", "echo", id, 1);
	std::unique_ptr<zmsg> reply{};
	while (1) {
		auto request = session.recv(std::move(reply));
		if (request) {
			break; // worker was interrupted
		}
		reply.swap(request);
	}
	return 0;
}