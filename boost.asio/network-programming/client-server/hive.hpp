#pragma once
#include <boost/asio.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>
#include <atomic>
#include "define.hpp"

SG_BEGIN
class hive : public boost::enable_shared_from_this<hive>
{
private:
	std::atomic_bool m_shutdown;
	boost::asio::io_service m_ios;
	boost::shared_ptr<boost::asio::io_service::work> m_worker;

public:
	hive();
	virtual ~hive() noexcept;

	hive(const hive&) = delete;
	hive& operator=(const hive&) = delete;

	boost::asio::io_service& get_service();
	bool is_stopped() const;

	boost::asio::io_service::count_type poll();
	boost::asio::io_service::count_type run();
	void stop();
	void reset();
};
SG_END