#pragma once
#include <boost/asio.hpp>
#include <iostream>
#include <thread>
#include <atomic>
#include <memory>
#include <vector>

class async_service
{
private:
	std::shared_ptr<boost::asio::ip::tcp::socket> m_sock;
	std::string m_response;
	boost::asio::streambuf m_request;

public:
	async_service(std::shared_ptr<boost::asio::ip::tcp::socket> sock) :
		m_sock(sock)
	{}

	void startHandling()
	{
		boost::asio::async_read_until(*m_sock, m_request, '\n', [this](const auto& ec, std::size_t bytes) {
			onRequestReceived(ec, bytes);
			});
	}

private:
	void onRequestReceived(const boost::system::error_code& ec, std::size_t bytes_transferred)
	{
		if (ec.value() != 0) {
			std::cout << ec.message() << std::endl;
			onFinish();
			return;
		}
		
		std::istream revstrm(&m_request);
		std::string tmp;
		std::getline(revstrm, tmp);
		std::cout << "Received from client: " << tmp << std::endl;

		m_response = processRequest(m_request);

		boost::asio::async_write(*m_sock, boost::asio::buffer(std::as_const(m_response)), [this](const auto& ec, std::size_t bytes) {
			onResponseSent(ec, bytes);
			});
	}

	void onResponseSent(const boost::system::error_code& ec, std::size_t bytes_transferred)
	{
		if (ec.value() != 0) {
			std::cout << ec.message() << std::endl;
		}
		onFinish();
	}

	void onFinish()
	{
		delete this;
	}

	std::string processRequest(boost::asio::streambuf& request)
	{
		int i = 0;
		while (i != 1000000) i++;
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
		std::string ret = "Response " + std::to_string(i) + "\n";
		return ret;
	}
};

class async_acceptor
{
private:
	boost::asio::io_service& m_ios;
	boost::asio::ip::tcp::acceptor m_acceptor;
	std::atomic_bool m_is_stopped;

public:
	async_acceptor(boost::asio::io_service& ios, int port):
		m_ios(ios),
		m_acceptor(m_ios, boost::asio::ip::tcp::endpoint(boost::asio::ip::address_v4::any(), port)),
		m_is_stopped{false}
	{}

	void start()
	{
		m_acceptor.listen();
		initAccept();
	}

	void stop()
	{
		m_is_stopped.store(true);
	}

private:
	void initAccept()
	{
		auto sock = std::make_shared<boost::asio::ip::tcp::socket>(m_ios);
		m_acceptor.async_accept(*sock, [this, sock](const auto& ec) {
			onAccept(ec, sock);
			});
	}

	void onAccept(const boost::system::error_code& ec, std::shared_ptr<boost::asio::ip::tcp::socket> sock)
	{
		if (ec.value() == 0) {
			(new async_service(sock))->startHandling();
		}
		else {
			std::cout << ec.message();
		}
		if (!m_is_stopped.load()) {
			initAccept();
		}
		else {
			m_acceptor.close();
		}
	}
};

class async_server
{
private:
	boost::asio::io_service m_ios;
	std::unique_ptr<boost::asio::io_service::work> m_work;
	std::unique_ptr<async_acceptor> m_acc;
	std::vector<std::unique_ptr<std::thread>> m_thread_pool;

public:
	static void start_server()
	{
		int port = 3333;
		try {
			async_server srv;
			int num_thread = std::min<int>(2, std::thread::hardware_concurrency() * 2);
			srv.start(port, num_thread);
			std::cout << "Press Enter to exit\n";
			std::cin.get();
			srv.stop();
		}
		catch (const std::exception& ex) {
			std::cout << ex.what() << std::endl;
		}
	}

	async_server()
	{
		m_work.reset(new boost::asio::io_service::work(m_ios));
	}

	void start(int port, int thread_pool_size)
	{
		assert(thread_pool_size > 0);
		m_acc.reset(new async_acceptor(m_ios, port));
		m_acc->start();
		for (int i = 0; i < thread_pool_size; ++i) {
			auto th = std::make_unique<std::thread>([this]() {
				m_ios.run();
				});
			m_thread_pool.push_back(std::move(th));
		}
	}

	void stop()
	{
		m_acc->stop();
		m_ios.stop();
		for (auto& th : m_thread_pool) {
			th->join();
		}
	}
};