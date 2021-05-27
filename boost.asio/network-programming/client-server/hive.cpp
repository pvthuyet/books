#include "hive.hpp"
#include <boost/make_shared.hpp>

SG_BEGIN
hive::hive() :
	m_shutdown{false},
	m_ios{},
	m_worker(boost::make_shared<boost::asio::io_service::work>(m_ios))
{}

hive::~hive() noexcept = default;

auto& hive::get_service()
{
	return m_ios;
}

bool hive::is_stopped() const
{
	return m_shutdown.load();
}

auto hive::poll()
{
	return m_ios.poll();
}

auto hive::run()
{
	return m_ios.run();
}

void hive::stop()
{
	auto old = m_shutdown.load();
	if (!old && m_shutdown.compare_exchange_strong(old, true)) {
		m_worker.reset();
		m_ios.run();
		m_ios.stop();
	}
}

void hive::reset()
{
	auto old = m_shutdown.load();
	if (m_shutdown.compare_exchange_strong(old, false)) {
		m_ios.reset();
		m_worker.reset(new boost::asio::io_service::work(m_ios));
	}
}

SG_END
