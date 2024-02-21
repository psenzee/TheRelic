#ifndef _SIMPLIFIER_H
#define _SIMPLIFIER_H

#define _DOLOG_

#include <fstream>

class Simplifier
{
private:

    static bool m_collapseTwoToOne(const Triangle &ta, const Triangle &tb,
	                        const std::vector<Point> &points, Triangle &out)
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
    static bool m_collapseThreeToOne(MeshIndex &index, int tida, int tidb, int &tidc,
	                          Triangle &out)
    {
		std::vector<Triangle> &tris = index.getMesh()->triangles;

		tidc = index.getAdjacencyMap().getCommonAdjacent(tida, tidb);

        if (tidc == -1)
		    return false;

        Line ab, bc, ca;

		tris[tida].getSharedEdge(tris[tidb], ab);
		tris[tidb].getSharedEdge(tris[tidc], bc);
		tris[tidc].getSharedEdge(tris[tida], ca);

		int center = ab.getCommonPoint(bc);

		out = Triangle(center == ab.pa ? ab.pb : ab.pa, 
		               center == bc.pa ? bc.pb : bc.pa, 
					   center == ca.pa ? ca.pb : ca.pa);
	
	    // TODO reverse normal if necessary..
	    return true;
    }
	static void m_collapseAt(MeshIndex &index, std::ostream &log, const std::vector<Triangle> &tris, 
	                  int tid, std::vector<Triangle> &out, std::set<int> &collapsed)
	{
		std::vector<int>   &adja   = index.getAdjacencyMap()[tid];
		std::vector<Point> &points = index.getMesh()->vertices;
		int idb, idc;
		Triangle t;
        for (int j = 0, sz = adja.size(); j < sz; j++)
		{
		    idb = adja[j];
		    std::vector<int> &adjb = index.getAdjacencyMap()[idb];
			if (tid != idb && collapsed.find(idb) == collapsed.end())
			{
			    // zero area?..
			    if (tris[idb].degenerate(points))
				{
				    collapsed.insert(idb);
#ifdef _DOLOG_
log << std::endl << "degenerate ";
tris[tid].print(log, points);
#endif
				}
                // try two-to-one..
				else if (m_collapseTwoToOne(tris[tid], tris[idb], points, t))
			    {
			        out.push_back(t);
				    collapsed.insert(tid);
			   	    collapsed.insert(idb);
#ifdef _DOLOG_
log << std::endl << "collapsing 2-to-1 ";
tris[tid].print(log, points);
tris[idb].print(log, points);
log << " to ";
t.print(log, points);
#endif
			    }
                // try three-to-one..
				else if (m_collapseThreeToOne(index, tid, idb, idc, t))
				{
				    out.push_back(t);
					collapsed.insert(tid);
					collapsed.insert(idb);
					collapsed.insert(idc);
#ifdef _DOLOG_
log << std::endl << "collapsing 3-to-1 ";
tris[tid].print(log, points);
tris[idb].print(log, points);
tris[idc].print(log, points);
log << " to ";
t.print(log, points);
#endif
				}
		    }
		}
	}
    static void m_collapse(MeshIndex &index, std::ostream &log,
	                const std::vector<int> &tids, int start, int end,
                    const std::vector<Triangle> &tris,
					const std::vector<Point> &points, std::vector<Triangle> &out,
					bool returnSimplifiedOnly)
    {
	    std::set<int>    collapsed;
	    int              i, tid;
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
	            m_collapseAt(index, log, tris, tid, out, collapsed);
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

public:

    static void simplify(MeshIndex &index, const std::vector<int> &tlist,
	              std::vector<Triangle> &out, bool returnSimplifiedOnly = false)
    {
        std::ofstream log("scollapse.log", std::ios::app);
        std::set<int> uniq;
		SimpleMesh *mesh = index.getMesh();
	    for (int i = 0, sz = tlist.size(); i < sz; i++)
	    {
    	    if (uniq.find(tlist[i]) == uniq.end())
	    	{
		        std::vector<int> ids;
		        index.getEdgeMap().findAllAdjacent(tlist[i], ids);
                m_collapse(index, log, ids, 0, ids.size(), mesh->triangles, mesh->vertices, out, returnSimplifiedOnly);
			    uniq.insert(tlist[i]);
			    for (int j = 0, jsz = ids.size(); j < jsz; j++)
    			    uniq.insert(ids[j]);
		    }
	    }
        log.close();
    }
};

#endif   /** _SIMPLIFIER_H */