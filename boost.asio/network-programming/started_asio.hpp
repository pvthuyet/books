#pragma once

#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/thread.hpp>
#include <boost/bind.hpp>

namespace asio = boost::asio;
void blocking_the_run1()
{
	asio::io_service ios;
	ios.run();
	std::cout << "we will see this line\n";
}

void blocking_the_run2()
{
	asio::io_service ios;
	asio::io_service::work worker(ios);
	ios.run();
	std::cout << "we will not see this line\n";
}

void non_blocking_poll1()
{
	asio::io_service ios;
	for (int i = 0; i < 5; ++i) {
		ios.poll();
		std::cout << "Line: " << i << '\n';
	}
}

void non_blocking_poll2()
{
	asio::io_service ios;
	asio::io_service::work worker(ios);
	for (int i = 0; i < 5; ++i) {
		ios.poll();
		std::cout << "Line: " << i << '\n';
	}
}


void remove_work_object1()
{
	asio::io_service ios;
	auto worker = boost::make_shared<asio::io_service::work>(ios);
	worker.reset();
	ios.run();
	std::cout << "we will not see this line\n";
}

void dealing_many_thread1()
{
	asio::io_service ios;
	int a = 0;
	auto job = [&a, &ios]() {
		std::cout << boost::this_thread::get_id() << " - " << ++a << std::endl;
		ios.run();
		std::cout << boost::this_thread::get_id() << " - " << "End.\n";
	};
	auto woker = boost::make_shared<asio::io_service::work>(ios);
	std::cout << "Press ENTER key to exit!\n";
	boost::thread_group threads;
	for (int i = 0; i < 5; ++i) {
		threads.create_thread(job);
	}
	std::cin.get();
	ios.stop();
	threads.join_all();
}

void working_boost_bind()
{
	auto job = [](auto ios, int counter) {
		std::cout << boost::this_thread::get_id() << "\t- " << counter << std::endl;
		ios->run();
		std::cout << boost::this_thread::get_id() << "\t- End\n";
	};

	auto ios = boost::make_shared<asio::io_service>();
	auto worker = boost::make_shared<asio::io_service::work>(*ios);
	boost::thread_group threads;
	for (int i = 0; i < 5; ++i) {
		threads.create_thread(std::bind(job, ios, i));
	}
	std::cout << "Press ENTER key to exit!\n";
	std::cin.get();
	ios->stop();
	threads.join_all();
}

void using_post()
{
	boost::mutex mux;
	
	auto fac = [&mux](size_t n) {
		{
			boost::lock_guard<boost::mutex> lock(mux);
			std::cout << boost::this_thread::get_id() << "\t- Calculating " << n << "! factorial\n";
		}
		if (n < 2) return n;
		size_t ret = 1;
		for (int i = 1; i <= n; ++i) {
			ret *= i;
			boost::this_thread::sleep_for(boost::chrono::milliseconds(1000));
		}
		{
			boost::lock_guard<boost::mutex> lock(mux);
			std::cout << boost::this_thread::get_id() << "\t- " << n << "! = " << ret << std::endl;
		}
		return ret;
	};

	auto job = [&mux](auto ios, int counter) {
		{
			boost::lock_guard<boost::mutex> lock(mux);
			std::cout << "thread " << counter << " - start" << std::endl;
		}

		ios->run();
		{
			boost::lock_guard<boost::mutex> lock(mux);
			std::cout << "thread " << counter << " - end" << std::endl;
		}
	};

	{
		boost::lock_guard<boost::mutex> lock(mux);
		std::cout << boost::this_thread::get_id() << "\t- The program will exit once all work has finished.\n";
	}

	auto ios = boost::make_shared<asio::io_service>();
	auto worker = boost::make_shared<asio::io_service::work>(*ios);

	boost::thread_group threads;
	for (int i = 0; i < 5; ++i) {
		threads.create_thread(std::bind(job, ios, i));
	}

	ios->post(std::bind(fac, 5));
	ios->post(std::bind(fac, 6));
	ios->post(std::bind(fac, 7));

	worker.reset();
	threads.join_all();
}

void using_dispatch()
{
	boost::mutex mux;

	auto job = [&mux](auto ios, int counter) {
		{
			boost::lock_guard<boost::mutex> lock(mux);
			std::cout << "thread " << counter << " - start" << std::endl;
		}

		ios->run();

		{
			boost::lock_guard<boost::mutex> lock(mux);
			std::cout << "thread " << counter << " - end" << std::endl;
		}
	};

	auto post = [&mux](int i) {
		{
			boost::lock_guard<boost::mutex> lock(mux);
			std::cout << "post() for i = " << i << std::endl;
		}
	};

	auto dispatch = [&mux](int i) {
		{
			boost::lock_guard<boost::mutex> lock(mux);
			std::cout << "dispatch() for i = " << i << std::endl;
		}
	};

	auto running = [dispatch, post](auto ios) {
		for (int i = 0; i < 5; ++i) {
			ios->dispatch(std::bind(dispatch, i));
			ios->post(std::bind(post, i));
			boost::this_thread::sleep_for(boost::chrono::milliseconds(1000));
		}
	};

	{
		boost::lock_guard<boost::mutex> lock(mux);
		std::cout << boost::this_thread::get_id() << "\t- The program will exit once all work has finished.\n";
	}

	auto ios = boost::make_shared<asio::io_service>();
	auto worker = boost::make_shared<asio::io_service::work>(*ios);

	boost::thread_group threads;
	for (int i = 0; i < 5; ++i) {
		threads.create_thread(std::bind(job, ios, i));
	}
	ios->post(std::bind(running, ios));

	worker.reset();
	threads.join_all();
}

void using_none_strand()
{
	boost::mutex mux;
	auto print = [&mux](std::string msg) {
		boost::lock_guard<boost::mutex> lock(mux);
		std::cout << msg << std::endl;
	};

	auto job = [print](auto ios, int n) {
		print("Thread " + std::to_string(n) + " start");
		ios->run();
		print("Thread " + std::to_string(n) + " end");
	};

	auto processing = [](int num) {
		std::cout << "Processing " << num << std::endl;
	};

	auto ios = boost::make_shared<asio::io_service>();
	auto worker = boost::make_shared<asio::io_service::work>(*ios);
	print("The program will be exit once all work has finished");
	
	boost::thread_group thrs;
	for (int i = 0; i < 5; ++i) {
		thrs.create_thread(std::bind(job, ios, i));
	}

	boost::this_thread::sleep_for(boost::chrono::milliseconds(500));
	ios->post(std::bind(processing, 1));
	ios->post(std::bind(processing, 2));
	ios->post(std::bind(processing, 3));
	ios->post(std::bind(processing, 4));
	ios->post(std::bind(processing, 5));

	worker.reset();
	thrs.join_all();
}

void using_strand()
{
	boost::mutex mux;
	auto print = [&mux](std::string msg) {
		boost::lock_guard<boost::mutex> lock(mux);
		std::cout << msg << std::endl;
	};

	auto job = [print](auto ios, int n) {
		print("Thread " + std::to_string(n) + " start");
		ios->run();
		print("Thread " + std::to_string(n) + " end");
	};

	auto processing = [](int num) {
		std::cout << "Processing " << num << std::endl;
	};

	auto ios = boost::make_shared<asio::io_service>();
	auto worker = boost::make_shared<asio::io_service::work>(*ios);
	asio::io_service::strand srd(*ios);
	print("The program will be exit once all work has finished");

	boost::thread_group thrs;
	for (int i = 0; i < 5; ++i) {
		thrs.create_thread(std::bind(job, ios, i));
	}

	boost::this_thread::sleep_for(boost::chrono::milliseconds(500));
	//ios->post(srd.wrap(std::bind(processing, 1)));
	//ios->post(srd.wrap(std::bind(processing, 2)));
	//ios->post(srd.wrap(std::bind(processing, 3)));
	//ios->post(srd.wrap(std::bind(processing, 4)));
	//ios->post(srd.wrap(std::bind(processing, 5)));

	boost::asio::post(srd, std::bind(processing, 1));
	boost::asio::post(srd, std::bind(processing, 2));
	boost::asio::post(srd, std::bind(processing, 3));
	boost::asio::post(srd, std::bind(processing, 4));
	boost::asio::post(srd, std::bind(processing, 5));

	worker.reset();
	thrs.join_all();
}

void handling_exception()
{
	boost::mutex mux;
	auto print = [&mux](std::string msg) {
		boost::lock_guard<boost::mutex> lock(mux);
		std::cout << msg << std::endl;
	};

	auto job = [print](auto ios, int n) {
		print("Thread " + std::to_string(n) + " start");
		try {
			ios->run();
		}
		catch (std::exception const& e) {
			std::cout << e.what() << std::endl;
		}
		
		print("Thread " + std::to_string(n) + " end");
	};

	auto processing = [](int num) {
		std::cout << "Processing " << num << std::endl;
		throw std::runtime_error(std::string("Exeption processing ") + std::to_string(num));
	};

	auto ios = boost::make_shared<asio::io_service>();
	auto worker = boost::make_shared<asio::io_service::work>(*ios);
	asio::io_service::strand srd(*ios);
	print("The program will be exit once all work has finished");

	boost::thread_group thrs;
	for (int i = 0; i < 5; ++i) {
		thrs.create_thread(std::bind(job, ios, i));
	}

	boost::this_thread::sleep_for(boost::chrono::milliseconds(500));
	boost::asio::post(srd, std::bind(processing, 1));
	boost::asio::post(srd, std::bind(processing, 2));
	boost::asio::post(srd, std::bind(processing, 3));

	worker.reset();
	thrs.join_all();
}

