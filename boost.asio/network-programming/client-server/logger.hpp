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
	void info(First first, const Args&... args)
	{
		std::lock_guard<std::mutex> lock(mMux);
		std::cout << first;
		auto outSpace = [](auto const& arg) {
			std::cout << ' ' << arg;
		};
		(..., outSpace(args));
		std::cout << std::endl;
	};

	void trace(std::string_view msg, const boost::source_location& loc)
	{
		std::lock_guard<std::mutex> lock(mMux);
		std::cout << loc.function_name() << ":" << loc.line() << " - " << msg << std::endl;
	}

private:
	logger() = default;

private:
	std::mutex mMux;
};

#define LOGENTER	logger::get_inst().trace("Enter {", BOOST_CURRENT_LOCATION)
#define LOGEND		logger::get_inst().trace("End }", BOOST_CURRENT_LOCATION)

SG_END