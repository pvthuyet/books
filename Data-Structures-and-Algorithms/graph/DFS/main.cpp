#include "dfs.hpp"
#include <iostream>

int main()
{
	graph_dfs g;
	g.add_edge(0, 1);
	g.add_edge(0, 9);
	g.add_edge(1, 2);
	g.add_edge(2, 0);
	g.add_edge(2, 3);
	g.add_edge(9, 3);

	std::cout << "DFS of vertice 2: ";
	g.run_dfs(2);
	g.print_result();

	std::cout << "DFS all: ";
	g.dfs_util();
	g.print_result();

	return EXIT_SUCCESS;
}