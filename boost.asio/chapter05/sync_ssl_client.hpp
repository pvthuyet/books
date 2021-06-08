#pragma once
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <iostream>

class SyncSSLClient
{
private:
	boost::asio::io_service m_ios;
	boost::asio::ip::tcp::endpoint m_ep;
	boost::asio::ssl::context m_ssl_context;
	boost::asio::ssl::stream<boost::asio::ip::tcp::socket> m_ssl_stream;

public:
	SyncSSLClient(const std::string& rawip, unsigned short port) :
		m_ep(boost::asio::ip::address::from_string(rawip), port),
		m_ssl_context(boost::asio::ssl::context::sslv3_client),
		m_ssl_stream(m_ios, m_ssl_context)
	{
		m_ssl_stream.set_verify_mode(boost::asio::ssl::verify_peer);
		m_ssl_stream.set_verify_callback([this](bool preverified, boost::asio::ssl::verify_context& context) ->bool {
			return on_peer_verify(preverified, context);
			});
	}

	static void statrt_client()
	{
		const std::string rawip = "127.0.0.1";
		int port = 3333;
		try {
			SyncSSLClient client(rawip, port);
			client.connect();
			std::cout << "sending request to server...\n";
			std::string response = client.emulate_long_computation_op(10);
			std::cout << "response received: " << response << std::endl;
			client.close();
		}
		catch (const std::exception& ex) {
			std::cerr << ex.what() << std::endl;
		}
	}

	void connect()
	{
		m_ssl_stream.lowest_layer().connect(m_ep);
		m_ssl_stream.handshake(boost::asio::ssl::stream_base::client);
	}

	void close()
	{
		boost::system::error_code ec;
		m_ssl_stream.shutdown(ec);
		m_ssl_stream.lowest_layer().shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
		m_ssl_stream.lowest_layer().close();
	}

	std::string emulate_long_computation_op(int duration_sec)
	{
		std::string request = "EMULATE_LONG_COMP_OP " + std::to_string(duration_sec) + "\n";
		send_request(request);
		return receive_response();
	}

private:
	bool on_peer_verify(bool preverified, boost::asio::ssl::verify_context& context)
	{
		return true;
	}

	void send_request(const std::string& request)
	{
		boost::asio::write(m_ssl_stream, boost::asio::buffer(request));
	}

	std::string receive_response()
	{
		boost::asio::streambuf buf;
		boost::asio::read_until(m_ssl_stream, buf, '\n');
		std::string response;
		std::istream input(&buf);
		std::getline(input, response);
		return response;
	}
};