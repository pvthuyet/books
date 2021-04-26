#include "dfs.hpp"
#include <algorithm>
#include <iostream>

void graph_dfs::add_edge(int v, int u)
{
	mAdj[v].push_back(u);
}

void graph_dfs::dfs(int v)
{
	mVisited[v] = true;
	mResult.push_back(v);
	for (auto it = mAdj[v].cbegin(); it != mAdj[v].cend(); ++it) {
		if (!mVisited[*it]) {
			dfs(*it);
		}
	}
}

void graph_dfs::run_dfs(int v)
{
	mResult.clear();
	mVisited.clear();
	dfs(v);
}

void graph_dfs::dfs_util()
{
	mResult.clear();
	mVisited.clear();
	for (auto const& i : mAdj) {
		if (!mVisited[i.first]) {
			dfs(i.first);
		}
	}
}

void graph_dfs::print_result() const
{
	for (auto const& v : mResult) std::cout << v << " ";
	std::cout << std::endl;
}