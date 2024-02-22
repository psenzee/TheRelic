#ifndef _EDGEMAP_H
#define _EDGEMAP_H

class SimpleMesh;

#include <vector>
#include <map>

#include "line.h"
#include "triangleidlist.h"
#include "linelist.h"
#include "triangle.h"

class EdgeMap
{
public:
    
    typedef TriangleIdList triangle_ids_t;
	//typedef std::vector<int> triangle_ids_t;

private:

    const SimpleMesh               *m_mesh;
    std::map<Line,  triangle_ids_t> m_map;
	std::map<Point, triangle_ids_t> m_normals;
	triangle_ids_t                  m_list;

public:

    EdgeMap()                                                  : m_mesh(NULL) {}
    EdgeMap(const SimpleMesh *mesh, const TriangleIdList &ids) : m_mesh(mesh) { create(ids); }
    EdgeMap(const SimpleMesh *mesh)                            : m_mesh(mesh) { create(); }

    const std::vector<int>               &getList(void)         { return m_list.list; }
    const std::map<Line, triangle_ids_t> &getMap(void)          { return m_map; }
    const SimpleMesh                     *getMesh(void)         { return m_mesh; }

    inline std::vector<int> &operator[](const Line &ln)         { return m_map[ln].list; }

	void clear(void)
	{
	    m_map.clear();
		m_list.list.clear();
		m_normals.clear();
	}

    void set(const TriangleIdList &ids)                         { create(ids); }
    void set(const SimpleMesh *m, const TriangleIdList &ids)    { m_mesh = m; create(ids); }
    void set(const SimpleMesh *m)                               { m_mesh = m; create(); }

    void findAdjacent(int tid, TriangleIdList &ids)       const;
	void findAllAdjacent(int tid, TriangleIdList &ids)    const;
	bool isSharedEdge(const Line &line)                   const;
    void findExternalEdges(LineList &edges)               const;
    void findInternalEdges(LineList &edges)               const;
    void getFaces(std::vector<LineList> &faces);
	void getFaces(const TriangleIdList &tlist, std::vector<LineList> &faces);
	int  getTriangleAtEdge(int tid, Triangle::Connection c);
    void simplify(int tid, std::vector<Triangle> &tris, bool returnSimplifiedOnly = false);
	void simplify(const TriangleIdList &tlist, std::vector<Triangle> &tris, bool returnSimplifiedOnly = false);
	void simplify(std::vector<Triangle> &tris, bool returnSimplifiedOnly = false);

    void findAdjacent(int tid, std::vector<int> &tids)    const;
	void findAllAdjacent(int tid, std::vector<int> &tids) const;
    void findExternalEdges(std::vector<Line> &edges)      const;
    void findInternalEdges(std::vector<Line> &edges)      const;

private:

    void create(const TriangleIdList &ids);
    void create(void);
	void getTriangleIdList(TriangleIdList &list);
    void doMapEdgeNormals(void);
};

#endif  /** _EDGEMAP_H */