#ifndef _MESHINDEX_H
#define _MESHINDEX_H

#include "point.h"
#include "triangle.h"
#include "indexlist.h"
#include "simplemesh.h"
#include "planemap.h"
#include "adjacencymap.h"
#include "edgemap.h"

#include <map>
#include <vector>

class MeshIndex
{
    SimpleMesh              *m_mesh;
    std::map<Point,    int>  m_vertices;
	std::map<Triangle, int>  m_triangles;
	PlaneMap                 m_planes;
	EdgeMap                  m_edges;
	AdjacencyMap             m_adjacency;

public:

    inline MeshIndex(SimpleMesh *mesh) : m_mesh(mesh) {}

    inline SimpleMesh   *getMesh(void)         { return m_mesh; }
	inline PlaneMap     &getPlaneMap(void)     { return m_planes; }
	inline EdgeMap      &getEdgeMap(void)      { return m_edges; }
	inline AdjacencyMap &getAdjacencyMap(void) { return m_adjacency; }

    void clear(void);
	int  find(const Point &p);
	int  find(const Triangle &t);
	int  find(const Point &a, const Point &b, const Point &c);
	void index(void);
    void getAllFaces(std::vector<LineList> &lns);
    void simplify(std::vector<Triangle> &tris, bool returnSimplifiedOnly = false);
	int  vertex(const Point &p);
	int  triangle(const Triangle &t);
	int  triangle(const Point &a, const Point &b, const Point &c);
	void getMeshSubset(const TriangleIdList &ids, SimpleMesh &mesh);

protected:

    void        indexVertices(void);
	void        indexTriangles(void);
	void        indexEdges(void);
	void        indexPlanes(void);
	void        indexAdjacency(void);
	inline bool trianglesIndexed(void) { return !m_triangles.empty();   }
	inline bool verticesIndexed(void)  { return !m_vertices.empty();    }
	inline bool planesIndexed(void)    { return !m_planes.getList().empty(); }
	inline bool edgesIndexed(void)     { return !m_edges.getList().empty();  }

private:

    void m_stats(void);
};

#endif  /** _MESHINDEX_H */