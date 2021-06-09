#include <cstdlib>
#include <iostream>
#include <memory>
#include <utility>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

class session : public std::enable_shared_from_this<session>
{
private:
	tcp::socket socket_;
	enum { max_length = 1024 };
	char data_[max_length];

public:
	session(tcp::socket socket)
		: socket_(std::move(socket))
	{
	}

	void start()
	{
		do_read();
	}

private:
	void do_read()
	{
		auto self(shared_from_this());
		socket_.async_read_some(boost::asio::buffer(data_, max_length),
			[this, self](boost::system::error_code ec, std::size_t length)
			{
				if (!ec)
				{
					std::cout << std::this_thread::get_id() << " " << socket_.local_endpoint().address() << ": ";
					std::cout.write(data_, length);
					std::cout << std::endl;
					do_write(length);
				}
			});
	}

	void do_write(std::size_t length)
	{
		auto self(shared_from_this());
		boost::asio::async_write(socket_, boost::asio::buffer(data_, length),
			[this, self](boost::system::error_code ec, std::size_t /*length*/)
			{
				if (!ec)
				{
					do_read();
				}
			});
	}
};

class server
{
private:
	tcp::acceptor acceptor_;

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
};

int main(int argc, char* argv[])
{
	try
	{
		boost::asio::io_context io_context;

		server s(io_context, 3333);

		io_context.run();
	}
	catch (std::exception& e)
	{
		std::cerr << "Exception: " << e.what() << "\n";
	}

	return 0;
}