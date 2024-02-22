#ifndef _LINELIST_H
#define _LINELIST_H

#include <set>
#include <map>
#include <vector>
#include <deque>

#include "line.h"
#include "indexlist.h"
#include "point.h"
#include "../stlutil.h"

class LineList : public IndexList
{
	typedef std::map<int, std::pair<int, int> > adjacentmap_t;

public:

	void set(const std::vector<Line> &lines)
	{
		if (lines.size() > 0)
		{
			std::set<int>                 uniq;
	        std::map<int, Line>           m;
			std::map<int, Line>::iterator it,
			                              end;
		    m_mapA(lines, m);
		    Line &ln = const_cast<Line &>(lines[0]);
			end = m.end();
			list.push_back(ln.pa);
			uniq.insert(ln.pa);
            int count = 0;
			while (true)
			{
				if (uniq.find(ln.pb) != uniq.end())
				    break;
				list.push_back(ln.pb);
				uniq.insert(ln.pb);
			    if ((it = m.find(ln.pb)) == end)
				    break;
				ln = (*it).second;
				count++;
			}
		}
	}
	void optimize(const std::vector<Point> &points)
	{
	    m_rotateToCorner(points);
	    if (list.size() > 2)
		{
	        std::vector<int> out;
	        Point            p, q;
			int              px = list[0];
		    for (int i = 1, sz = list.size(); i < sz; i++)
		    {
                if (i == 1 || q != (p = (points[list[i]] - points[list[i - 1]]).normalize()))
				    out.push_back(list[i - 1]);
				q = p;
		    }
		    out.push_back(list[i - 1]);
			list = out;
		}
	}
	inline bool add(const Line &ln)
	{
	    bool r = true;
	    if (list.size() == 0)
		{
		    list.push_back(ln.pa);
			list.push_back(ln.pb);
		}
		else if (r = (ln.pa == list.back()))
            list.push_back(ln.pb);
		return r;
	}
	void diagonals(int pid, std::vector<int> &into) // optimize() prerequisite
	{
	    diagonals(pid, this->list, into);
	}
	bool isConvex(const std::vector<Point> &points) const // optimize() prerequisite
	{
	    if (list.size() > 2)
		{
	        Point refcross(m_vcross(0, points));
		    for (int i = 1, sz = list.size(); i < sz; i++)
		        if (refcross != m_vcross(i, points))
			        return false;
		}
        return true;
	}
	bool splitConcave(const std::vector<Point> &points, std::vector<LineList> &lists) const // optimize() prerequisite
	{
	    std::vector<int>                   ccv;
		std::vector< std::pair<int, int> > pairs;
		m_concaves(points, ccv);
		adjacentmap_t amap;
        m_adjacentmap(list, amap);
		m_diagonalPairs(amap, ccv, ccv, pairs);
		if (pairs.empty())
            m_diagonalPairs(amap, ccv, list, pairs);
		if (!pairs.empty())
		{
		    std::vector<int> lnsa,
			                 lnsb;
			splitLineList(pairs[0].first, pairs[0].second, lnsa, lnsb);
			lists.push_back(LineList());
			lists.back().list = lnsa;
			lists.back().optimize(points);
			lists.push_back(LineList());
			lists.back().list = lnsb;
			lists.back().optimize(points);
            return true;
		}
		return false;
	}
	void getLineList(int fromid, int toid, std::vector<int> &newlist) const
	{
	    bool addp  = false;
		int  point = 0;
	    for (int i = 0, sz = list.size(); i < sz || addp; i++)
		{
		    point = m_point(list, i, 0);
		    if (point == fromid)
			    addp = true;
			if (addp)
			    newlist.push_back(point);
			if (point == toid)
			    addp = false;
		}
	}
	void splitLineList(int fromid, int toid, std::vector<int> &lista, std::vector<int> &listb) const
	{
	    bool adda  = false,
		     addb  = true;
		int  point = 0;
	    for (int i = 0, sz = list.size(); i < sz; i++)
		{
		    point = m_point(list, i, 0);
		    if (point == fromid)
			    adda = true;
		    if (point == toid)
			    addb = true;

			if (adda) lista.push_back(point);
			if (addb) listb.push_back(point);

			if (point == toid)
			    adda = false;
			if (point == fromid)
				addb = false;
		}
	}
	static bool diagonal(const std::vector<int> list, int pida, int pidb)
	{
	    if (pida == pidb)
	        return false;
		int aid = StlUtil< std::vector<int> >::find(list, pida, -1),
		    bid = StlUtil< std::vector<int> >::find(list, pidb, -1),
			max = list.size() - 1;
		return (aid != -1 && bid != -1) && 
		      !(aid == 0 && bid == max || bid == 0 && aid == max || aid - 1 == bid || bid - 1 == aid);
	}
	static void diagonals(int pid, const std::vector<int> &list, std::vector<int> &into)
	{
	    for (int i = 0, sz = list.size(); i < sz; i++)
		    if (m_diagonal(list, pid, i))
                into.push_back(list[i]);
	}
	static void diagonals(int pid, const std::vector<int> &list, const std::vector<int> &in, std::vector<int> &into)
	{
	    int id = -1;
	    for (int i = 0, sz = in.size(); i < sz; i++)
		{
		    id = StlUtil< std::vector<int> >::find(list, pid, -1);
		    if (id != -1 && m_diagonal(list, id, i))
                into.push_back(list[i]);
		}
	}

private:

    static void m_diagonalPairs(adjacentmap_t &adjmap,
	                            const std::vector<int> &lista, const std::vector<int> &listb,
								std::vector< std::pair<int, int> > &pairs)
	{
	    int sza = lista.size(), szb = listb.size();
		for (int i = 0; i < sza; i++)
		    for (int j = 0; j < szb; j++)
			    if (lista[i] != listb[j] &&
				    adjmap[lista[i]].first  != listb[j] &&
					adjmap[lista[i]].second != listb[j])
					pairs.push_back(std::pair<int, int>(lista[i], listb[j]));
	}
    static void m_mapA(const std::vector<Line> &lines, std::map<int, Line> &map)
	{
        for (int i = 0, sz = lines.size(); i < sz; i++)
		    map[lines[i].pa] = lines[i];
	}
	inline static int m_findA(const std::vector<Line> &lines, int id)
	{
        for (int i = 0, sz = lines.size(); i < sz; i++)
		    if (lines[i].pa == id)
			    return i;
		return -1;
	}
	inline static int m_findB(const std::vector<Line> &lines, int id)
	{
        for (int i = 0, sz = lines.size(); i < sz; i++)
		    if (lines[i].pb == id)
			    return i;
		return -1;
	}
	inline Point m_vdir(int pid, const std::vector<Point> &points) const
	{
	    const Point &p = points[m_point(list, pid, -1)],
		            &q = points[m_point(list, pid,  0)];
		return q - p;
	}
    inline Point m_vcross(int pid, const std::vector<Point> &points) const
	{
        return (m_vdir(pid + 1, points).cross(m_vdir(pid, points))).normalize();
	}
	static inline int m_point(const std::vector<int> &list, int index, int offset)
	{
	    int sz = list.size();
	    while (index + offset < 0)
		    index += sz;
		return list[(index + offset) % sz];
	}
	static inline bool m_adjacent(const std::vector<int> &list, int pid, int index)
	{
	    return pid == m_point(list, index, -1) ||
		       pid == m_point(list, index,  0) ||
			   pid == m_point(list, index,  1);
	}
	static inline bool m_diagonal(const std::vector<int> &list, int pid, int index)
	{
	    return !m_adjacent(list, pid, index);
	}
	void m_rotateToCorner(const std::vector<Point> &points)
	{
	    int sz = list.size();
		// rotate the list until it starts on a corner.. TODO should make this rotate to convex corner only..
	    if (sz > 2)
		{
		    std::deque<int> ls(list.begin(), list.end());
	        for (int i = 0; i < sz &&
                    (points[ls[1]] - points[ls[0]]).normalize() == 
                    (points[ls[0]] - points[ls.back()]).normalize(); i++)
		    {
			    int id = ls.back();
				ls.pop_back();
				ls.push_front(id);
		    }
			list.clear();
			list.assign(ls.begin(), ls.end());
		}
	}
	void m_concaves(const std::vector<Point> &points, std::vector<int> &verts) const // optimize() prerequisite
	{
	    // TODO note: this will not work if first vertex is concave..
	    Point refcross(m_vcross(0, points));
		for (int i = 1, sz = list.size(); i < sz; i++)
		    if (refcross != m_vcross(i, points))
			    verts.push_back(i);
	}
	static void m_adjacentmap(const std::vector<int> &ids, adjacentmap_t &map)
	{
	    map.clear();
	    for (int i = 0, sz = ids.size(); i < sz; i++)
		    map[m_point(ids, i, 0)] = std::pair<int, int>(m_point(ids, i, -1), m_point(ids, i, 1));
    }
};

#endif  /** _LINELIST_H */
