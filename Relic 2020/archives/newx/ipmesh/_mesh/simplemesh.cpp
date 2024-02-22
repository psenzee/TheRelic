#pragma warning(disable: 4786)

#include "simplemesh.h"

#include <map>
#include <vector>

int SimpleMesh::find(const Triangle &t)
{
    for (int i = 0, sz = triangles.size(); i < sz; i++)
	    if (t == triangles[i])
		    return i;
    return -1;
}

int SimpleMesh::find(const Triangle &t, const IndexList &ids)
{
    const std::vector<int> arr = ids.list;
    for (int i = 0, sz = ids.size(); i < sz; i++)
	    if (t == triangles[arr[i]])
		    return arr[i];
    return -1;
}

int SimpleMesh::find(const Point &p)
{
    for (int i = 0, sz = vertices.size(); i < sz; i++)
	    if (p == vertices[i])
		    return i;
    return -1;
}

int SimpleMesh::find(const Point &p, const IndexList &ids)
{
    const std::vector<int> arr = ids.list;
    for (int i = 0, sz = vertices.size(); i < sz; i++)
	    if (p == vertices[arr[i]])
		    return arr[i];
    return -1;
}