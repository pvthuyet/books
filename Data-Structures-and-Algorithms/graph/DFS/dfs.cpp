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

#include <map>
std::vector<bool> vis2;
std::vector<bool> backedages;

bool dfsIsCyc(int i, auto& adj)
{
	if (!vis2[i]) {
		vis2[i] = true;
		backedages[i] = true;
		for (auto it = adj[i].cbegin(); it != adj[i].cend(); ++it) {
			int v = *it;
			if (!vis2[v]) {
				auto rs = dfsIsCyc(v, adj);
				if (rs == true) return true;
			}
			else if (backedages[v]) {
				return true;
			}
		}
	}
	backedages[i] = false;
	return false;
}

bool graph_dfs::isCyclic()
{
	int V = 11;
	int E = 11;
	std::vector<std::vector<int>> adj(V, std::vector<int>{});
	adj[7].push_back(0);
	adj[0].push_back(4);
	adj[4].push_back(5);
	adj[5].push_back(6);
	adj[6].push_back(8);
	adj[8].push_back(9);
	adj[9].push_back(3);
	adj[3].push_back(2);
	adj[2].push_back(1);
	adj[1].push_back(10);
	adj[4].push_back(6);

	vis2.resize(V);
	fill_n(vis2.begin(), V, false);
	backedages.resize(V);
	fill_n(backedages.begin(), V, false);

	for (int i = 0; i < V; ++i) {
		if (dfsIsCyc(i, adj)) return true;
	}
	return false;
}

std::vector<bool> visited3;
std::map<int, int> parents3;

int getParent3(int v)
{
	auto found = parents3.find(v);
	if (found != parents3.cend()) return found->second;
	return -1;
}

bool dfs3(int i, auto& adj)
{
	if (!visited3[i]) {
		visited3[i] = true;
		for (auto it = adj[i].cbegin(); it != adj[i].cend(); ++it) {
			int v = *it;
			if (!visited3[v]) {
				parents3[v] = i;
				auto rs = dfs3(v, adj);
				if (rs) return true;
			}
			else if (v != getParent3(i)) {
				return true;
			}
		}
	}
	return false;
}

bool graph_dfs::isCycleUndirected()
{
	int V = 4;
	int E = 2;
	std::vector<std::vector<int>> adj(V, std::vector<int>{});
	adj[1].push_back(2);
	adj[2].push_back(1);
	adj[2].push_back(3);
	adj[3].push_back(2);

	visited3.resize(V);
	fill_n(visited3.begin(), V, false);
	for (int i = 0; i < V; ++i) {
		auto rs = dfs3(i, adj);
		if (rs) return true;
	}
	return false;
}