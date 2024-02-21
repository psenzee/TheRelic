#ifndef _SIMPLEMESH_H
#define _SIMPLEMESH_H

#include "point.h"
#include "triangle.h"
#include "indexlist.h"

#include <map>
#include <vector>

class SimpleMesh
{
    int m_index(const Point &p, std::map<Point, int> &map)
	{
	    std::map<Point, int>::iterator it = map.find(p);
	    if (it == map.end())
		{
		    int id = vertices.size();
		    vertices.push_back(p);
            map[p] = id;
			return id;
		}
		return (*it).second;
	}

public:

    std::vector<Point>    vertices;
	std::vector<Triangle> triangles;

    SimpleMesh() {}
	SimpleMesh(std::vector<PointTriangle> &mesh)
	{
	    std::map<Point, int> points;
		Triangle t;
		for (int i = 0, sz = mesh.size(); i < sz; i++)
		{
		    t.pa = m_index(mesh[i].a, points);
		    t.pb = m_index(mesh[i].b, points);
		    t.pc = m_index(mesh[i].c, points);
			triangles.push_back(t);
		}
	}

    int find(const Triangle &t);
    int find(const Triangle &t, const IndexList &ids);
    int find(const Point &p);
    int find(const Point &p, const IndexList &ids);
	int find(const Point &a, const Point &b, const Point &c)
	{
	    int ida = find(a), idb = find(b), idc = find(c);
		return (ida < 0 || idb < 0 || idc < 0) ? -1 : find(Triangle(ida, idb, idc));
	}
	int vertex(const Point &p)
	{
	    int id = find(p);
		if (id == -1) { id = vertices.size(); vertices.push_back(p); }
		return id;
	}
	int triangle(const Triangle &t)
	{
	    int id = find(t);
		if (id == -1) { id = triangles.size(); triangles.push_back(t); }
		return id;
	}
	int triangle(const Point &a, const Point &b, const Point &c)
	{
	    int ida = find(a), idb = find(b), idc = find(c);
		return (ida < 0 || idb < 0 || idc < 0) ? -1 : triangle(Triangle(ida, idb, idc));
	}
};

#endif  /** _SIMPLEMESH_H */