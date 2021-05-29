#pragma once
#include "define.hpp"
#include <mutex>
#include <iostream>
#include <string>
#include <boost/assert/source_location.hpp>

SG_BEGIN

class logger
{
public:
	static logger& get_inst()
	{
		static logger log;
		return log;
	}

	template<typename First, typename... Args>
	void info(const boost::source_location& loc, First first, const Args&... args)
	{
		std::string_view fname(loc.function_name());
		std::string_view nameonly{ fname.substr(0, fname.find_first_of('(')) };

		std::lock_guard<std::mutex> lock(mMux);
		std::cout << nameonly << ":" << loc.line() << " - ";
		std::cout << first;
		auto outSpace = [](auto const& arg) {
			std::cout << ' ' << arg;
		};
		(..., outSpace(args));
		std::cout << std::endl;
	};

	template<typename MSG>
	void trace(MSG msg, const boost::source_location& loc)
	{
		std::string_view fname(loc.function_name());
		std::string_view nameonly{ fname.substr(0, fname.find_first_of('(')) };

		std::lock_guard<std::mutex> lock(mMux);
		std::cout << nameonly << ":" << loc.line() << " - " << msg << std::endl;
	}

private:
	logger() = default;

private:
	std::mutex mMux;
};

#define LOGENTER		logger::get_inst().trace("Enter {"sv, BOOST_CURRENT_LOCATION)
#define LOGEND			logger::get_inst().trace("End }"sv, BOOST_CURRENT_LOCATION)
#define LOGDEBUG(msg)	logger::get_inst().trace(msg, BOOST_CURRENT_LOCATION)
#define LOGINFO			logger::get_inst().info

SG_END