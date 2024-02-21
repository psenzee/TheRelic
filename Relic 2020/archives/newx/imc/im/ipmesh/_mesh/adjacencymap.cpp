#pragma warning(disable: 4786)

#include "adjacencymap.h"
#include "simplemesh.h"
#include "edgemap.h"

#include <iostream>

void AdjacencyMap::create(const EdgeMap &emap)
{
    int tsz = m_mesh->triangles.size();
	m_map.clear();
	m_map.resize(tsz, std::vector<int>());
    for (int i = 0; i < tsz; i++)
	    emap.findAdjacent(i, m_map[i]);
}