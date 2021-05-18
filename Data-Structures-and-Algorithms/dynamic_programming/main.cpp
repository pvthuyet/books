#include "dyp.hpp"
#include <iomanip>

int main()
{
	std::cout << std::fixed << std::setprecision(6) << std::left;
	dyp::fibonacci(1000);
	return EXIT_SUCCESS;
}