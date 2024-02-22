#pragma warning(disable: 4786)

#include "edgemap.h"
#include "triangle.h"
#include "line.h"
#include "simplemesh.h"
#include "triangleidlist.h"
#include "planemap.h"
#include "pointline.h"

#include <vector>
#include <map>
#include <set>
#include <algorithm>
#include <iterator>

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <mmsystem.h>

bool collapseToOne(const Triangle &ta, const Triangle &tb, const std::vector<Point> &points, Triangle &out)
{
    if (ta == tb)
	{
	    out = ta;
	    return true;
	}

	Triangle::Connection ca,
	                     cb = tb.getSharedEdgeConnection(ta, ca);
    if (ca == Triangle::CONN_NONE)
	    return false;

    int oa = ta.getPoint(ca, Triangle::EP_OTHER),
        ob = tb.getPoint(cb, Triangle::EP_OTHER),
		aa = ta.getPoint(ca, Triangle::EP_START),
		ab = ta.getPoint(ca, Triangle::EP_END);

    PointLine lna0(points[oa], points[aa]), lna1(points[aa], points[ob]),
	          lnb0(points[oa], points[ab]), lnb1(points[ab], points[ob]);

    if      (lna0.parallel(lna1)) out.set(oa, ab, ob);
	else if (lnb0.parallel(lnb1)) out.set(oa, aa, ob);
	else return false;
	// TODO reverse normal if necessary..
	return true;
}

int EdgeMap::getTriangleAtEdge(int tid, Triangle::Connection c)
{
    const Triangle   &t   = m_mesh->triangles[tid];
	std::vector<int> &ids = m_map[t.edge(c)].list;
	for (int i = 0, sz = ids.size(); i < sz; i++)
	    if (ids[i] != tid)
		    return ids[i];
	return -1;
}

#include <fstream>

#define _DOLOG_

void collapse(EdgeMap &em, std::ostream &log, const std::vector<int> &tids, int start, int end,
              const std::vector<Triangle> &tris, const std::vector<Point> &points, std::vector<Triangle> &out, bool returnSimplifiedOnly = false)
{
    Triangle         t;
	std::set<int>    collapsed;
	int              i, tid, tidj;
	std::vector<int> adjacent;
    for (i = start; i < end; i++)
	{
	    tid = tids[i];
	    if (tris[tid].degenerate(points))
		{
		    collapsed.insert(tid);
#ifdef _DOLOG_
log << std::endl << "degenerate ";
tris[tid].print(log, points);
#endif
		}
		else if (collapsed.find(tid) == collapsed.end())
		{
		    adjacent.clear();
			em.findAdjacent(tid, adjacent);
	        for (int j = 0, sz = adjacent.size(); j < sz; j++)
		    {
			    tidj = adjacent[j];
			    if (tid != tidj)
				{
			        if (tris[tidj].degenerate(points))
					{
#ifdef _DOLOG_
log << std::endl << "degenerate ";
tris[tidj].print(log, points);
#endif
				        collapsed.insert(tidj);
					}
				    else if (collapsed.find(tidj) == collapsed.end() && 
				             collapseToOne(tris[tid], tris[tidj], points, t))
			        {
			            out.push_back(t);
				        collapsed.insert(tid);
			   	        collapsed.insert(tidj);
#ifdef _DOLOG_
log << std::endl << "collapsing ";
tris[tid].print(log, points);
tris[tidj].print(log, points);
log << " to ";
t.print(log, points);
#endif
			        }
				}
		    }			
		}
		log << std::flush;
	}
	if (!returnSimplifiedOnly)
	{
	    std::set<int>::iterator cend = collapsed.end();
        for (i = start; i < end; i++)
	        if (collapsed.find(tids[i]) == cend)
		        out.push_back(tris[tids[i]]);
	}
}
/*
bool find(const Triangle &t, Triangle::Connection from, int depth, int maxdepth, const Triangle &source)
{
    
    if (depth == 0)
	    find(Triangle::CONN_AB, 1, maxdepth, t);



}
*/

void EdgeMap::simplify(int tid, std::vector<Triangle> &out, bool returnSimplifiedOnly)
{
    std::ofstream log("collapse.log", std::ios::app);
    TriangleIdList list;
	findAllAdjacent(tid, list);
	collapse(*this, log, list.list, 0, list.size(), m_mesh->triangles, m_mesh->vertices, out, returnSimplifiedOnly);
    log.close();
}

void EdgeMap::simplify(const TriangleIdList &tlist, std::vector<Triangle> &out, bool returnSimplifiedOnly)
{
    std::ofstream log("collapse.log", std::ios::app);
    std::set<int> uniq;
	for (int i = 0, sz = tlist.size(); i < sz; i++)
	{
	    if (uniq.find(tlist.list[i]) == uniq.end())
		{
		    TriangleIdList ids;
		    findAllAdjacent(tlist.list[i], ids);
            collapse(*this, log, ids.list, 0, ids.size(), m_mesh->triangles, m_mesh->vertices, out, returnSimplifiedOnly);
			uniq.insert(tlist.list[i]);
			for (int j = 0, jsz = ids.size(); j < jsz; j++)
			    uniq.insert(ids.list[j]);
		}
	}
    log.close();
}

void EdgeMap::doMapEdgeNormals(void)
{
	const std::vector<Triangle> &tris   = m_mesh->triangles;
	const std::vector<Point>    &points = m_mesh->vertices;
	TriangleIdList         tlist;
	Point                  norm;
	getTriangleIdList(tlist);
	std::vector<int>      &tids   = tlist.list;
    for (int i = 0, sz = tids.size(); i < sz; i++)
	{
		const Triangle &t = tris[tids[i]];
        m_normals[PointLine(points[t.pa], points[t.pb]).normal()].add(tids[i]);
        m_normals[PointLine(points[t.pb], points[t.pc]).normal()].add(tids[i]);
        m_normals[PointLine(points[t.pc], points[t.pa]).normal()].add(tids[i]);
	}
}

void EdgeMap::simplify(std::vector<Triangle> &out, bool returnSimplifiedOnly)
{
    simplify(m_list, out, returnSimplifiedOnly);
}

void EdgeMap::create(const TriangleIdList &tris)
{
    m_list.list.clear();
    m_map.clear();
	if (m_mesh != NULL)
	{
	    const std::vector<int> &ids = tris.list;
        for (int i = 0, sz = ids.size(); i < sz; i++)
	    {
	        const Triangle &t = m_mesh->triangles[ids[i]];
		    m_list.add(ids[i]);
            m_map[Line(t.pa, t.pb)].add(ids[i]);
		    m_map[Line(t.pb, t.pc)].add(ids[i]);
		    m_map[Line(t.pc, t.pa)].add(ids[i]);
	    }
	}
	doMapEdgeNormals();
}

void EdgeMap::create(void)
{
    m_list.list.clear();
    m_map.clear();
	if (m_mesh != NULL)
	{
        for (int i = 0, sz = m_mesh->triangles.size(); i < sz; i++)
	    {
	        const Triangle &t = m_mesh->triangles[i];
		    m_list.add(i);
            m_map[Line(t.pa, t.pb)].add(i);
		    m_map[Line(t.pb, t.pc)].add(i);
		    m_map[Line(t.pc, t.pa)].add(i);
	    }
	}
	doMapEdgeNormals();
}

void EdgeMap::findAdjacent(int tid, TriangleIdList &tris) const
{
	findAdjacent(tid, tris.list);
}

void EdgeMap::findAllAdjacent(int tid, TriangleIdList &ids) const
{
    findAllAdjacent(tid, ids.list);
}

void EdgeMap::findAdjacent(int tid, std::vector<int> &ids) const
{
    std::map<Line, TriangleIdList> &map = const_cast<std::map<Line, TriangleIdList> &>(m_map);
    if (tid >= 0)
	{
	    const Triangle &t = m_mesh->triangles[tid];
		std::vector<int> &vab = map[Line(t.pa, t.pb)].list,
		                 &vbc = map[Line(t.pb, t.pc)].list,
		                 &vca = map[Line(t.pc, t.pa)].list;
		std::copy(vab.begin(), vab.end(), std::back_insert_iterator< std::vector<int> >(ids));
		std::copy(vbc.begin(), vbc.end(), std::back_insert_iterator< std::vector<int> >(ids));
        std::copy(vca.begin(), vca.end(), std::back_insert_iterator< std::vector<int> >(ids));
	}
}

void EdgeMap::findAllAdjacent(int tid, std::vector<int> &ids) const
{
    std::set<int>     uniq;
	std::vector<int>  stack;
	int               id;
	stack.push_back(tid);
	while (!stack.empty())
	{
	    id = stack.back();
		stack.pop_back();
		if (uniq.find(id) == uniq.end())
		{
	        uniq.insert(id);
		    findAdjacent(id, stack);
		}
	}
	std::copy(uniq.begin(), uniq.end(), 
	    std::back_insert_iterator< std::vector<int> >(ids));
}

void EdgeMap::getFaces(std::vector<LineList> &faces)
{
    getFaces(m_list, faces);
}

void EdgeMap::getFaces(const TriangleIdList &tlist, std::vector<LineList> &faces)
{
    std::set<int> uniq;
	for (int i = 0, sz = tlist.size(); i < sz; i++)
	{
	    if (uniq.find(tlist.list[i]) == uniq.end())
		{
		    TriangleIdList ids;
		    findAllAdjacent(tlist.list[i], ids);
			uniq.insert(tlist.list[i]);
			for (int j = 0, jsz = ids.size(); j < jsz; j++)
			    uniq.insert(ids.list[j]);
			faces.push_back(LineList());
			EdgeMap em(m_mesh, ids);
			em.findExternalEdges(faces.back());
			faces.back().optimize(m_mesh->vertices);
		}
	}
}


void EdgeMap::getTriangleIdList(TriangleIdList &list)
{    
   for (std::map<Line, TriangleIdList>::iterator it = m_map.begin(), end = m_map.end(); it != end; ++it)
       std::copy((*it).second.list.begin(), (*it).second.list.end(), 
	       std::back_insert_iterator< std::vector<int> >(list.list));
}

bool EdgeMap::isSharedEdge(const Line &line) const
{
    return const_cast<std::map<Line, TriangleIdList> &>(m_map)[line].size() > 1;
}

void EdgeMap::findExternalEdges(LineList &lines) const
{
    std::vector<Line> lns;
    findExternalEdges(lns);
    lines.set(lns);
}

void EdgeMap::findInternalEdges(LineList &lines) const
{
    std::vector<Line> lns;
    findInternalEdges(lns);
    lines.set(lns);
}

void EdgeMap::findExternalEdges(std::vector<Line> &lines) const
{
    std::map<Line, TriangleIdList> &map = const_cast<std::map<Line, TriangleIdList> &>(m_map);
    for (std::map<Line, TriangleIdList>::iterator it = map.begin(), end = map.end(); it != end; ++it)
        if ((*it).second.size() == 1)
		    lines.push_back((*it).first);
}

void EdgeMap::findInternalEdges(std::vector<Line> &lines) const
{
    std::map<Line, TriangleIdList> &map = const_cast<std::map<Line, TriangleIdList> &>(m_map);
    for (std::map<Line, TriangleIdList>::iterator it = map.begin(), end = map.end(); it != end; ++it)
        if ((*it).second.size() > 1)
		    lines.push_back((*it).first);
}