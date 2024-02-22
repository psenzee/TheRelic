#ifndef _ADJACENCYMAP_H
#define _ADJACENCYMAP_H

class SimpleMesh;
class EdgeMap;

#include <vector>

#include "line.h"
#include "triangleidlist.h"
#include "linelist.h"
#include "triangle.h"

class AdjacencyMap
{
    const SimpleMesh                 *m_mesh;
    std::vector< std::vector<int> >   m_map;

public:

    inline AdjacencyMap() : m_mesh(NULL) {}
    
    const std::vector< std::vector<int> >  &getMap(void)  { return m_map;  }
    const SimpleMesh                       *getMesh(void) { return m_mesh; }

	void clear(void)                                      { m_map.clear(); }

    void set(const SimpleMesh *m, const EdgeMap &emap)    { m_mesh = m; create(emap); }

	inline std::vector<int> &at(int i)              { return m_map.at(i); }
	inline std::vector<int> &operator[](int i)      { return m_map[i]; }

	inline bool isAdjacent(int tida, int tidb, bool includeSelf = true)
	{
		return tida == tidb ? includeSelf : find(at(tida), tidb) != -1;
	}
	int getOtherAdjacent(int tida, int tidb, std::vector<int> &into)
	{
	    int n, count = 0;
		std::vector<int> &adj = m_map[tida];
		for (int i = 0, sz = adj.size(); i < sz; i++)
		{
		    n = adj[i];
			if (n != tidb && n != tida)
			{
			    count++;
			    into.push_back(n);
			}
		}
		return count;
	}
	int getCommonAdjacent(int tida, int tidb)
	{
		std::vector<int> adja, adjb;
		if (getOtherAdjacent(tida, tidb, adja) > 0 &&
		    getOtherAdjacent(tidb, tida, adjb) > 0)
		    return findCommon(adja, adjb);
		return -1;
	}

private:

    void create(const EdgeMap &emap);
	static inline int find(const std::vector<int> &v, int id)
	{
		for (int i = 0, sz = v.size(); i < sz; i++)
		    if (v[i] == id)
			    return i;
		return -1;
	}
	static inline int findCommon(const std::vector<int> &v, const std::vector<int> &u)
	{
	    int id = -1;
		for (int i = 0, sz = u.size(); i < sz; i++)
		    if ((id = find(v, u[i])) != -1)
			    return v[id];
		return -1;
	}
};

#endif  /** _ADJACENCYMAP_H */