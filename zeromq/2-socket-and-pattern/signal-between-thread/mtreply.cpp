#include <Windows.h>
#include <zhelpers.hpp>
#include <thread>

int test()
{
	return 0;
}

int main()
{
	return test();

	using namespace std::string_literals;
	zmq::context_t ctx(1);

	zmq::socket_t receiver(ctx, ZMQ_PAIR);
	receiver.bind("inproc://step3");

	std::thread t([&ctx]() {
		zmq::socket_t receiver(ctx, ZMQ_PAIR);
		receiver.bind("inproc://step2");

		std::thread t([&ctx]() {
			zmq::socket_t sender(ctx, ZMQ_PAIR);
			sender.connect("inproc://step2");
			std::ostringstream oss;
			oss << std::this_thread::get_id() << " hi ";
			for (int i = 0; i < 10; ++i) {
				s_send(sender, oss.str() + std::to_string(i));
				s_sleep(1000);
			}
			s_send(sender, "stop"s);
			});

		// wait for signal
		// signal downstream to step 3
		zmq::socket_t sender(ctx, ZMQ_PAIR);
		sender.connect("inproc://step3");

		while (true) {
			auto msg = s_recv(receiver);
			if (msg == "stop"s) {
				s_send(sender, msg);
				break;
			}
			std::ostringstream oss;
			oss << std::this_thread::get_id() << " " << msg;
			s_send(sender, oss.str());
		}
		t.join();
		});

	while (1) {
		auto msg = s_recv(receiver);
		std::cout << std::this_thread::get_id() << " received: " << msg << std::endl;
		if (msg == "stop"s) {
			break;
		}
	}

	t.join();
	return 0;
}