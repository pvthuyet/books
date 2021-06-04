#include "AsyncTcpClient.hpp"
#include "TcpSection.hpp"
#include <iostream>

using namespace boost;
void handler(unsigned int req_id, const std::string& response, const boost::system::error_code& ec)
{
	if (ec.value() == 0) {
		std::cout << "Request #" << req_id
			<< " has completed. Response: " << response << std::endl;
	}
	else if (ec == asio::error::operation_aborted) {
		std::cout << "Request #" << req_id
			<< " has canceled by user." << std::endl;
	}
	else {
		std::cout << "Request #" << req_id
			<< " failed! Error code = " << ec.value()
			<< ". Error message = " << ec.message() << std::endl;
	}
}

void AsyncTcpClient::start()
{
	try {
		AsyncTcpClient client;
		client.emulateLongConputationOp(10, "127.0.0.1", 3333, handler, 1);
		std::this_thread::sleep_for(std::chrono::seconds(5));

		client.emulateLongConputationOp(10, "127.0.0.1", 3334, handler, 2);
		client.cancelRequest(1);
		std::this_thread::sleep_for(std::chrono::seconds(6));

		client.emulateLongConputationOp(10, "127.0.0.1", 3335, handler, 3);
		std::this_thread::sleep_for(std::chrono::seconds(15));
		client.close();
	}
	catch (std::exception const& ex) {
		std::cout << ex.what() << std::endl;
	}
}

AsyncTcpClient::AsyncTcpClient()
{
	m_work = std::make_unique<boost::asio::io_service::work>(m_ios);
	m_thread = std::make_unique<std::thread>([this]() {
		this->m_ios.run();
		});
}

void AsyncTcpClient::emulateLongConputationOp(
	unsigned int duration_sec,
	const std::string& rawip,
	unsigned short port,
	Callback cb,
	unsigned int req_id)
{
	std::string request = "EMULATE_LONG_CALC_OP " + std::to_string(duration_sec) + "\n";
	auto ses = std::make_shared<TcpSection>(m_ios, rawip, port, request, req_id, cb);
	ses->m_sock.open(ses->m_ep.protocol());
	{
		std::lock_guard<std::mutex> lock(m_active_session_guard);
		m_active_sessions[req_id] = ses;
	}
	ses->m_sock.async_connect(ses->m_ep, [this, ses](const boost::system::error_code& ec) {
		if (ec.value() != 0) {
			ses->m_ec = ec;
			onRequestComplete(ses);
			return;
		}

		{
			std::lock_guard<std::mutex> lock(ses->m_cancel_guard);
			if (ses->m_was_cancelled) {
				onRequestComplete(ses);
				return;
			}


			asio::async_write(ses->m_sock,
				asio::buffer(ses->m_request), [this, ses](const boost::system::error_code& ec, std::size_t bytes) {
					if (ec.value() != 0) {
						ses->m_ec = ec;
						onRequestComplete(ses);
						return;
					}
					{
						std::lock_guard<std::mutex> lock(ses->m_cancel_guard);
						if (ses->m_was_cancelled) {
							onRequestComplete(ses);
							return;
						}

						asio::async_read_until(
							ses->m_sock,
							ses->m_res_buf,
							'\n',
							[this, ses](const boost::system::error_code& ec, std::size_t bytes_transferred) {
								if (ec.value() != 0) {
									ses->m_ec = ec;
								}
								else {
									std::istream strm(&ses->m_res_buf);
									std::getline(strm, ses->m_res);
								}
								onRequestComplete(ses);
							});
					}
				});
		}
	});
}

void AsyncTcpClient::cancelRequest(unsigned int req_id)
{
	std::lock_guard<std::mutex> lock(m_active_session_guard);
	auto it = m_active_sessions.find(req_id);
	if (it != m_active_sessions.end()) {
		std::lock_guard<std::mutex> cancelLock(it->second->m_cancel_guard);
		it->second->m_was_cancelled = true;
		it->second->m_sock.cancel();
	}
}

void AsyncTcpClient::close()
{
	m_work.reset(nullptr);
	m_thread->join();
}

void AsyncTcpClient::onRequestComplete(std::shared_ptr<TcpSection> session)
{
	boost::system::error_code ignore_ec;
	session->m_sock.shutdown(asio::ip::tcp::socket::shutdown_both, ignore_ec);
	{
		std::lock_guard<std::mutex> lock(m_active_session_guard);
		auto it = m_active_sessions.find(session->m_id);
		if (it != m_active_sessions.end())
			m_active_sessions.erase(it);
	}

	boost::system::error_code ec;
	if (session->m_ec.value() == 0 && session->m_was_cancelled) {
		ec = asio::error::operation_aborted;
	}
	else {
		ec = session->m_ec;
	}

	session->m_callback(session->m_id, session->m_res, ec);
}