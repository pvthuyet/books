#include "dyp.hpp"
#include <iomanip>

int main()
{
	std::cout << std::fixed << std::setprecision(6) << std::left;
	//dyp::fibonacci(1000);
	//dyp::lcs("GXTXAYB", "AGGTAB");
	//dyp::coinchange();
	dyp::matrixMultiplication();
	return EXIT_SUCCESS;
}