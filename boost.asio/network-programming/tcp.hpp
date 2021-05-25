#pragma once

#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>

namespace asio = boost::asio;
void synchronous_client()
{
	using namespace std::string_literals;
	boost::mutex mux;
	auto print = [&mux](std::string msg) {
		boost::lock_guard<boost::mutex> lock(mux);
		std::cout << msg << std::endl;
	};

	auto job = [print](auto ios, int id) {
		print("Thread "s + std::to_string(id) + " start"s);
		try {
			while (true) {
				boost::system::error_code ec;
				ios->run(ec);
				if (ec) {
					print(ec.message());
				}
				break;
			}
		}
		catch (std::exception const& e) {
			print(e.what());
		}
		print("Thread "s + std::to_string(id) + " end"s);
	};

	auto ios = boost::make_shared<asio::io_service>();
	auto worker = boost::make_shared<asio::io_service::work>(*ios);
	auto srd = boost::make_shared<asio::io_service::strand>(*ios);
	print("Press Enter to exit.");

	boost::thread_group thrds;
	for (int i = 0; i < 3; ++i) {
		thrds.create_thread(std::bind(job, ios, i));
	}

	asio::ip::tcp::socket socket(*ios);
	try {
		asio::ip::tcp::resolver resolver(*ios);
		asio::ip::tcp::resolver::query query("www.packtpub.com", boost::lexical_cast<std::string>(80));
		auto it = resolver.resolve(query);
		auto edp = *it;
		print("Connecting to " + edp.host_name());
		socket.connect(edp);
		print("Connected");
	}
	catch (std::exception const& e) {
		print(e.what());
	}

	std::cin.get();

	boost::system::error_code ec;
	socket.shutdown(asio::ip::tcp::socket::shutdown_both, ec);
	socket.close(ec);

	ios->stop();
	thrds.join_all();
}

void onconnect(boost::system::error_code const& ec, std::function<void(std::string)> print) 
{
	if (ec) print("On connect error" + ec.message());
	else print("Connected");
};

void asynchronous_client()
{
	using namespace std::string_literals;
	boost::mutex mux;
	auto print = [&mux](std::string msg) {
		boost::lock_guard<boost::mutex> lock(mux);
		std::cout << msg << std::endl;
	};

	auto job = [print](auto ios, int id) {
		print("Thread "s + std::to_string(id) + " start"s);
		try {
			while (true) {
				boost::system::error_code ec;
				ios->run(ec);
				if (ec) {
					print(ec.message());
				}
				break;
			}
		}
		catch (std::exception const& e) {
			print(e.what());
		}
		print("Thread "s + std::to_string(id) + " end"s);
	};

	auto ios = boost::make_shared<asio::io_service>();
	auto worker = boost::make_shared<asio::io_service::work>(*ios);
	auto srd = boost::make_shared<asio::io_service::strand>(*ios);
	print("Press Enter to exit.");

	boost::thread_group thrds;
	for (int i = 0; i < 3; ++i) {
		thrds.create_thread(std::bind(job, ios, i));
	}

	auto socket = boost::make_shared<asio::ip::tcp::socket>(*ios);
	try {
		asio::ip::tcp::resolver resolver(*ios);
		asio::ip::tcp::resolver::query query("www.packtpub.com", boost::lexical_cast<std::string>(80));
		auto it = resolver.resolve(query);
		auto ep = *it;
		print("Connecting to " + ep.host_name());
		socket->async_connect(ep, boost::bind(onconnect, _1, print));
	}
	catch (std::exception const& e) {
		print(e.what());
	}

	std::cin.get();

	boost::system::error_code ec;
	socket->shutdown(asio::ip::tcp::socket::shutdown_both, ec);
	socket->close(ec);

	ios->stop();
	thrds.join_all();
}