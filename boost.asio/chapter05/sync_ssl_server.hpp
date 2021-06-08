#pragma once
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <thread>
#include <atomic>
#include <iostream>

class ssl_service
{
public:
	void handle_client(boost::asio::ssl::stream<boost::asio::ip::tcp::socket>& ssl_stream)
	{
		try {
			ssl_stream.handshake(boost::asio::ssl::stream_base::server);
			boost::asio::streambuf request;
			boost::asio::read_until(ssl_stream, request, '\n');
			int i = 0;
			while (i != 1000000) ++i;
			std::this_thread::sleep_for(std::chrono::milliseconds(500));
			std::string response = "Response\n";
			boost::asio::write(ssl_stream, boost::asio::buffer(response));
		}
		catch (std::exception const& ex) {
			std::cerr << ex.what() << std::endl;
		}
	}
};

class ssl_acceptor
{
private:
	boost::asio::io_service& m_ios;
	boost::asio::ip::tcp::acceptor m_acceptor;
	boost::asio::ssl::context m_ssl_context;

public:
	ssl_acceptor(boost::asio::io_service& ios, int port)
		: m_ios(ios),
		m_acceptor(m_ios, boost::asio::ip::tcp::endpoint(boost::asio::ip::address_v4::any(), port)),
		m_ssl_context(boost::asio::ssl::context::sslv23_server)
	{
		m_ssl_context.set_options(
			boost::asio::ssl::context::default_workarounds
			| boost::asio::ssl::context::no_sslv2
			| boost::asio::ssl::context::single_dh_use
		);

		m_ssl_context.set_password_callback([this](std::size_t len, boost::asio::ssl::context::password_purpose purpose) {
			return get_password(len, purpose);
			});

		m_ssl_context.use_certificate_chain_file("D:\\projects\\books\\boost.asio\\chapter05\\cert.pem");
		m_ssl_context.use_private_key_file("D:\\projects\\books\\boost.asio\\chapter05\\key.pem",
			boost::asio::ssl::context::pem);
		//m_ssl_context.use_tmp_dh_file("D:\\projects\\books\\boost.asio\\chapter05\\dhparams.pem");

		m_acceptor.listen();
	}

	void accept()
	{
		boost::asio::ssl::stream<boost::asio::ip::tcp::socket> ssl_stream(m_ios, m_ssl_context);
		m_acceptor.accept(ssl_stream.lowest_layer());
		ssl_service svc;
		svc.handle_client(ssl_stream);
	}

private:
	std::string get_password(std::size_t len, boost::asio::ssl::context::password_purpose purpose) const
	{
		return "pass";
	}
};

class ssl_server
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
			ssl_server srv;
			srv.start(port);
			std::cin.get();
			srv.stop();
		}
		catch (const std::exception& ex) {
			std::cerr << ex.what() << std::endl;
		}
	}

	void start(int port)
	{
		m_thread.reset(new std::thread([this, port](){
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
		ssl_acceptor acc(m_ios, port);
		while (!m_stop.load()) {
			acc.accept();
		}
	}
};