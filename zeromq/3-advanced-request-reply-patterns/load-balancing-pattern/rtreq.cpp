#include <Windows.h>
#include <zhelpers.hpp>
#include <thread>
#include <syncstream>
#include <sstream>
#include <fmt/format.h>

int main()
{
	using namespace std::string_literals;
	zmq::context_t ctx(1);
	zmq::socket_t broker(ctx, ZMQ_ROUTER);
#if (defined(WIN32))
	broker.bind("tcp://*:5671");
#else
	broker.bind("ipc://routing.ipc");
#endif
	constexpr int NUM = 10;
	std::vector<std::jthread> pool;
	for (int i = 0; i < NUM; ++i) {
		pool.push_back(std::jthread([&ctx, i](std::stop_token stk) {
			zmq::socket_t worker(ctx, ZMQ_REQ);
			
			//  We use a string identity for ease here
#if (defined (WIN32))
			s_set_id(worker, i);
			
			worker.connect("tcp://localhost:5671"); // "ipc" doesn't yet work on windows.
#else
			s_set_id(worker);
			worker.connect("ipc://routing.ipc");
#endif
			std::ostringstream oss;
			oss << std::this_thread::get_id();

			int total = 0;
			while (1) {
				//  Tell the broker we're ready for work
				auto msg = fmt::format("Hi Boss {}", total);
				s_send(worker, msg);

				//  Get workload from broker, until finished
				auto workload = s_recv(worker);
				if ("Fired!"s == workload) {
					std::cout << fmt::format("[{}] Processed: {} {} tasks\n", oss.str(), worker.get(zmq::sockopt::routing_id), total);
					break;
				}
				else {
					std::cout << fmt::format("[{}] {} {}\n", oss.str(), worker.get(zmq::sockopt::routing_id), workload);
				}
				++total;
				//  Do some random work
				std::this_thread::sleep_for(std::chrono::milliseconds(100));
			}
			}));
	}

	//  Run for five seconds and then tell workers to end
	std::ostringstream oss;
	oss << std::this_thread::get_id();

	auto start = std::chrono::steady_clock::now();
	int num_fired = 0;
	while (1) {
		//  Next message gives us least recently used worker
		auto ident = s_recv(broker);
		auto del = s_recv(broker); // Envelope delimiter
		auto msg = s_recv(broker); // response from worker
		std::cout << fmt::format("[{}] {} {} {}\n", oss.str(), ident, del, msg);

		s_sendmore(broker, ident);
		s_sendmore(broker, "");
		//  Encourage workers until it's time to fire them
		std::chrono::duration<double> elapsed_seconds = std::chrono::steady_clock::now() - start;
		if (elapsed_seconds.count() < 5) {
			s_send(broker, "Work harder"s);
		}
		else {
			s_send(broker, "Fired!"s);
			if (++num_fired == NUM) break;
		}
	}
	std::cin.get();
	return EXIT_SUCCESS;
}