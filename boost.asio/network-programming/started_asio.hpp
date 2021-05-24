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