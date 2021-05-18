#include <bitset>
#include <iostream>

class bital
{
public:
	static int add(int x, int y)
	{
		constexpr int N = 8;
		std::bitset<N> binx(x);
		std::bitset<N> biny(y);
		std::cout << x << ": " << binx << std::endl;
		std::cout << y << ": " << biny << std::endl;

		while (y != 0) {
			int carry = x & y;
			std::cout << "\tx&y:\t\t" << std::bitset<N>(carry) << std::endl;
			x = x ^ y;
			std::cout << "\tx^y\t\t" << std::bitset<N>(x) << std::endl;
			y = carry << 1;
			std::cout << "\t(x&y)<<1:\t" << std::bitset<N>(y) << std::endl;
			std::cout << std::endl;
		}
		std::cout << "sum: " << x << std::endl;
		return x;
	}
};