#pragma once
#include "TcpSection.hpp"
#include <boost/noncopyable.hpp>

class AsyncTcpClient : public boost::noncopyable
{
public:
	static void start();
	AsyncTcpClient();
	void emulateLongConputationOp(
		unsigned int duration_sec,
		const std::string& rawip,
		unsigned short port,
		Callback cb,
		unsigned int req_id
	);

	void cancelRequest(unsigned int req_id);
	void close();

private:
	void onRequestComplete(std::shared_ptr<TcpSection> session);

private:
	boost::asio::io_service m_ios;
	std::map<int, std::shared_ptr<TcpSection>> m_active_sessions;
	std::mutex m_active_session_guard;
	std::unique_ptr<boost::asio::io_service::work> m_work;
	std::unique_ptr<std::thread> m_thread;
};