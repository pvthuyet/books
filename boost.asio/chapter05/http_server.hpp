#pragma once
#include <boost/asio.hpp>
#include <boost/filesystem.hpp>
#include <fstream>
#include <atomic>
#include <thread>
#include <iostream>
#include <map>

class async_service
{
public:
	std::shared_ptr<boost::asio::ip::tcp::socket> m_sock;
	boost::asio::streambuf m_request;
	std::map<std::string, std::string> m_request_headers;
	std::string m_requested_resource;

	std::unique_ptr<char[]> m_resource_buffer;
	unsigned int m_response_status_code;
	std::size_t m_resource_size_bytes;
	std::string m_response_headers;
	std::string m_response_status_line;
	inline static std::map<int, std::string> http_status_table = {
		{200, "200 OK"},
		{404, "200 Not Found"},
		{413, "413 Request Entity Too Large"},
		{500, "500 Server Error"},
		{501, "501 Not Implemented"},
		{505, "505 HTTP Version Not Supported"}
	};

public:
	async_service(std::shared_ptr<boost::asio::ip::tcp::socket> sock):
		m_sock(sock),
		m_request(4096),
		m_response_status_code(200),
		m_resource_size_bytes(0)
	{}

	void start_handling()
	{
		boost::asio::async_read_until(*m_sock,
			m_request,
			"\r\n",
			[this](auto const& ec, std::size_t bytes) {
				on_request_line_received(ec, bytes);
			});
	}

private:
	void on_request_line_received(const boost::system::error_code& ec, std::size_t bytes)
	{
		if (0 != ec.value()) {
			std::cout << ec.message() << std::endl;
			if (ec == boost::asio::error::not_found) {
				m_response_status_code = 413;
				send_response();
				return;
			}
			else {
				on_finish();
				return;
			}
		}

		std::string request_line;
		std::istream request_stream(&m_request);
		std::getline(request_stream, request_line, '\r');
		// remove '\n'
		request_stream.get();

		std::string request_method;
		std::istringstream request_line_stream(request_line);
		request_line_stream >> request_method;

		// we only support GET method
		if (request_method.compare("GET") != 0) {
			m_response_status_code = 501;
			send_response();
			return;
		}

		request_line_stream >> m_requested_resource;
		
		std::string request_http_version;
		request_line_stream >> request_http_version;

		if (request_http_version.compare("HTTP/1.1") != 0) {
			m_response_status_code = 505;
			send_response();
			return;
		}

		boost::asio::async_read_until(*m_sock,
			m_request,
			"\r\n\r\n",
			[this](const auto& ec, std::size_t bytes) {
				on_header_received(ec, bytes);
			});
	}

	void on_header_received(boost::system::error_code const& ec, std::size_t bytes)
	{
		if (ec.value() != 0) {
			std::cout << ec.message() << std::endl;
			if (ec == boost::asio::error::not_found) {
				m_response_status_code = 413;
				send_response();
				return;
			}
			else {
				on_finish();
				return;
			}
		}

		std::istream req_stream(&m_request);
		std::string header_name, header_value;
		while (!req_stream.eof()) {
			std::getline(req_stream, header_name, ':');
			if (!req_stream.eof()) {
				std::getline(req_stream, header_value, '\r');
				req_stream.get();
				m_request_headers[header_name] = header_value;
			}
		}
		process_request();
		send_response();
	}

	void process_request()
	{
		std::string res_file_path = std::string("D:\\http_root") + m_requested_resource;
		if (!boost::filesystem::exists(res_file_path)) {
			m_response_status_code = 404;
			return;
		}

		std::ifstream res_fstream(res_file_path, std::ifstream::binary);
		if (!res_fstream.is_open()) {
			m_response_status_code = 500;
			return;
		}

		res_fstream.seekg(0, std::ifstream::end);
		m_resource_size_bytes = static_cast<std::size_t>(res_fstream.tellg());

		m_resource_buffer.reset(new char[m_resource_size_bytes]);

		res_fstream.seekg(std::ifstream::beg);
		res_fstream.read(m_resource_buffer.get(), 
			m_resource_size_bytes
			);

		m_response_headers += std::string("content-length") + ": " + std::to_string(m_resource_size_bytes) + "\r\n";
	}

	void send_response()
	{
		m_sock->shutdown(boost::asio::ip::tcp::socket::shutdown_receive);
		auto status_line = http_status_table.at(m_response_status_code);
		m_response_status_line = std::string("HTTP/1.1 ") + status_line + "\r\n";
		m_response_headers += "\r\n";

		std::vector<boost::asio::const_buffer> response_buffers;
		response_buffers.push_back(boost::asio::buffer(m_response_status_line));

		if (m_response_headers.length() > 0) {
			response_buffers.push_back(boost::asio::buffer(m_response_headers));
		}

		if (m_resource_size_bytes > 0) {
			response_buffers.push_back(boost::asio::buffer(m_resource_buffer.get(),
				m_resource_size_bytes
				));
		}

		// 
		boost::asio::async_write(*m_sock, response_buffers,
			[this](const auto& ec, std::size_t bytes) {
				on_response_sent(ec, bytes);
			});
	}

	void on_response_sent(const boost::system::error_code& ec, std::size_t bytes)
	{
		if (!ec) {
			std::cerr << ec.message() << std::endl;
		}

		m_sock->shutdown(boost::asio::ip::tcp::socket::shutdown_both);
		on_finish();
	}

	void on_finish()
	{
		delete this;
	}
};

class async_acceptor
{
private:
	boost::asio::io_service& m_ios;
	boost::asio::ip::tcp::acceptor m_acceptor;
	std::atomic_bool m_is_stopped;

public:
	async_acceptor(boost::asio::io_service& ios, int port) :
		m_ios(ios),
		m_acceptor(m_ios, boost::asio::ip::tcp::endpoint(boost::asio::ip::address_v4::any(), port)),
		m_is_stopped{ false }
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
			(new async_service(sock))->start_handling();
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