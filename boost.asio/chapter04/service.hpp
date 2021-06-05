#pragma once
#include <boost/asio.hpp>
#include <iostream>
#include <thread>

class service_demo
{
public:
	void handleClient(boost::asio::ip::tcp::socket& sock)
	{
		try {
			boost::asio::streambuf request;
			boost::asio::read_until(sock, request, '\n');
			auto revbuf = request.data();
			std::string str(boost::asio::buffers_begin(revbuf), boost::asio::buffers_begin(revbuf) + revbuf.size());
			std::cout << "receive: " << str << std::endl;
			int i{};
			while (i != 1000000) {
				i++;
				std::this_thread::sleep_for(std::chrono::milliseconds(500));
				const std::string response = "response from server " + std::to_string(i) + "\n";
				boost::asio::write(sock, boost::asio::buffer(response));
			}
		}
		catch (const std::exception const& ex) {
			std::cout << ex.what() << std::endl;
		}
	}
};

class acceptor_demo
{
public:
	acceptor_demo(boost::asio::io_service& ios, unsigned short port) :
		m_ios(ios),
		m_acceptor(m_ios, boost::asio::ip::tcp::endpoint(boost::asio::ip::address_v4::any(), port))
	{
		m_acceptor.listen();
	}

	void accept()
	{
		boost::asio::ip::tcp::socket sock(m_ios);
		m_acceptor.accept(sock);
		service_demo srv;
		srv.handleClient(sock);
	}

private:
	boost::asio::io_service & m_ios;
	boost::asio::ip::tcp::acceptor m_acceptor;
};

class server_demo
{
public:
	server_demo() : m_stop{ false } {}

	void start(unsigned short port)
	{
		m_thread = std::make_unique<std::thread>([this, port]() {
			run(port);
			});
	}

	void stop()
	{
		m_stop.store(true);
		m_thread->join();
	}

private:
	void run(unsigned short port)
	{
		acceptor_demo acc(m_ios, port);
		while (!m_stop.load()) {
			acc.accept();
		}
	}

private:
	std::unique_ptr<std::thread> m_thread;
	std::atomic_bool m_stop;
	boost::asio::io_service m_ios;
};