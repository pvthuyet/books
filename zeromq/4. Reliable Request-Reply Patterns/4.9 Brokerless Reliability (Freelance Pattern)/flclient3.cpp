#include "flcapi.hpp"
#include <iostream>

int main()
{
	using namespace std::literals;
	using namespace date;
	namespace chr = std::chrono;
	auto ymd = 2015_y / sep / 25;
	auto next_moth = ymd + months{ 1 };
	chr::time_point<chr::system_clock> tp = chr::floor<chr::milliseconds>(chr::system_clock::now());
	std::cout << tp << std::endl;
	auto next = tp + 10ms;
	std::cout << next << std::endl;
	return 0;
}