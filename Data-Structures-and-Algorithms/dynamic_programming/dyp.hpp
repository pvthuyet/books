#include <iostream>
#include <vector>
#include <algorithm>
#include <chrono>

class dyp
{
public:
	static int fibonacci(int n)
	{
		// recursive method
		/*
		{
			auto start = std::chrono::steady_clock::now();
			auto rs = dyp{}.fibo_recur(n);
			std::cout << std::endl;
			auto end = std::chrono::steady_clock::now();
			std::chrono::duration<double> diff = end - start;
			std::cout << "fibo " << n << "th: " << rs << std::endl;
			std::cout << "time recursive: " << diff.count() * 1000 << " ms" << std::endl;
		}
		*/
		// dynamic programing
		{
			auto start = std::chrono::steady_clock::now();
			std::vector<int> fibos(n + 1, 0);
			fibos[0] = 0;
			fibos[1] = 1;
			auto rs = dyp{}.fibo_memoization(n, fibos);
			std::chrono::duration<double> diff = std::chrono::steady_clock::now() - start;
			std::cout << "fibo " << n << "th: " << rs << std::endl;
			std::cout << "time memoization: " << diff.count() * 1000 << " ms" << std::endl;
			//for (auto x : fibos) std::cout << x << ' '; std::cout << std::endl;
		}
		{
			auto start = std::chrono::steady_clock::now();
			auto rs = dyp{}.fibo_tabulation(n);
			std::chrono::duration<double> diff = std::chrono::steady_clock::now() - start;
			std::cout << "fibo " << n << "th: " << rs << std::endl;
			std::cout << "time tabulation: " << diff.count() * 1000 << " ms" << std::endl;
		}
		return 0;
	}


private:
	int fibo_recur(int n)
	{
		if (n <= 1) return n;
		return fibo_recur(n-1) + fibo_recur(n-2);
	}

	int fibo_memoization(int n, auto& cache)
	{
		if (cache[n] == 0) {
			if (n <= 1) {
				cache[n] = n;
			}
			else {
				cache[n] = fibo_memoization(n - 1, cache) + fibo_memoization(n - 2, cache);
			}
		}
		return cache[n];
	}

	int fibo_tabulation(int n)
	{
		int v0 = 0;
		int v1 = 1;
		int v = 0;
		for (int i = 2; i <= n; ++i) {
			v = v0 + v1;
			v0 = v1;
			v1 = v;
		}
		return v;
	}
};
