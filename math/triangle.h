#ifndef _TRIANGLE_H
#define _TRIANGLE_H

#include "math/tuple3t.h"
#include "math/planet.h"
#include "math/pointtrianglet.h"

#include "line.h"

#include <vector>

class Triangle
{
public:

    enum Connection      { CONN_AB  = 0, CONN_BC, CONN_CA, CONN_NONE };
	enum EndPoint        { EP_START = 0, EP_END, EP_OTHER            };
	enum PointIdentifier { PID_A    = 0, PID_B, PID_C, PID_NONE = -1 };

    union
	{
        struct { int pa, pb, pc; };
	    int point[3];
	};

    inline Triangle()                       : pa(0),  pb(0),  pc(0)  {}
    inline Triangle(int pa, int pb, int pc) : pa(pa), pb(pb), pc(pc) {}

    inline void set(int a, int b, int c) { pa = a; pb = b; pc = c; }

    inline bool operator==(const Triangle &t) const
	{
	    // order matters here (for eventual culling)..
		return pa == t.pa && pb == t.pb && pc == t.pc;
	}
    inline bool operator<(const Triangle &t) const
	{
	    // order matters here (for eventual culling)..
	    if      (pa <  t.pa) return true;
		else if (pa >  t.pa) return false;

	    if      (pb <  t.pb) return true;
		else if (pb >  t.pb) return false;

	    if      (pc <  t.pc) return true;
		
		return false;
	}
	inline std::ostream &print(std::ostream &s, const std::vector< Tuple3T<float> > &points) const
	{
	    return s << "[tri a " << pa << ":" << points[pa] << " " <<
                         "b " << pb << ":" << points[pb] << " " <<  
                         "c " << pc << ":" << points[pc] << "]";
	}
	inline Tuple3T<float> normal(const std::vector< Tuple3T<float> > &points) const
	{
        return (points[pb] - points[pa]).cross(points[pc] - points[pa]).normalize();
	}
	inline PlaneT<float> plane(const std::vector< Tuple3T<float> > &points) const
	{
        return PlaneT<float>(points[pa], points[pb], points[pc]);
	}
	inline bool inside(const Tuple3T<float> &p, const std::vector< Tuple3T<float> > &points) const
	{
		return sameside(p, points[pa], points[pb], points[pc]) && 
		       sameside(p, points[pb], points[pa], points[pc]) && 
			   sameside(p, points[pc], points[pa], points[pb]);
	}
	inline bool sameside(const Tuple3T<float> &p1, const Tuple3T<float> &p2, const Tuple3T<float> &a, const Tuple3T<float> &b) const
	{
		Tuple3T<float> b_a(b - a);
		return (b_a.cross(p1 - a)).dot((b_a.cross(p2 - a))) > 0;
	}
	inline Connection connection(const Line &line) const
	{
	    if      ((line.pa == pa && line.pb == pb) || (line.pb == pa && line.pa == pb)) return CONN_AB;
		else if ((line.pa == pb && line.pb == pc) || (line.pb == pb && line.pa == pc)) return CONN_BC;
		else if ((line.pa == pc && line.pb == pa) || (line.pb == pc && line.pa == pa)) return CONN_CA;
		else                                                                           return CONN_NONE;
	}
	inline Line edge(Connection c) const
	{
	    return Line(getPoint(c, EP_START), getPoint(c, EP_END));
	}
	inline Line &edge(Connection c, Line &ln) const
	{
	    ln.set(getPoint(c, EP_START), getPoint(c, EP_END));
		return ln;
	}
	inline int getPoint(Connection c, EndPoint end) const
	{
		return c == CONN_NONE ? -1 : point[(c + (int)end) % 3];
	}
	inline int getPoint(PointIdentifier pi) const
	{
        return point[pi];
	}
	static inline PointIdentifier getPointId(Connection c, EndPoint end)
	{
		return c == CONN_NONE ? PID_NONE : (PointIdentifier)((c + (int)end) % 3);
	}
	inline PointIdentifier getPointId(int p) const
	{
	    if      (point[PID_A] == p) return PID_A;
	    else if (point[PID_B] == p) return PID_B;
	    else if (point[PID_C] == p) return PID_C;
		else                        return PID_NONE;
	}
	inline bool equals(Connection c, int start, int end) const
	{
	    int p0 = getPoint(c, EP_START), p1 = getPoint(c, EP_END);
	    return (p0 == start && p1 == end) || (p0 == end && p1 == start);
	}
	inline bool equals(Connection c, const Triangle &t, Connection tc) const
	{
	    return equals(c, t.getPoint(tc, EP_START), t.getPoint(tc, EP_END));
	}
	inline bool getSharedEdge(const Triangle &t, Line &ln) const
	{
		Connection c = getSharedEdgeConnection(t);
		if (c != CONN_NONE) ln = edge(c);
        return c != CONN_NONE;
	}
	inline Connection getSharedEdgeConnection(const Triangle &t) const
	{
	    Connection throwaway;
		return getSharedEdgeConnection(t, throwaway);
	}
	inline Connection getSharedEdgeConnection(const Triangle &t, Connection &tc) const
	{
	    Line ln;
		if      ((tc = t.connection(edge(CONN_AB, ln))) != CONN_NONE) return CONN_AB;
		else if ((tc = t.connection(edge(CONN_BC, ln))) != CONN_NONE) return CONN_BC;
		else if ((tc = t.connection(edge(CONN_CA, ln))) != CONN_NONE) return CONN_CA;
		else                                                          return CONN_NONE;
	}
	inline bool degenerate(const std::vector< Tuple3T<float> > &pts) const
	{
	    return pa == pb || pb == pc || pc == pa ||
		       pts[pa] == pts[pb] || pts[pb] == pts[pc] ||
			   pts[pc] == pts[pa];
	}
	inline int getOtherPoint(const Line &line) const
	{
	    return getPoint(connection(line), EP_OTHER);
	}
};

#endif  /** _TRIANGLE_H */
