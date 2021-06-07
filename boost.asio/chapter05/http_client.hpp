#pragma once
#include <boost/asio.hpp>
#include <string>
#include <map>
#include <mutex>
#include <iostream>
#include "http_errors.hpp"

class HTTPClient;
class HTTPRequest;
class HTTPResponse;

typedef void(*Callback)(const HTTPRequest& request, const HTTPResponse& response, const boost::system::error_code& ec);

class HTTPResponse
{
	friend class HTTPRequest;
	HTTPResponse() :
		m_response_stream(&m_response_buf)
	{}

public:
	unsigned int get_status_code() const { return m_status_code; }
	const std::string& get_status_message() const { return m_status_message; }
	const std::map<std::string, std::string>& get_headers() { return m_headers; }
	const std::istream& get_response() const { return m_response_stream; }

private:
	boost::asio::streambuf& get_response_buf() { return m_response_buf; }

	void set_status_code(unsigned int status_code)
	{
		m_status_code = status_code;
	}

	void set_status_message(const std::string& status_message)
	{
		m_status_message = status_message;
	}

	void add_header(const std::string& name, const std::string& value)
	{
		m_headers[name] = value;
	}

private:
	unsigned int						m_status_code;
	std::string							m_status_message;
	std::map<std::string, std::string>	m_headers;
	boost::asio::streambuf				m_response_buf;
	std::iostream						m_response_stream;
};

class HTTPRequest
{
	friend class HTTPClient;
	static constexpr unsigned int DEFAULT_PORT = 80;
	HTTPRequest(boost::asio::io_service& ios, unsigned int id):
		m_port(DEFAULT_PORT),
		m_id(id),
		m_callback(nullptr),
		m_sock(ios),
		m_resolver(ios),
		m_was_cancelled(false),
		m_ios(ios)
	{}

public:
	std::string		m_host;
	unsigned int	m_port;
	std::string		m_uri;
	unsigned int	m_id;
	Callback		m_callback;
	std::string		m_request_buf;
	boost::asio::ip::tcp::socket m_sock;
	boost::asio::ip::tcp::resolver m_resolver;
	HTTPResponse	m_response;
	bool			m_was_cancelled;
	std::mutex		m_cancel_mux;
	boost::asio::io_service& m_ios;

public:
	void execute()
	{
		boost::asio::ip::tcp::resolver::query rel_query(m_host,
			std::to_string(m_port),
			boost::asio::ip::tcp::resolver::query::numeric_service
			);

		std::unique_lock<std::mutex> cancel_lock(m_cancel_mux);
		if (m_was_cancelled) {
			cancel_lock.unlock();
			on_finish(boost::system::error_code(boost::asio::error::operation_aborted));
			return;
		}

		m_resolver.async_resolve(rel_query, [this](const boost::system::error_code& ec, boost::asio::ip::tcp::resolver::iterator it) {
			on_host_name_resolved(ec, it);
			});
	}

	void cancel()
	{
		std::lock_guard<std::mutex> lock(m_cancel_mux);
		m_resolver.cancel();
		if (m_sock.is_open()) {
			m_sock.cancel();
		}
	}

private:
	void on_host_name_resolved(const boost::system::error_code& ec, boost::asio::ip::tcp::resolver::iterator it)
	{
		if (ec.value() != 0) {
			on_finish(ec);
			return;
		}

		std::unique_lock<std::mutex> cancel_lock(m_cancel_mux);
		if (m_was_cancelled) {
			cancel_lock.unlock();
			on_finish(boost::system::error_code(boost::asio::error::operation_aborted));
			return;
		}

		// connect to the host
		boost::asio::async_connect(m_sock, it, [this](auto const& ec, boost::asio::ip::tcp::resolver::iterator it) {
			on_connection_established(ec, it);
			});
	}

	void on_connection_established(const boost::system::error_code& ec, boost::asio::ip::tcp::resolver::iterator it)
	{
		if (ec.value() != 0) {
			on_finish(ec);
			return;
		}
		m_request_buf += "GET " + m_uri + " HTTP/1.1\r\n";
		// header
		m_request_buf += "Host: " + m_host + "\r\n";
		m_request_buf += "\r\n";

		std::unique_lock<std::mutex> cancel_lock(m_cancel_mux);
		if (m_was_cancelled) {
			cancel_lock.unlock();
			on_finish(boost::system::error_code(boost::asio::error::operation_aborted));
			return;
		}

		boost::asio::async_write(m_sock,
			boost::asio::buffer(m_request_buf),
			[this](const boost::system::error_code& ec, std::size_t bytes) {
				on_request_sent(ec, bytes);
			});
	}

	void on_request_sent(boost::system::error_code const& ec, std::size_t bytes)
	{
		if (ec.value() != 0) {
			on_finish(ec);
			return;
		}
		m_sock.shutdown(boost::asio::ip::tcp::socket::shutdown_send);

		std::unique_lock<std::mutex> cancel_lock(m_cancel_mux);
		if (m_was_cancelled) {
			cancel_lock.unlock();
			on_finish(boost::system::error_code(boost::asio::error::operation_aborted));
			return;
		}

		// read the status line
		boost::asio::async_read_until(m_sock, m_response.get_response_buf(), "\r\n",
			[this](const boost::system::error_code& ec, std::size_t bytes) {
				on_status_line_received(ec, bytes);
			});
	}

	void on_status_line_received(const boost::system::error_code& ec, std::size_t bytes)
	{
		if (ec.value() != 0) {
			on_finish(ec);
			return;
		}

		// parse the status line
		std::string http_version;
		std::string str_status_code;
		std::string status_message;

		std::istream response_stream(&m_response.get_response_buf());
		response_stream >> http_version;

		if (http_version != "HTTP/1.1") {
			on_finish(http_errors::invalid_response);
			return;
		}

		response_stream >> str_status_code;
		unsigned long status_code = 200;
		try {
			status_code = std::stoul(str_status_code);
		}
		catch (const std::exception&) {
			on_finish(http_errors::invalid_response);
			return;
		}

		std::getline(response_stream, status_message, '\r');
		// remove symbol '\n' from buffer
		response_stream.get();

		m_response.set_status_code(status_code);
		m_response.set_status_message(status_message);

		std::unique_lock<std::mutex> cancel_lock(m_cancel_mux);
		if (m_was_cancelled) {
			cancel_lock.unlock();
			on_finish(boost::system::error_code(boost::asio::error::operation_aborted));
			return;
		}

		boost::asio::async_read_until(m_sock, m_response.get_response_buf(), "\r\n\r\n", [this](boost::system::error_code const& ec, std::size_t bytes) {
			on_headers_received(ec, bytes);
			});
	}

	void on_headers_received(const boost::system::error_code& ec, std::size_t bytes)
	{
		if (ec.value() != 0) {
			on_finish(ec);
			return;
		}

		std::string header, header_name, header_value;
		std::istream response_stream(&m_response.get_response_buf());

		while (true) {
			std::getline(response_stream, header, '\r');

			// remove '\n'
			response_stream.get();
			if (header == "")
				break;

			std::size_t sep_pos = header.find(':');
			if (sep_pos != std::string::npos) {
				header_name = header.substr(0, sep_pos);
				if (sep_pos < header.length() - 1) {
					header_value = header.substr(sep_pos + 1);
				}
				else {
					header_value = "";
				}
				m_response.add_header(header_name, header_value);
			}
		}

		std::unique_lock<std::mutex> cancel_lock(m_cancel_mux);
		if (m_was_cancelled) {
			cancel_lock.unlock();
			on_finish(boost::system::error_code(boost::asio::error::operation_aborted));
			return;
		}

		// read body
		boost::asio::async_read(m_sock,
			m_response.get_response_buf(),
			[this](const auto& ec, std::size_t bytes) {
				on_response_body_received(ec, bytes);
			});
	}

	void on_response_body_received(const boost::system::error_code& ec, std::size_t bytes)
	{
		if (ec == boost::asio::error::eof) {
			on_finish(boost::system::error_code());
		}
		else {
			on_finish(ec);
		}
	}

	void on_finish(const boost::system::error_code& ec)
	{
		m_callback(*this, m_response, ec);
	}
};

static void handler(const HTTPRequest& req, const HTTPResponse& res, const boost::system::error_code& ec)
{
	if (ec.value() == 0) {
		std::cout << "Request #" << req.m_id << " finished. Response to server: " << res.get_response().rdbuf() << std::endl;

	}
	else if (ec == boost::asio::error::operation_aborted) {
		std::cout << "Request #" << req.m_id << " has been cancelled by user\n";
	}
	else {
		std::cout << ec.message() << "\n";
	}
}

class HTTPClient
{
public:
	static void start_client()
	{
		try {
			HTTPClient client;
			auto req1 = client.create_request(1);
			req1->m_host = "localhost";
			req1->m_uri = "/index.html";
			req1->m_port = 3333;
			req1->m_callback = handler;
			req1->execute();
			std::cin.get();
			client.close();
		}
		catch (std::exception const& ex) {
			std::cout << ex.what() << std::endl;
		}
	}

	HTTPClient()
	{
		m_work.reset(new boost::asio::io_service::work(m_ios));
		m_thread.reset(new std::thread([this]() {
			m_ios.run();
			}));
	}

	std::shared_ptr<HTTPRequest> create_request(unsigned int id)
	{
		return std::shared_ptr<HTTPRequest>(new HTTPRequest(m_ios, id));
	}

	void close()
	{
		m_work.reset(nullptr);
		m_thread->join();
	}

private:
	boost::asio::io_service m_ios;
	std::unique_ptr<boost::asio::io_service::work> m_work;
	std::unique_ptr<std::thread> m_thread;
};