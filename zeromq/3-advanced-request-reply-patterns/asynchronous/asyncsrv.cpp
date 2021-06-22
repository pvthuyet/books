//  Asynchronous client-to-server (DEALER to ROUTER)
//
//  While this example runs in a single process, that is to make
//  it easier to start and stop the example. Each task has its own
//  context and conceptually acts as a separate process.

#include <vector>
#include <thread>
#include <memory>
#include <functional>
#include <random>
#include <fmt/format.h>
#include <fmt/color.h>
#include <fmt/printf.h>

#include <zmq.hpp>
#include <Windows.h>
#include "zhelpers.hpp"

int gen_num(int a, int b)
{
	std::random_device rd;  //Will be used to obtain a seed for the random number engine
	std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
	std::uniform_int_distribution<> distrib(a, b);
	return distrib(gen);
}

inline int current_thread_id(int id)
{
	return id;
	std::ostringstream oss;
	oss << std::this_thread::get_id();
	//return oss.str();
}

//  This is our client task class.
//  It connects to the server, and then sends a request once per second
//  It collects responses as they arrive, and it prints them out. We will
//  run several client tasks in parallel, each with a different random ID.
//  Attention! -- this random work well only on linux.

class client_task
{
	zmq::context_t ctx_;
	zmq::socket_t client_socket_;
	int id_;

public:
	client_task(int id) : 
		ctx_(1), 
		client_socket_(ctx_, ZMQ_DEALER),
		id_{id}
	{}

	~client_task()
	{
		fmt::print(fmt::fg(fmt::color::cadet_blue), "[client {}] {}\n", current_thread_id(id_), "~client_task");
	}

	void start()
	{
		// generate random identity
		int iden = gen_num(1000, 9999);
		client_socket_.set(zmq::sockopt::routing_id, fmt::format("{}", iden));
		client_socket_.connect("tcp://localhost:5570");

		zmq::pollitem_t items[] = {
			{static_cast<void*>(client_socket_), 0, ZMQ_POLLIN, 0}
		};
		int req_num = 0;
		try {
			while (true) {
				for (int i = 0; i < 100; ++i) {
					zmq::poll(items, 1, 10);
					if (items[0].revents & ZMQ_POLLIN) {
						fmt::print("\n[{}] {} ", current_thread_id(id_), iden);
						s_dump(client_socket_);
					}
				}
				auto req_str = fmt::format("request #{}", ++req_num);
				s_send(client_socket_, req_str);
			}
		}
		catch (const std::exception& ex) {
			fmt::print(fmt::fg(fmt::color::red), "[client {}] {}\n", current_thread_id(id_), ex.what());
		}
	}
};

class server_worker
{
	zmq::context_t& ctx_;
	zmq::socket_t worker_;
	int id_;
public:
	server_worker(zmq::context_t& ctx, int socket_type, int id) :
		ctx_(ctx),
		worker_(ctx_, socket_type),
		id_{id}
	{}

	~server_worker()
	{
		fmt::print(fmt::fg(fmt::color::cadet_blue), "[server worker {}] {}\n", current_thread_id(id_), "~server_worker");
	}

	void work()
	{
		try {
			worker_.connect("inproc://backend");
			while (true) {
				zmq::message_t identity;
				zmq::message_t msg;
				zmq::message_t copied_id;
				zmq::message_t copied_msg;
				worker_.recv(&identity);
				worker_.recv(&msg);

				int replies = gen_num(0, 5);
				for (int i = 0; i < replies; ++i) {
					s_sleep(gen_num(1000, 3000));
					copied_id.copy(&identity);
					copied_msg.copy(&msg);
					worker_.send(copied_id, ZMQ_SNDMORE);
					worker_.send(copied_msg);
				}
			}
		}
		catch (std::exception const& ex) {
			fmt::print(fmt::fg(fmt::color::red), "[{}] {}\n", current_thread_id(id_), ex.what());
		}
	}
};

//  .split server task
//  This is our server task.
//  It uses the multithreaded server model to deal requests out to a pool
//  of workers and route replies back to clients. One worker can handle
//  one request at a time but one client can talk to multiple workers at
//  once.
class server_task
{
	zmq::context_t ctx_;
	zmq::socket_t frontend_;
	zmq::socket_t backend_;
	int id_;

public:
	server_task(int id) :
		ctx_(1),
		frontend_(ctx_, ZMQ_ROUTER),
		backend_(ctx_, ZMQ_DEALER),
		id_{id}
	{}

	~server_task()
	{
		fmt::print(fmt::fg(fmt::color::cadet_blue), "[server task {}] {}\n", current_thread_id(id_), "~server_task");
	}

	void run2()
	{
		constexpr const int KMAX_NUM = 5;
		std::vector<server_worker*> workers;
		std::vector<std::thread*> worker_threads;

		try {
			frontend_.bind("tcp://*:5570");
			backend_.bind("inproc://backend");

			for (int i = 0; i < KMAX_NUM; ++i) {
				workers.push_back(new server_worker(ctx_, ZMQ_DEALER, i + 10));

				worker_threads.push_back(new std::thread(std::bind(&server_worker::work, workers[i])));
				worker_threads[i]->detach();
			}

			zmq::proxy(frontend_, backend_);
		}
		catch (std::exception const& ex) {
			fmt::print(fmt::fg(fmt::color::red), "[{}] {}\n", current_thread_id(id_), ex.what());
		}

		for (int i = 0; i < KMAX_NUM; ++i) {
			delete workers[i];
			delete worker_threads[i];
		}
	}

	void run()
	{
		constexpr const int KMAX_NUM = 5;
		std::vector<std::unique_ptr<server_worker>> workers;
		std::vector<std::unique_ptr<std::thread>> worker_threads;

		try {
			frontend_.bind("tcp://*:5570");
			backend_.bind("inproc://backend");

			for (int i = 0; i < KMAX_NUM; ++i) {

				workers.push_back(std::make_unique<server_worker>(std::ref(ctx_), ZMQ_DEALER, 10 + i));
				worker_threads.push_back(std::make_unique<std::thread>([&workers, i]() {
					workers[i]->work();
				}));
				worker_threads[i]->detach();
			}

			zmq::proxy(frontend_, backend_);
		}
		catch (std::exception const& ex) {
			fmt::print(fmt::fg(fmt::color::red), "[{}] {}\n", current_thread_id(id_), ex.what());
		}
	}
};

int main()
{
	client_task ct1{1};
	client_task ct2{ 2 };
	client_task ct3{ 3 };
	server_task st{4};

	std::thread t1(std::bind(&client_task::start, &ct1));
	std::thread t2(std::bind(&client_task::start, &ct2));
	std::thread t3(std::bind(&client_task::start, &ct3));
	std::thread t4(std::bind(&server_task::run, &st));

	t1.detach();
	t2.detach();
	t3.detach();
	t4.detach();

	std::cout << "Press ENTER to exit\n";
	std::cin.get();
	return EXIT_SUCCESS;
}
