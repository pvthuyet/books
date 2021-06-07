#pragma once
#include <boost/asio.hpp>
#include <thread>
#include <mutex>
#include <memory>
#include <iostream>

namespace http_errors
{
	enum http_error_codes
	{
		invalid_response = 1
	};

	class http_errors_categogy : public boost::system::error_category
	{
	public:
		const char* name() const noexcept
		{
			return "http_errors";
		}

		std::string message(int e) const
		{
			switch (e)
			{
			case invalid_response:
				return "server response can't be parsed.";

			default:
				break;
			}
			return {};
		}
	};

	const boost::system::error_category& get_http_errors_category()
	{
		static http_errors_categogy cat;
		return cat;
	}

	boost::system::error_code make_error_code(http_error_codes e)
	{
		return boost::system::error_code(static_cast<int>(e), get_http_errors_category());
	}
}

namespace boost { namespace system {
	template<>
	struct is_error_code_enum<http_errors::http_error_codes>
	{
		BOOST_STATIC_CONSTANT(bool, value = true);
	};
}}