#include "sync_server.hpp"
#include "boost/asio.hpp"
#include <iostream>

using namespace boost;
void processRequest(asio::ip::tcp::socket& sock)
{
	asio::streambuf buf;
	system::error_code ec;
	asio::read(sock, buf, ec);
	if (ec != asio::error::eof) {
		throw system::system_error(ec);
	}
	else {
		auto revbuf = buf.data();
		std::string str(asio::buffers_begin(revbuf), asio::buffers_begin(revbuf) + revbuf.size());
		std::cout << "receive: " << str << std::endl;
	}
	constexpr char response_buf[] = { 0x48, 0x69, 0x21 };
	asio::write(sock, asio::buffer(response_buf));
	sock.shutdown(asio::socket_base::shutdown_send);
}

void sync_server::start()
{
	unsigned short port = 3333;
	try {
		asio::ip::tcp::endpoint ep(asio::ip::address_v4::any(), port);
		asio::io_service ios;
		asio::ip::tcp::acceptor acp(ios, ep);
		asio::ip::tcp::socket sock(ios);
		acp.accept(sock);
		processRequest(sock);
	}
	catch (std::exception const& ex) {
		std::cout << ex.what() << std::endl;
	}
}