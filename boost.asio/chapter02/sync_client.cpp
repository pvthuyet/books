#include "sync_client.hpp"
#include <boost/asio.hpp>
#include <iostream>

using namespace boost;
void communicate(asio::ip::tcp::socket& sock)
{
	constexpr char reqest_buf[] = { 0x48, 0x65, 0x0, 0x6c, 0x6c, 0x6f };
	asio::write(sock, asio::buffer(reqest_buf));
	sock.shutdown(asio::socket_base::shutdown_send);
	asio::streambuf sb;
	boost::system::error_code ec;
	asio::read(sock, sb, ec);
	if (ec == asio::error::eof) {
		auto buf = sb.data();
		std::string str(asio::buffers_begin(buf), asio::buffers_begin(buf) + buf.size());
		std::cout << "receive: " << str << std::endl;
	}
	else {
		throw boost::system::system_error(ec);
	}
}

void sync_client::start()
{
	std::string rawip = "127.0.0.1";
	unsigned short port = 3333;
	try {
		asio::ip::tcp::endpoint ep(asio::ip::address::from_string(rawip), port);
		asio::io_service ios;
		asio::ip::tcp::socket sock(ios, ep.protocol());
		sock.connect(ep);
		communicate(sock);
	}
	catch (std::exception const& ex) {
		std::cout << ex.what() << std::endl;
	}	
}