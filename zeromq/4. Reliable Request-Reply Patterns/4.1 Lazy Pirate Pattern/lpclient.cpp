//
//  Lazy Pirate client
//  Use zmq_poll to do a safe request-reply
//  To run, start piserver and then randomly kill/restart it
//

#include <iostream>
#include <fmt/core.h>
#include <fmt/color.h>
#include <Windows.h>
#include <zhelpers.hpp>
#include <sstream>
#include <memory>
#include <charconv>
#include <vld.h>

constexpr const int REQUEST_TIMEOUT = 2500;
constexpr const int REQUEST_RETRIES = 3;

//  Helper function that returns a new configured socket
//  connected to the Hello World server
//
static std::unique_ptr<zmq::socket_t> s_client_socket(zmq::context_t& ctx)
{
	fmt::print("[inf] connecting to server ...\n");
	auto client = std::make_unique<zmq::socket_t>(ctx, zmq::socket_type::req);
	client->connect("tcp://localhost:5555");

	// Configure socket to not wait at close time
	client->set(zmq::sockopt::linger, 0);
	return client;
}

int main()
{
	zmq::context_t ctx(1);
	auto client = s_client_socket(ctx);

	int sequence = 0;
	int retries_left = REQUEST_RETRIES;
	while (retries_left) {
		std::stringstream request;
		request << ++sequence;
		s_send(*client, request.str());
		s_sleep(1000);

		bool expect_rely = true;
		while (expect_rely) {
			// Poll socket for reply with timeout
			std::vector<zmq::pollitem_t> items = {
				{*client, 0, ZMQ_POLLIN, 0}
			};

			zmq::poll(items, REQUEST_TIMEOUT);

			// if we got reply, process it
			if (items[0].revents & ZMQ_POLLIN) {
				// we got reply from the server, must match sequence
				std::string reply = s_recv(*client);
				int result{};
				auto [p, ec] = std::from_chars(std::data(reply), std::data(reply) + std::size(reply), result);
				if (result == sequence) {
					fmt::print("[inf] server replied OK ({})\n", reply);
					retries_left = REQUEST_RETRIES;
					expect_rely = false;
				}
				else {
					fmt::print(fg(fmt::color::red), "[err] malformed reply from server: {}\n", reply);
				}
			}
			else {
				if (--retries_left == 0) {
					fmt::print(fg(fmt::color::red), "[err] server seems to be offline, abandoning\n");
					expect_rely = false;
					break;
				}
				else {
					fmt::print(fg(fmt::color::yellow), "[warn] no response from server, retrying...\n");
					client = s_client_socket(ctx);
					// Send request again, on new socket
					s_send(*client, request.str());
				}
			}
		}
	}

	return EXIT_SUCCESS;
}