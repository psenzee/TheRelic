#pragma warning(disable: 4786)

#include "meshindex.h"
#include "timer.h"
#include "edgemap.h"
#include "simplifier.h"

void MeshIndex::clear(void)
{
    m_vertices.clear();
    m_triangles.clear();
	m_planes.clear();
	m_edges.clear();
}

int MeshIndex::find(const Point &p)
{
    std::map<Point, int>::iterator it = m_vertices.find(p);
	return it == m_vertices.end() ? -1 : (*it).second;
}

int MeshIndex::find(const Triangle &t)
{
    std::map<Triangle, int>::iterator it = m_triangles.find(t);
	return it == m_triangles.end() ? -1 : (*it).second;
}

int MeshIndex::find(const Point &a, const Point &b, const Point &c)
{
    int ida = find(a), idb = find(b), idc = find(c);
	return (ida < 0 || idb < 0 || idc < 0) ? -1 : find(Triangle(ida, idb, idc));
}

void MeshIndex::index(void)
{
	m_stats();

    Timer etime, ttime;

    indexVertices();	etime.report(std::cout, "  index time - vertices");
    indexTriangles();	etime.report(std::cout, "  index time - triangles");
	indexEdges();       etime.report(std::cout, "  index time - edges");
	indexPlanes();      etime.report(std::cout, "  index time - planes");
	indexAdjacency();   etime.report(std::cout, "  index time - adjacency");

	ttime.report(std::cout, "total index time");
}

void MeshIndex::getAllFaces(std::vector<LineList> &lns)
{
	if (!planesIndexed())
	    indexPlanes();

	Timer   etime;
    EdgeMap emap;

	for (int i = 0, sz = m_planes.getPlaneCount(); i < sz; i++)
	{
        emap.set(m_mesh, *m_planes.getTriangles(m_planes.getPlane(i)));
        emap.getFaces(lns);
    }

	etime.report(std::cout, "face extraction time");
    std::cout << "face count (" << lns.size() << ")" << std::endl;
}

void MeshIndex::simplify(std::vector<Triangle> &tris, bool returnSimplifiedOnly)
{
	if (!planesIndexed())
	    indexPlanes();

	Timer   etime;
	EdgeMap emap;

    for (int i = 0, sz = m_planes.getPlaneCount(); i < sz; i++)
    {
        emap.set(m_mesh, *m_planes.getTriangles(m_planes.getPlane(i)));
      //emap.simplify(tris, returnSimplifiedOnly);
	    Simplifier::simplify(*this, emap.getList(), tris, returnSimplifiedOnly);
    }

	etime.report(std::cout, "simplification time");
    std::cout << "triangle count (" << tris.size() << ")" << std::endl;
}

void MeshIndex::m_stats(void)
{
    std::cout << "triangle count (" << m_mesh->triangles.size() << ")" << std::endl;
    std::cout << "vertex count (" << m_mesh->vertices.size() << ")" << std::endl;
}

int MeshIndex::vertex(const Point &p)
{
    int id = find(p);
	if (id == -1)
	{
	    id = m_mesh->vertices.size();
		m_mesh->vertices.push_back(p);
        m_vertices[p] = id;
    }
	return id;
}

int MeshIndex::triangle(const Triangle &t)
{
    int id = find(t);
	if (id == -1)
	{
	    id = m_mesh->triangles.size();
		m_mesh->triangles.push_back(t);
        m_triangles[t] = id;
    }
	return id;
}

int MeshIndex::triangle(const Point &a, const Point &b, const Point &c)
{
    int ida = find(a), idb = find(b), idc = find(c);
	return (ida < 0 || idb < 0 || idc < 0) ? -1 : triangle(Triangle(ida, idb, idc));
}

void MeshIndex::getMeshSubset(const TriangleIdList &ids, SimpleMesh &mesh)
{
}

void MeshIndex::indexVertices(void)
{
	m_vertices.clear();
	std::vector<Point> &verts = m_mesh->vertices;
    for (int i = 0, sz = verts.size(); i < sz; i++)
	    m_vertices[verts[i]] = i;
}

void MeshIndex::indexTriangles(void)
{
    m_triangles.clear();
    std::vector<Triangle> &tris = m_mesh->triangles;
    for (int i = 0, sz = tris.size(); i < sz; i++)
	    m_triangles[tris[i]] = i;
}

void MeshIndex::indexEdges(void)
{
    m_edges.set(m_mesh);
}

void MeshIndex::indexPlanes(void)
{
    m_planes.set(m_mesh);
}

void MeshIndex::indexAdjacency(void)
{
    if (!edgesIndexed())
	    indexEdges();
	m_adjacency.set(m_mesh, m_edges);
}