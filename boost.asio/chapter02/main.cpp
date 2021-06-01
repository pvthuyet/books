#include <iostream>
#include <boost/asio.hpp>
#include <memory>

using namespace boost;
void prepairing_buffer_for_output()
{
	constexpr std::string_view buf = "hello";
	auto outbuf = asio::buffer(buf);
	std::cout << typeid(outbuf).name() << std::endl;
}

void prepairing_buffer_for_input()
{
	constexpr size_t BUF_SIZE_BYTES = 20;
	auto buf = std::make_unique<char[]>(BUF_SIZE_BYTES);
	auto inbuf = asio::buffer(buf.get(), BUF_SIZE_BYTES);
	std::cout << typeid(inbuf).name() << std::endl;
}

void writeToSocket(asio::ip::tcp::socket& sock)
{
	constexpr std::string_view buf = "hello";
	std::size_t total_bytes_written = 0;
	while (total_bytes_written != buf.length()) {
		total_bytes_written += sock.write_some(asio::buffer(buf.data() + total_bytes_written, buf.length() - total_bytes_written));
	}
}

void writeToSocket2(asio::ip::tcp::socket& sock)
{
	constexpr std::string_view buf = "hello";
	sock.send(asio::buffer(buf));
}

void writeToSocket3(asio::ip::tcp::socket& sock)
{
	constexpr std::string_view buf = "hello";
	asio::write(sock, asio::buffer(buf));
}

void writing_to_a_tcp_socket_sync()
{
	std::string rawip = "127.0.0.1";
	unsigned short port = 3333;
	try {
		asio::ip::tcp::endpoint ep(asio::ip::address::from_string(rawip), port);
		asio::io_service ios;
		//step 1
		asio::ip::tcp::socket sock(ios, ep.protocol());
		sock.connect(ep);
		writeToSocket(sock);
	}
	catch (std::exception const& ex) {
		std::cout << ex.what() << std::endl;
	}
}

int main()
{
	//prepairing_buffer_for_output();
	//prepairing_buffer_for_input();
	writing_to_a_tcp_socket_sync();
	return EXIT_SUCCESS;
}