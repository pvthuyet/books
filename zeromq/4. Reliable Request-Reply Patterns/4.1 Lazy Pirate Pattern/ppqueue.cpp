//
//  Paranoid Pirate queue
//
//     Andreas Hoelzlwimmer <andreas.hoelzlwimmer@fh-hagenberg.at>
//
#include <Windows.h>
#include <zmsg.hpp>
#include <vector>
#include <string>
#include <algorithm>
#include <fmt/core.h>
#include <fmt/color.h>
#include <gsl/gsl_assert>

constexpr const int HEARTBEAT_LIVENESS = 3;
constexpr const int HEARTBEAT_INTERVAL = 1000; // msecs

struct  worker_t
{
	std::string identity;	// address of worker
	//int64_t expiry;			// Expires at this time
	std::chrono::steady_clock::time_point expiry;
};

//  Insert worker at end of queue, reset expiry
//  Worker must not already be in queue

static void s_worker_append(std::vector<worker_t>& queue, std::string& ident)
{
	auto found = std::ranges::find_if(queue, [&ident](auto const& item) {
		return item.identity == ident;
		});

	if (found == std::cend(queue)) {
		worker_t wk;
		wk.identity = ident;
		wk.expiry = std::chrono::steady_clock::now();
		queue.push_back(std::move(wk));
	}
}

// Remove worker from queue, if present
static void s_worker_delete(std::vector<worker_t>& queue, std::string_view ident)
{
	std::erase_if(queue, [&ident](auto const& item) {
		return item.identity == ident;
		});
}

// Reset worker expiry, worker must be present
static void s_worker_refresh(std::vector<worker_t>& queue, std::string_view ident)
{
	auto found = std::ranges::find_if(queue, [&ident](auto const& item) {
		return item.identity == ident;
		});
	if (found != std::cend(queue)) {
		found->expiry = std::chrono::steady_clock::now();
	}
	else {
		fmt::print("[err] worker {} not ready\n", ident);
	}
}

// pop next available worker off queue, return identity
static std::string s_worker_dequeue(std::vector<worker_t>& queue)
{
	Ensures(queue.size());
	auto ident = queue[0].identity;
	queue.erase(queue.begin());
	return ident;
}

// Look for & kill expired workers
static void s_queue_purge(std::vector<worker_t>& queue)
{
	std::erase_if(queue, [](auto const& item) {
		auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - item.expiry);
		auto iserase = ms.count() > HEARTBEAT_LIVENESS * HEARTBEAT_INTERVAL;
		if (iserase) {
			fmt::print("[info] the {} is expired\n", item.identity);
		}
		return iserase;
		});
}

int main()
{
	using namespace std::string_literals;
	using namespace std::string_view_literals;
	zmq::context_t ctx(1);
	zmq::socket_t frontend(ctx, zmq::socket_type::router);
	zmq::socket_t backend(ctx, zmq::socket_type::router);
	frontend.bind("tcp://*:5555");
	backend.bind("tcp://*:5556");

	// Queue of available workers
	std::vector<worker_t> queue;

	// send out hearbeats at regular intervals
	auto heartbeat_at = std::chrono::steady_clock::now();
	while (1) {
		std::vector<zmq::pollitem_t> items = {
			{backend, 0, ZMQ_POLLIN, 0},
			{frontend, 0, ZMQ_POLLIN, 0},
		};

		// Poll frontend only if we have available workers
		if (queue.size()) {
			zmq::poll(items, HEARTBEAT_INTERVAL);
		}
		else {
			zmq::poll(&items[0], 1, HEARTBEAT_INTERVAL);
		}

		// Handle worker activity on backend
		if (items[0].revents & ZMQ_POLLIN) {
			zmsg msg(backend);
			std::string ident(msg.unwrap());

			// Return reply to client if it's not a control message
			if (msg.parts() == 1) {
				std::string_view addr(msg.address());
				if (addr == "READY"sv) {
					s_worker_delete(queue, ident);
					s_worker_append(queue, ident);
				}
				else {
					if (addr == "HEARTBEAT"sv) {
						s_worker_refresh(queue, ident);
					}
					else {
						fmt::print("[error] invalid message from {}\n", ident);
						msg.dump();
					}
				}
			}
			else {
				msg.send(frontend);
				s_worker_append(queue, ident);
			}
		}
		if (items[1].revents & ZMQ_POLLIN) {
			// Now get next client request, route to next worker
			zmsg msg(frontend);
			auto ident = s_worker_dequeue(queue);
			msg.wrap(ident.c_str(), nullptr);
			msg.send(backend);
		}

		// Send heartbeats to idle workers if it's time
		auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - heartbeat_at);
		if (diff.count() > HEARTBEAT_INTERVAL) {
			for (auto const& item : queue) {
				zmsg msg("HEARTBEAT");
				msg.wrap(item.identity.c_str(), nullptr);
				msg.send(backend);
			}
			heartbeat_at = std::chrono::steady_clock::now();
		}
		s_queue_purge(queue);
	}

	// We never exit the main loop
	//But pretend to do the right shutdown anyhow
	queue.clear();
	return 0;
}