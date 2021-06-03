#include <iostream>
#include <boost/asio.hpp>
#include <memory>
#include "sync_client.hpp"
#include "sync_server.hpp"

using namespace boost;

template <class F>
struct y_combinator {
	F f; // the lambda will be stored here

	// a forwarding operator():
	template <class... Args>
	decltype(auto) operator()(Args&&... args) const {
		// we pass ourselves to f, then the arguments.
		// the lambda should take the first argument as `auto&& recurse` or similar.
		return f(*this, std::forward<Args>(args)...);
	}
};
// helper function that deduces the type of the lambda:
template <class F>
y_combinator<std::decay_t<F>> make_y_combinator(F&& f) {
	return { std::forward<F>(f) };
}

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

struct Session
{
	std::shared_ptr<asio::ip::tcp::socket> sock;
	std::string buf;
	std::size_t total_bytes_written;
};

void callback(const boost::system::error_code& ec, std::size_t bytes_transferred, std::shared_ptr<Session> se)
{
	if (ec) {
		std::cout << ec.message() << std::endl;
		return;
	}
	se->total_bytes_written += bytes_transferred;
	if (se->total_bytes_written == se->buf.length())
		return;

	se->sock->async_write_some(
		asio::buffer(se->buf.c_str() + se->total_bytes_written, se->buf.length() - se->total_bytes_written),
		std::bind(callback, std::placeholders::_1, std::placeholders::_2, se)
	);
}

void writeAsyncToSocket(std::shared_ptr<asio::ip::tcp::socket> sock)
{
	auto se = std::make_shared<Session>();
	se->sock = sock;
	se->buf = "hello";
	se->total_bytes_written = 0;
	se->sock->async_write_some(asio::buffer(std::as_const(se->buf)),
		std::bind(callback, std::placeholders::_1, std::placeholders::_2, se)
	);
}

void writeAsyncToSocket2(std::shared_ptr<asio::ip::tcp::socket> sock)
{
	auto se = std::make_shared<Session>();
	se->sock = sock;
	se->buf = "hello";
	se->total_bytes_written = 0;
	sock->async_send(asio::buffer(std::as_const(se->buf)),
		[se](const boost::system::error_code& ec, std::size_t bytes_transferred) {
			if (ec) {
				std::cout << ec.message() << std::endl;
				return;
			}
		});
}

void writeAsyncToSocket3(std::shared_ptr<asio::ip::tcp::socket> sock)
{
	auto se = std::make_shared<Session>();
	se->sock = sock;
	se->buf = "hello";
	se->total_bytes_written = 0;
	asio::async_write(*sock, 
		asio::buffer(std::as_const(se->buf)),
		[](auto const& ec, auto const& bytes) {
			return;
		});
}

void writing_to_a_tcp_socket_async()
{
	std::string rawip = "127.0.0.1";
	unsigned short port = 3333;
	try {
		asio::ip::tcp::endpoint ep(asio::ip::address::from_string(rawip), port);
		asio::io_service ios;
		//step 1
		auto sock = std::make_shared<asio::ip::tcp::socket>(ios, ep.protocol());
		sock->connect(ep);
		writeAsyncToSocket(sock);
		ios.run();
	}
	catch (std::exception const& ex) {
		std::cout << ex.what() << std::endl;
	}
}

void test()
{
	auto fac = make_y_combinator([](auto&& recurse, int n) -> int{
		if (n <= 1) return 1;
		return n * recurse(n - 1);
		});
	std::cout << fac(5) << std::endl;
}

void canceling_asynchronous_opearation()
{
	std::string rawip = "127.0.0.1";
	unsigned short port = 3333;
	try {
		asio::ip::tcp::endpoint ep(asio::ip::address::from_string(rawip), port);
		asio::io_service ios;
		//step 1
		auto sock = std::make_shared<asio::ip::tcp::socket>(ios, ep.protocol());
		sock->async_connect(ep, [](auto const& ec) {
			if (ec == asio::error::operation_aborted) {
				std::cout << "Operation cancelled\n";
				return;
			}
			std::cout << ec.message() << std::endl;
			});
		std::thread worker([&ios]() {
			ios.run();
			});
		std::this_thread::sleep_for(std::chrono::seconds(2));
		sock->cancel();
		worker.join();
	}
	catch (std::exception const& ex) {
		std::cout << ex.what() << std::endl;
	}
}

int main()
{
	//prepairing_buffer_for_output();
	//prepairing_buffer_for_input();
	//writing_to_a_tcp_socket_sync();
	//test();
	//writing_to_a_tcp_socket_async();
	//canceling_asynchronous_opearation();
	
	//sync_client{}.start();
	sync_server{}.start();

	return EXIT_SUCCESS;
}