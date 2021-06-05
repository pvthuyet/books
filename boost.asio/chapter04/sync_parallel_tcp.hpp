#pragma once
#include <atomic>
#include <thread>
#include <iostream>
#include <memory>
#include <boost/asio.hpp>

class par_service
{
public:
	void startHandlingClient(std::shared_ptr<boost::asio::ip::tcp::socket> sock)
	{
		std::thread th([this, sock]() {
			handleClient(sock);
			});
		th.detach();
	}

private:
	void handleClient(std::shared_ptr<boost::asio::ip::tcp::socket> sock)
	{
		try {
			boost::asio::streambuf request;
			boost::asio::read_until(*sock, request, '\n');
			int i = 0;
			while (i != 1000000) i++;
			std::this_thread::sleep_for(std::chrono::milliseconds(500));
			const std::string response = "Response from " + std::to_string(i) + "\n";
			boost::asio::write(*sock.get(), boost::asio::buffer(response));
		}
		catch (std::exception const& ex) {
			std::cout << ex.what() << std::endl;
		}
		delete this;
	}
};

class par_acceptor
{
private:
	boost::asio::io_service& m_ios;
	boost::asio::ip::tcp::acceptor m_acceptor;

public:
	par_acceptor(boost::asio::io_service& ios, unsigned short port) :
		m_ios(ios),
		m_acceptor(m_ios, boost::asio::ip::tcp::endpoint(boost::asio::ip::address_v4::any(), port))
	{
		m_acceptor.listen();
	}

	void accept()
	{
		auto sock = std::make_shared<boost::asio::ip::tcp::socket>(m_ios);
		m_acceptor.accept(*sock);
		(new par_service())->startHandlingClient(sock);
	}
};

class par_server
{
private:
	std::unique_ptr<std::thread> m_thread;
	std::atomic_bool m_stop;
	boost::asio::io_service m_ios;

public:
	static void start_server()
	{
		int port = 3333;
		try {
			par_server srv;
			srv.start(port);
			std::cout << "Press Enter to exit\n";
			std::cin.get();
			srv.stop();
		}
		catch (const std::exception& ex) {
			std::cout << ex.what() << std::endl;
		}
	}

	void start(int port)
	{
		std::cout << "Start server\n";
		m_thread.reset(new std::thread([this, port]() {
			run(port);
			}));
	}

	void stop()
	{
		m_stop.store(true);
		m_thread->join();
	}

private:
	void run(int port)
	{
		par_acceptor acc(m_ios, port);
		while (!m_stop.load()) {
			acc.accept();
		}
	}
}; 