#include <boost/asio.hpp>
#include <iostream>
#include <memory>
#include <utility>
#include <vector>
#include <ctime>

using boost::asio::ip::tcp;

class shared_const_buffer
{
public:
	explicit shared_const_buffer(const std::string& data) :
		data_(new std::vector<char>(data.begin(), data.end())),
		buffer_(boost::asio::buffer(*data_))
	{}
	using value_type = boost::asio::const_buffer;
	using const_iterator = const boost::asio::const_buffer*;
	const boost::asio::const_buffer* begin() const { return &buffer_; }
	const boost::asio::const_buffer* end() const { return &buffer_ + 1; }

private:
	std::shared_ptr<std::vector<char> > data_;
	boost::asio::const_buffer buffer_;
};

class session : public std::enable_shared_from_this<session>
{
private:
	// The socket used to communicate with the client.
	tcp::socket socket_;

public:
	session(tcp::socket socket)
		: socket_(std::move(socket))
	{}

	void start()
	{
		do_write();
	}

private:
	void do_write()
	{
		std::time_t now = std::time(0);
		shared_const_buffer buffer(std::ctime(&now));

		auto self(shared_from_this());
		boost::asio::async_write(socket_, buffer,
			[self](boost::system::error_code /*ec*/, std::size_t /*length*/)
			{
			});
	}
};

class server
{
public:
	server(boost::asio::io_context& io_context, short port)
		: acceptor_(io_context, tcp::endpoint(tcp::v4(), port))
	{
		do_accept();
	}

private:
	void do_accept()
	{
		acceptor_.async_accept(
			[this](boost::system::error_code ec, tcp::socket socket)
			{
				if (!ec)
				{
					std::make_shared<session>(std::move(socket))->start();
				}

				do_accept();
			});
	}

	tcp::acceptor acceptor_;
};

int main()
{
	try {
		boost::asio::io_context io_context;
		server s(io_context, 3333);
		io_context.run();
	}
	catch (const std::exception& ex) {
		std::cerr << ex.what() << std::endl;
	}
	return EXIT_SUCCESS;
}