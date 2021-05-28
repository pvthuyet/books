#pragma once
#include <boost/asio.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <vector>
#include "define.hpp"

SG_BEGIN
class Acceptor;
class hive;
class Connection : public boost::enable_shared_from_this<Connection>
{
	friend class Acceptor;
	friend class hive;

private:
	boost::shared_ptr<hive>			m_hive;
	boost::asio::ip::tcp::socket	m_socket;
	boost::asio::io_service::strand	m_io_strand;
	boost::asio::deadline_timer		m_timer;
	boost::posix_time::ptime		m_last_time;
	std::vector<boost::uint8_t>		m_recv_buffer;
	std::list<boost::int32_t>		m_pending_recvs;
	std::list<std::vector<boost::uint8_t> > m_pending_sends;
	boost::int32_t					m_receive_buffer_size;
	boost::int32_t					m_timer_interval;
	volatile boost::uint32_t		m_error_state;

protected:
	Connection(boost::shared_ptr<hive> hive);
	virtual ~Connection();

private:
	Connection(const Connection& rhs);
	Connection& operator =(const Connection& rhs);
	void StartSend();
	void StartRecv(boost::int32_t total_bytes);
	void StartTimer();
	void StartError(const boost::system::error_code& error);
	void DispatchSend(std::vector<boost::uint8_t> buffer);
	void DispatchRecv(boost::int32_t total_bytes);
	void DispatchTimer(const boost::system::error_code& error);
	void HandleConnect(const boost::system::error_code& error);
	void HandleSend(const boost::system::error_code& error, std::list<std::vector<boost::uint8_t> >::iterator itr);
	void HandleRecv(const boost::system::error_code& error, boost::int32_t actual_bytes);
	void HandleTimer(const boost::system::error_code& error);

private:
	// Called when the connection has successfully connected to the local
	// host.
	virtual void OnAccept(const std::string& host, boost::uint16_t port) = 0;

	// Called when the connection has successfully connected to the remote
	// host.
	virtual void OnConnect(const std::string& host, boost::uint16_t port) = 0;

	// Called when data has been sent by the connection.
	virtual void OnSend(const std::vector<boost::uint8_t>& buffer) = 0;

	// Called when data has been received by the connection. 
	virtual void OnRecv(std::vector<boost::uint8_t>& buffer) = 0;

	// Called on each timer event.
	virtual void OnTimer(const boost::posix_time::time_duration& delta) = 0;

	// Called when an error is encountered.
	virtual void OnError(const boost::system::error_code& error) = 0;

public:
	// Returns the hive object.
	boost::shared_ptr<hive> GetHive();

	// Returns the socket object.
	boost::asio::ip::tcp::socket& GetSocket();

	// Returns the strand object.
	boost::asio::io_service::strand& GetStrand();

	// Sets the application specific receive buffer size used. For stream 
	// based protocols such as HTTP, you want this to be pretty large, like 
	// 64kb. For packet based protocols, then it will be much smaller, 
	// usually 512b - 8kb depending on the protocol. The default value is
	// 4kb.
	void SetReceiveBufferSize(boost::int32_t size);

	// Returns the size of the receive buffer size of the current object.
	boost::int32_t GetReceiveBufferSize() const;

	// Sets the timer interval of the object. The interval is changed after 
	// the next update is called.
	void SetTimerInterval(boost::int32_t timer_interval_ms);

	// Returns the timer interval of the object.
	boost::int32_t GetTimerInterval() const;

	// Returns true if this object has an error associated with it.
	bool HasError();

	// Binds the socket to the specified interface.
	void Bind(const std::string& ip, boost::uint16_t port);

	// Starts an a/synchronous connect.
	void Connect(const std::string& host, boost::uint16_t port);

	// Posts data to be sent to the connection.
	void Send(const std::vector<boost::uint8_t>& buffer);

	// Posts a recv for the connection to process. If total_bytes is 0, then 
	// as many bytes as possible up to GetReceiveBufferSize() will be 
	// waited for. If Recv is not 0, then the connection will wait for exactly
	// total_bytes before invoking OnRecv.
	void Recv(boost::int32_t total_bytes = 0);

	// Posts an asynchronous disconnect event for the object to process.
	void Disconnect();
};
SG_END