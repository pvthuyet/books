#pragma once

#include <unordered_map>
#include <list>
#include <vector>

class graph_dfs
{
private:
	std::unordered_map<int, bool> mVisited;
	std::unordered_map<int, std::list<int>> mAdj;
	std::vector<int> mResult;
	void dfs(int v);

public:
	void add_edge(int v, int u);
	void run_dfs(int v);
	void dfs_util();
	void print_result() const;
};