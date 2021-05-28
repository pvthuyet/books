#include "acceptor.hpp"
#include "hive.hpp"
#include "connection.hpp"
#include <boost/bind.hpp>

SG_BEGIN
Acceptor::Acceptor(boost::shared_ptr<hive> hiv) :
	m_hive(hiv),
	m_acceptor(hiv->get_service()),
	m_strand(hiv->get_service()),
	m_timer(hiv->get_service()),
	m_last_time{},
	m_timer_interval(1000),
	m_error_state(0)
{}

Acceptor::~Acceptor() noexcept = default;

void Acceptor::StartTimer()
{
	m_last_time = boost::posix_time::microsec_clock::local_time();
	m_timer.expires_from_now(boost::posix_time::milliseconds(m_timer_interval));
	m_timer.async_wait(m_strand.wrap(boost::bind(&Acceptor::HandleTimer, shared_from_this(), _1)));
}

void Acceptor::StartError(const boost::system::error_code& error)
{
	auto old = m_error_state.load();
	if (m_error_state.compare_exchange_strong(old, 1)) {
		boost::system::error_code ec;
		m_acceptor.cancel(ec);
		m_acceptor.close(ec);
		OnError(error);
	}
}

void Acceptor::DispatchAccept(boost::shared_ptr<Connection> connection)
{
	m_acceptor.async_accept(connection->GetSocket(),
		connection->GetStrand().wrap(boost::bind(&Acceptor::HandleAccept, shared_from_this(), _1, connection))
		);
}

void Acceptor::HandleTimer(const boost::system::error_code& error)
{
	if (error || HasError() || m_hive->is_stopped())
		StartError(error);
	else
	{
		OnTimer(boost::posix_time::microsec_clock::local_time() - m_last_time);
		StartTimer();
	}
}

void Acceptor::HandleAccept(const boost::system::error_code& error, boost::shared_ptr<Connection> connection)
{
	if (error || HasError() || m_hive->is_stopped())
		connection->StartError(error);
	else
	{
		if (connection->GetSocket().is_open())
		{
			connection->StartTimer();
			if (OnAccept(connection, connection->GetSocket().remote_endpoint().address().to_string(), connection->GetSocket().remote_endpoint().port()))
			{
				connection->OnAccept(m_acceptor.local_endpoint().address().to_string(), m_acceptor.local_endpoint().port());
			}
		}
		else
			StartError(error);
	}
}

void Acceptor::Stop()
{
	m_strand.post(boost::bind(&Acceptor::HandleTimer, shared_from_this(), boost::asio::error::connection_reset));
}

void Acceptor::Accept(boost::shared_ptr<Connection> connection)
{
	m_strand.post(boost::bind(&Acceptor::DispatchAccept, shared_from_this(), connection));
}

void Acceptor::Listen(const std::string& host, const uint16_t& port)
{
	boost::asio::ip::tcp::resolver resolver(m_hive->get_service());
	boost::asio::ip::tcp::resolver::query query(host, boost::lexical_cast<std::string>(port));
	boost::asio::ip::tcp::endpoint endpoint = *resolver.resolve(query);
	m_acceptor.open(endpoint.protocol());
	m_acceptor.set_option(boost::asio::ip::tcp::acceptor::reuse_address(false));
	m_acceptor.bind(endpoint);
	m_acceptor.listen(boost::asio::socket_base::max_connections);
	StartTimer();
}

// Acceptor::GetHive definition
boost::shared_ptr<hive> Acceptor::GetHive()
{
	return m_hive;
}

// Acceptor::GetAcceptor definition
boost::asio::ip::tcp::acceptor& Acceptor::GetAcceptor()
{
	return m_acceptor;
}

// Acceptor::GetTimerInterval definition
int32_t Acceptor::GetTimerInterval() const
{
	return m_timer_interval;
}

// Acceptor::SetTimerInterval definition
void Acceptor::SetTimerInterval(int32_t timer_interval)
{
	m_timer_interval = timer_interval;
}

// Acceptor::HasError definition
bool Acceptor::HasError()
{
	return m_error_state.load() == 1;
}

SG_END