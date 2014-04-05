
#include "main.h"
#include "graph.h"

void graph_test()
{
	const int V = 13, E = 12;
	int val[V];
	for(int i=0;i<V;i++) val[i] = 1;
	naive_allocator al;
	separated_graph<int, naive_allocator> G(V, E, &al, val);

	int edges[E][2] =
	{
		{0, 1},
		{1, 2},
		{2, 3},
		{3, 4},
		{4, 0},
		{2, 5},
		{5, 6},
		{2, 7}, 
		{8, 9},
		{9, 10},
		{10, 12},
		{9, 11}
	};

	for(int i=0;i<E;i++) G.add_edge(edges[i][0], edges[i][1]);

	G.init();
	for(int i=0;i<V;i++){
		std::vector<int> ret = G.query(i);
		printf("%d: ", i);
		for(int j=0;j<ret.size();j++) printf("%d ", ret[j]);
		puts("");
	}
}
