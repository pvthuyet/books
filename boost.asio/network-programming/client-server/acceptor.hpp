#pragma once
#include <boost/asio.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <atomic>
#include "define.hpp"

SG_BEGIN
class hive;
class Connection;
class Acceptor : public boost::enable_shared_from_this<Acceptor>
{
	friend class hive;
private:
	boost::shared_ptr<hive>				m_hive;
	boost::asio::ip::tcp::acceptor		m_acceptor;
	boost::asio::io_service::strand		m_strand;
	boost::asio::deadline_timer			m_timer;
	boost::posix_time::ptime			m_last_time;
	boost::int32_t						m_timer_interval;
	std::atomic_int						m_error_state;

private:
	void StartTimer();
	void StartError(const boost::system::error_code& error);
	void DispatchAccept(boost::shared_ptr<Connection> connection);
	void HandleTimer(const boost::system::error_code& error);
	void HandleAccept(const boost::system::error_code& error, boost::shared_ptr<Connection> connection);

public:
	Acceptor(boost::shared_ptr<hive> hiv);
	virtual ~Acceptor() noexcept;

private:
	// Called when a connection has connected to the server. This function 
	// should return true to invoke the connection's OnAccept function if the 
	// connection will be kept. If the connection will not be kept, the 
	// connection's Disconnect function should be called and the function 
	// should return false.
	virtual bool OnAccept(boost::shared_ptr<Connection> connection, const std::string& host, boost::uint16_t port) = 0;

	// Called on each timer event.
	virtual void OnTimer(const boost::posix_time::time_duration& delta) = 0;

	// Called when an error is encountered. Most typically, this is when the
	// Acceptor is being closed via the Stop function or if the Listen is 
	// called on an address that is not available.
	virtual void OnError(const boost::system::error_code& error) = 0;

private:
	// Returns the Hive object.
	boost::shared_ptr<hive> GetHive();

	// Returns the Acceptor object.
	boost::asio::ip::tcp::acceptor& GetAcceptor();

	// Sets the timer interval of the object. The interval is changed after 
	// the next update is called. The default value is 1000 ms.
	void SetTimerInterval(boost::int32_t timer_interval_ms);

	// Returns the timer interval of the object.
	boost::int32_t GetTimerInterval() const;

	// Returns true if this object has an error associated with it.
	bool HasError();

public:
	// Begin listening on the specific network interface.
	void Listen(const std::string& host, const boost::uint16_t& port);

	// Posts the connection to the listening interface. The next client that
	// connections will be given this connection. If multiple calls to Accept
	// are called at a time, then they are accepted in a FIFO order.
	void Accept(boost::shared_ptr<Connection> connection);

	// Stop the Acceptor from listening.
	void Stop();
};
SG_END