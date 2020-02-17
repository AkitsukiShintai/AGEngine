#ifndef KRUSKAL_H
#define KRUSKAL_H
#include "disjoint_sets.h"
#include "graph.h"

template <typename Vertex, typename Edge>
std::vector<Edge> kruskal( Graph<Vertex,Edge> const& g ) {


	std::vector<Edge> sortedEdge(g.GetEdges().begin(), g.GetEdges().end());

	std::vector<Edge> mst;
	std::sort(sortedEdge.begin(), sortedEdge.end());
	DisjointSets sets(g.Size());
	for (size_t i = 0; i < g.Size(); ++i) sets.Make();

	for (size_t i = 0; i< sortedEdge.size(); i++)
	{
		if (sets.GetRepresentative(sortedEdge[i].ID1()) != sets.GetRepresentative(sortedEdge[i].ID2()))
		{
			sets.Join(sortedEdge[i].ID1(), sortedEdge[i].ID2());
			mst.push_back(sortedEdge[i]);
		}
		
	}
	
	return mst;
}

#endif
