#pragma once

#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <algorithm>

namespace asio = boost::asio;
void synchronous_client()
{
	using namespace std::string_literals;
	boost::mutex mux;
	auto print = [&mux](std::string msg) {
		boost::lock_guard<boost::mutex> lock(mux);
		std::cout << msg << std::endl;
	};

	auto job = [print](auto ios, int id) {
		print("Thread "s + std::to_string(id) + " start"s);
		try {
			while (true) {
				boost::system::error_code ec;
				ios->run(ec);
				if (ec) {
					print(ec.message());
				}
				break;
			}
		}
		catch (std::exception const& e) {
			print(e.what());
		}
		print("Thread "s + std::to_string(id) + " end"s);
	};

	auto ios = boost::make_shared<asio::io_service>();
	auto worker = boost::make_shared<asio::io_service::work>(*ios);
	auto srd = boost::make_shared<asio::io_service::strand>(*ios);
	print("Press Enter to exit.");

	boost::thread_group thrds;
	for (int i = 0; i < 3; ++i) {
		thrds.create_thread(std::bind(job, ios, i));
	}

	asio::ip::tcp::socket socket(*ios);
	try {
		asio::ip::tcp::resolver resolver(*ios);
		asio::ip::tcp::resolver::query query("www.packtpub.com", boost::lexical_cast<std::string>(80));
		auto it = resolver.resolve(query);
		auto edp = *it;
		print("Connecting to " + edp.host_name());
		socket.connect(edp);
		print("Connected");
	}
	catch (std::exception const& e) {
		print(e.what());
	}

	std::cin.get();

	boost::system::error_code ec;
	socket.shutdown(asio::ip::tcp::socket::shutdown_both, ec);
	socket.close(ec);

	ios->stop();
	thrds.join_all();
}

void onconnect(boost::system::error_code const& ec, std::function<void(std::string)> print) 
{
	if (ec) print("On connect error" + ec.message());
	else print("Connected");
};

void asynchronous_client()
{
	using namespace std::string_literals;
	boost::mutex mux;
	auto print = [&mux](std::string msg) {
		boost::lock_guard<boost::mutex> lock(mux);
		std::cout << msg << std::endl;
	};

	auto job = [print](auto ios, int id) {
		print("Thread "s + std::to_string(id) + " start"s);
		try {
			while (true) {
				boost::system::error_code ec;
				ios->run(ec);
				if (ec) {
					print(ec.message());
				}
				break;
			}
		}
		catch (std::exception const& e) {
			print(e.what());
		}
		print("Thread "s + std::to_string(id) + " end"s);
	};

	auto ios = boost::make_shared<asio::io_service>();
	auto worker = boost::make_shared<asio::io_service::work>(*ios);
	auto srd = boost::make_shared<asio::io_service::strand>(*ios);
	print("Press Enter to exit.");

	boost::thread_group thrds;
	for (int i = 0; i < 3; ++i) {
		thrds.create_thread(std::bind(job, ios, i));
	}

	auto socket = boost::make_shared<asio::ip::tcp::socket>(*ios);
	try {
		asio::ip::tcp::resolver resolver(*ios);
		asio::ip::tcp::resolver::query query("www.packtpub.com", boost::lexical_cast<std::string>(80));
		auto it = resolver.resolve(query);
		auto ep = *it;
		print("Connecting to " + ep.host_name());
		socket->async_connect(ep, boost::bind(onconnect, _1, print));
	}
	catch (std::exception const& e) {
		print(e.what());
	}

	std::cin.get();

	boost::system::error_code ec;
	socket->shutdown(asio::ip::tcp::socket::shutdown_both, ec);
	socket->close(ec);

	ios->stop();
	thrds.join_all();
}


void on_accept(boost::system::error_code const& ec, std::function<void(std::string)> print)
{
	if (ec) print("OnAccept Error: " + ec.message());
	else print("Accepted!");
}

void asynchronous_server()
{
	using namespace std::string_literals;
	boost::mutex mux;
	auto print = [&mux](std::string msg) {
		boost::lock_guard<boost::mutex> lock(mux);
		std::cout << msg << std::endl;
	};

	auto job = [print](auto ios, int id) {
		print("Thread "s + std::to_string(id) + " start"s);
		try {
			while (true) {
				boost::system::error_code ec;
				ios->run(ec);
				if (ec) {
					print(ec.message());
				}
				break;
			}
		}
		catch (std::exception const& e) {
			print(e.what());
		}
		print("Thread "s + std::to_string(id) + " end"s);
	};

	auto ios = boost::make_shared<asio::io_service>();
	auto worker = boost::make_shared<asio::io_service::work>(*ios);
	auto srd = boost::make_shared<asio::io_service::strand>(*ios);
	print("Press Enter to exit.");

	boost::thread_group thrds;
	for (int i = 0; i < 3; ++i) {
		thrds.create_thread(std::bind(job, ios, i));
	}

	auto acceptor = boost::make_shared<asio::ip::tcp::acceptor>(*ios);
	auto socket = boost::make_shared<asio::ip::tcp::socket>(*ios);
	try {
		asio::ip::tcp::resolver resolver(*ios);
		asio::ip::tcp::resolver::query query("127.0.0.1", boost::lexical_cast<std::string>(4444));
		asio::ip::tcp::endpoint ep = *resolver.resolve(query);
		acceptor->open(ep.protocol());
		acceptor->set_option(asio::ip::tcp::acceptor::reuse_address(false));
		acceptor->bind(ep);
		acceptor->listen(asio::socket_base::max_connections);
		acceptor->async_accept(*socket, boost::bind(on_accept, _1, print));
	}
	catch (std::exception const& e) {
		print(e.what());
	}

	std::cin.get();

	boost::system::error_code ec;
	acceptor->close(ec);

	socket->shutdown(asio::ip::tcp::socket::shutdown_both, ec);
	socket->close(ec);

	ios->stop();
	thrds.join_all();
}

using namespace std::string_literals;
boost::mutex gMux;

template<typename First, typename... Args>
void gbprint(First first, const Args&... args) 
{
	boost::lock_guard<boost::mutex> lock(gMux);
	std::cout << first;
	auto outSpace = [](auto const& arg) {
		std::cout << ' ' << arg;
	};
	(..., outSpace(args));
	//std::cout << '\n';
};

void WorkerThread(boost::shared_ptr<asio::io_service> ios, int id) 
{
	gbprint("Thread"s, id, "start"s);	
	try {
		while (true) {
			boost::system::error_code ec;
			ios->run(ec);
			if (ec) {
				gbprint(ec);
			}
			break;
		}
	}
	catch (std::exception const& e) {
		gbprint(e.what());
	}
	gbprint("Thread"s, id, "end"s);
};

struct ClientContext : public boost::enable_shared_from_this<ClientContext>
{
	asio::ip::tcp::socket mSocket;
	size_t mRecvBufferIndex;
	std::vector<boost::uint8_t> mRecvBuffer;	
	std::list<std::vector<boost::uint8_t>> mSendBuffer;

	ClientContext(asio::io_service& ios) : 
		mSocket(ios),
		mRecvBufferIndex(0),
		mRecvBuffer(4096)
	{}

	void Close()
	{
		boost::system::error_code ec;
		mSocket.shutdown(asio::ip::tcp::socket::shutdown_both, ec);
		mSocket.close(ec);
	}

	void OnSend(boost::system::error_code const& ec,
		std::list<std::vector<boost::uint8_t>>::iterator it)
	{
		if (ec) {
			gbprint("OnSend Error: "s, ec, "\n");
			Close();
		}
		else {
			gbprint("Send "s, (*it).size(), " bytes.", "\n");
		}
		mSendBuffer.erase(it);

		// start next pending send
		if (!mSendBuffer.empty()) {
			asio::async_write(mSocket,
				asio::buffer(mSendBuffer.front()),
				boost::bind(&ClientContext::OnSend, 
					shared_from_this(), 
					asio::placeholders::error,
					mSendBuffer.begin())
			);
		}
	}

	void send(const void* buffer, size_t len)
	{
		std::vector<boost::uint8_t> output;
		std::copy(static_cast<const uint8_t*>(buffer),
			static_cast<const uint8_t*>(buffer) + len,
			std::back_inserter(output));

		bool cansendnow = mSendBuffer.empty();

		mSendBuffer.push_back(output);
		if (cansendnow) {
			asio::async_write(
				mSocket,
				asio::buffer(mSendBuffer.front()),
				boost::bind(
					&ClientContext::OnSend,
					shared_from_this(),
					asio::placeholders::error,
					mSendBuffer.begin()
				)
			);
		}
	}

	void OnRecv(const boost::system::error_code& ec, size_t bytes)
	{
		if (ec) {
			gbprint("OnRecv Error:", ec, "\n");
			Close();
		}
		else {
			mRecvBufferIndex += bytes;
			gbprint("Recv", bytes, "bytes\n");

			// dump data
			for (int i = 0; i < mRecvBufferIndex; ++i) {
				gbprint(mRecvBuffer[i], ' ');
				if ((i + 1) % 16 == 0)
					gbprint("\n");
			}
		}
		gbprint(std::dec);

		mRecvBufferIndex = 0;
		Recv();
	}

	void Recv()
	{
		mSocket.async_read_some(
			asio::buffer(
				&mRecvBuffer[mRecvBufferIndex],
				mRecvBuffer.size() - mRecvBufferIndex),
			boost::bind(&ClientContext::OnRecv, shared_from_this(), _1, _2)
		);
	}
};

void OnAccept(const boost::system::error_code& ec, boost::shared_ptr<ClientContext> clnt)
{
	if (ec) {
		gbprint("OnAccept Error:", ec, "\n");
	}
	else {
		gbprint("Accepted!\n");
	}
	clnt->send("Hi there!", 9);
	clnt->Recv();
}

void reading_writing_socket()
{
	auto ios = boost::make_shared<asio::io_service>();
	auto worker = boost::make_shared<asio::io_service::work>(*ios);
	auto srd = boost::make_shared<asio::io_service::strand>(*ios);
	gbprint("Press Enter to exit!\n");
	boost::thread_group ths;
	ths.create_thread(boost::bind(&WorkerThread, ios, 1));

	auto acceptor = boost::make_shared<asio::ip::tcp::acceptor>(*ios);
	auto client = boost::make_shared<ClientContext>(*ios);
	try {
		asio::ip::tcp::resolver resol(*ios);
		asio::ip::tcp::resolver::query query("127.0.0.1"s, boost::lexical_cast<std::string>(4444));
		asio::ip::tcp::endpoint ep = *resol.resolve(query);
		acceptor->open(ep.protocol());
		acceptor->set_option(asio::ip::tcp::acceptor::reuse_address(false));
		acceptor->bind(ep);
		acceptor->listen(asio::socket_base::max_connections);
		acceptor->async_accept(client->mSocket, boost::bind(OnAccept, _1, client));
		gbprint("Listening on:", ep, "\n");
	}
	catch (std::exception const& e) {
		gbprint(e.what());
	}

	std::cin.get();
	boost::system::error_code ec;
	acceptor->close(ec);
	ios->stop();
	ths.join_all();
}