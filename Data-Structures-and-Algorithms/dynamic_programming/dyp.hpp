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

	static int lcs(std::string_view const s1, std::string_view const s2)
	{
		auto l1 = s1.length();
		auto l2 = s2.length();
		std::vector<std::vector<int>> lens(l1 + 1, std::vector<int>(l2 + 1, 0));
		for (int i = 1; i <= l1; ++i) {
			for (int j = 1; j <= l2; ++j) {
				if (s1[i - 1] == s2[j - 1]) {
					lens[i][j] = 1 + lens[i - 1][j - 1];
				}
				else {
					lens[i][j] = std::max<int>(lens[i - 1][j], lens[i][j - 1]);
				}
			}
		}
		return lens[l1][l2];
	}

	static int coinchange()
	{
		int n = 4;
		int m = 3;
		int S[] = { 1,2,3 };
		count2(S, m, n);
		std::vector<std::vector<int>> cache(m + 1, std::vector<int>(n + 1, 0));
		fill_n(cache[0].begin(), n + 1, 0);
		for (int r = 0; r <= m; ++r) {
			cache[r][0] = 1;
		}

		for (int r = 1; r <= m; ++r) {
			for (int c = 1; c <= n; ++c) {
				if (c >= S[r - 1]) {
					cache[r][c] = cache[r - 1][c] + cache[r][c - S[r - 1]];
				}
				else {
					cache[r][c] = cache[r - 1][c];
				}
			}
		}
		return cache[m][n];
	}
	static long long int count2(int S[], int m, int n)
	{
		std::vector<long long int> cache(n + 1, 0);
		cache[0] = 1;

		for (int r = 1; r <= m; ++r) {
			for (int c = 1; c <= n; ++c) {
				if (c >= S[r - 1])
					cache[c] += cache[c - S[r - 1]];
			}
		}
		return cache[n];
	}

	static int matrixMultiplication()
	{
		// creating a 2d array to store the result
		int N = 4;
		int arr[] = {10, 30, 5, 60};
		std::vector<std::vector<int>> dp(N + 1, std::vector<int>(N + 1, 0));
		
		for (int i = 0; i < N; i++)
			dp[i][i] = 0;

		for (int L = 2; L < N; L++) {
			for (int i = 1; i < N - L + 1; i++) {
				int j = i + L - 1;
				dp[i][j] = INT_MAX;
				for (int k = i; k < j; k++) {
					dp[i][j] = std::min<int>(dp[i][j],
						dp[i][k] + dp[k + 1][j] + arr[i - 1] * arr[k] * arr[j]);
				}
			}
		}
		return dp[1][N - 1];
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
