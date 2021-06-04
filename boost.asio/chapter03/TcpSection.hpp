#pragma once
#include <boost/asio.hpp>
#include <boost/predef.h>

typedef void (*Callback)(unsigned int request_id, const std::string& response, const boost::system::error_code& ec);

struct TcpSection
{
	TcpSection(
		boost::asio::io_service& ios,
		const std::string& rawip,
		unsigned short port,
		const std::string request,
		unsigned int id,
		Callback cb
	) :
		m_sock(ios),
		m_ep(boost::asio::ip::address::from_string(rawip), port),
		m_request(request),
		m_id(id),
		m_callback(cb),
		m_was_cancelled(false)
	{}

	boost::asio::ip::tcp::socket m_sock;
	boost::asio::ip::tcp::endpoint m_ep; // remote endpoint
	std::string m_request;

	boost::asio::streambuf m_res_buf;
	std::string m_res;

	boost::system::error_code m_ec;

	unsigned int m_id;
	
	Callback m_callback;

	bool m_was_cancelled;
	std::mutex m_cancel_guard;
};