#include "connection.hpp"
#include "hive.hpp"
#include "acceptor.hpp"
#include <boost/bind.hpp>
#include <boost/interprocess/detail/atomic.hpp>

SG_BEGIN
// Connection constructor
Connection::Connection(boost::shared_ptr<hive> hive)
	: m_hive(hive), m_socket(hive->get_service()), m_io_strand(hive->get_service()), m_timer(hive->get_service()), m_receive_buffer_size(4096), m_timer_interval(1000), m_error_state(0)
{
}

// Connection destructor
Connection::~Connection()
{
}

// Connection::Bind definition
void Connection::Bind(const std::string& ip, uint16_t port)
{
	boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::address::from_string(ip), port);
	m_socket.open(endpoint.protocol());
	m_socket.set_option(boost::asio::ip::tcp::acceptor::reuse_address(false));
	m_socket.bind(endpoint);
}

// Connection::StartSend definition
void Connection::StartSend()
{
	if (!m_pending_sends.empty())
	{
		boost::asio::async_write(m_socket, boost::asio::buffer(m_pending_sends.front()), m_io_strand.wrap(boost::bind(&Connection::HandleSend, shared_from_this(), boost::asio::placeholders::error, m_pending_sends.begin())));
	}
}

// Connection::StartRecv definition
void Connection::StartRecv(int32_t total_bytes)
{
	if (total_bytes > 0)
	{
		m_recv_buffer.resize(total_bytes);
		boost::asio::async_read(m_socket, boost::asio::buffer(m_recv_buffer), m_io_strand.wrap(boost::bind(&Connection::HandleRecv, shared_from_this(), _1, _2)));
	}
	else
	{
		m_recv_buffer.resize(m_receive_buffer_size);
		m_socket.async_read_some(boost::asio::buffer(m_recv_buffer), m_io_strand.wrap(boost::bind(&Connection::HandleRecv, shared_from_this(), _1, _2)));
	}
}

// Connection::StartTimer definition
void Connection::StartTimer()
{
	m_last_time = boost::posix_time::microsec_clock::local_time();
	m_timer.expires_from_now(boost::posix_time::milliseconds(m_timer_interval));
	m_timer.async_wait(m_io_strand.wrap(boost::bind(&Connection::DispatchTimer, shared_from_this(), _1)));
}

// Connection::StartError definition
void Connection::StartError(const boost::system::error_code& error)
{
	if (boost::interprocess::ipcdetail::atomic_cas32(&m_error_state, 1, 0) == 0)
	{
		boost::system::error_code ec;
		m_socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
		m_socket.close(ec);
		m_timer.cancel(ec);
		OnError(error);
	}
}

// Connection::HandleConnect definition
void Connection::HandleConnect(const boost::system::error_code& error)
{
	if (error || HasError() || m_hive->is_stopped())
		StartError(error);
	else
	{
		if (m_socket.is_open())
			OnConnect(m_socket.remote_endpoint().address().to_string(), m_socket.remote_endpoint().port());
		else
			StartError(error);
	}
}

// Connection::HandleSend definition
void Connection::HandleSend(const boost::system::error_code& error, std::list<std::vector<uint8_t> >::iterator itr)
{
	if (error || HasError() || m_hive->is_stopped())
		StartError(error);
	else
	{
		OnSend(*itr);
		m_pending_sends.erase(itr);
		StartSend();
	}
}

// Connection::HandleRecv definition
void Connection::HandleRecv(const boost::system::error_code& error, int32_t actual_bytes)
{
	if (error || HasError() || m_hive->is_stopped())
		StartError(error);
	else
	{
		m_recv_buffer.resize(actual_bytes);
		OnRecv(m_recv_buffer);
		m_pending_recvs.pop_front();
		if (!m_pending_recvs.empty())
			StartRecv(m_pending_recvs.front());
	}
}

// Connection::HandleTimer definition
void Connection::HandleTimer(const boost::system::error_code& error)
{
	if (error || HasError() || m_hive->is_stopped())
		StartError(error);
	else
	{
		OnTimer(boost::posix_time::microsec_clock::local_time() - m_last_time);
		StartTimer();
	}
}

// Connection::DispatchSend definition
void Connection::DispatchSend(std::vector<uint8_t> buffer)
{
	bool should_start_send = m_pending_sends.empty();
	m_pending_sends.push_back(buffer);
	if (should_start_send)
		StartSend();
}

// Connection::DispatchRecv definition
void Connection::DispatchRecv(int32_t total_bytes)
{
	bool should_start_receive = m_pending_recvs.empty();
	m_pending_recvs.push_back(total_bytes);
	if (should_start_receive)
		StartRecv(total_bytes);
}

// Connection::DispatchTimer definition
void Connection::DispatchTimer(const boost::system::error_code& error)
{
	m_io_strand.post(boost::bind(&Connection::HandleTimer, shared_from_this(), error));
}

// Connection::Connect definition
void Connection::Connect(const std::string& host, uint16_t port)
{
	boost::system::error_code ec;
	boost::asio::ip::tcp::resolver resolver(m_hive->get_service());
	boost::asio::ip::tcp::resolver::query query(host, boost::lexical_cast<std::string>(port));
	boost::asio::ip::tcp::resolver::iterator iterator = resolver.resolve(query);
	m_socket.async_connect(*iterator, m_io_strand.wrap(boost::bind(&Connection::HandleConnect, shared_from_this(), _1)));
	StartTimer();
}

// Connection::Disconnect definition
void Connection::Disconnect()
{
	m_io_strand.post(boost::bind(&Connection::HandleTimer, shared_from_this(), boost::asio::error::connection_reset));
}

// Connection::Recv definition
void Connection::Recv(int32_t total_bytes)
{
	m_io_strand.post(boost::bind(&Connection::DispatchRecv, shared_from_this(), total_bytes));
}

// Connection::Send definition
void Connection::Send(const std::vector<uint8_t>& buffer)
{
	m_io_strand.post(boost::bind(&Connection::DispatchSend, shared_from_this(), buffer));
}

// Connection::GetSocket definition
boost::asio::ip::tcp::socket& Connection::GetSocket()
{
	return m_socket;
}

// Connection::GetStrand definition
boost::asio::io_service::strand& Connection::GetStrand()
{
	return m_io_strand;
}

// Connection::GetHive definition
boost::shared_ptr<hive> Connection::GetHive()
{
	return m_hive;
}

// Connection::SetReceiveBufferSize definition
void Connection::SetReceiveBufferSize(int32_t size)
{
	m_receive_buffer_size = size;
}

// Connection::GetReceiveBufferSize definition
int32_t Connection::GetReceiveBufferSize() const
{
	return m_receive_buffer_size;
}

// Connection::GetTimerInterval definition
int32_t Connection::GetTimerInterval() const
{
	return m_timer_interval;
}

// Connection::SetTimerInterval definition
void Connection::SetTimerInterval(int32_t timer_interval)
{
	m_timer_interval = timer_interval;
}

// Connection::HasError definition
bool Connection::HasError()
{
	return (boost::interprocess::ipcdetail::atomic_cas32(&m_error_state, 1, 1) == 1);
}
SG_END