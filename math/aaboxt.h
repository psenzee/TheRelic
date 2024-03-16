#ifndef _AABOXT_H
#define _AABOXT_H

#include "math.h"
#include "vectors3.h"
#include "matrix4t.h"
#include "tuple3t.h"
#include "tuple2t.h"
#include "pointtrianglet.h"
#include "planet.h"

#include <map>
#include <vector>
#include <algorithm>

#include <float.h>

template <class T>
class AABoxT
{
public:

    typedef AABoxT<T>         self;
    typedef Tuple3T<T>        point_t;
    typedef PlaneT<T>         plane_t;
    typedef PointTriangleT<T> triangle_t;

    point_t mn, mx;

    inline AABoxT();
	inline AABoxT(const point_t &a, const point_t &b);
	inline AABoxT(T ax, T ay, T az, T bx, T by, T bz);
    inline AABoxT(const std::vector<point_t> &points);
    inline AABoxT(const AABoxT<T> &box);

    enum PlanePosition { PLANE_LEFT, PLANE_RIGHT, PLANE_LOWER, PLANE_UPPER, PLANE_FRONT, PLANE_BACK, PLANE_COUNT, PLANE_MIN = 0 };

    enum { LEFT = 0, RIGHT = 1, LOWER = 0, UPPER = 2, FRONT = 0, BACK = 4 };

	enum Corner {
	              LLF  = LEFT  | LOWER | FRONT,
	              LLB  = LEFT  | LOWER | BACK,
				  LUF  = LEFT  | UPPER | FRONT,
				  LUB  = LEFT  | UPPER | BACK,
                  RLF  = RIGHT | LOWER | FRONT,
	              RLB  = RIGHT | LOWER | BACK,
				  RUF  = RIGHT | UPPER | FRONT,
				  RUB  = RIGHT | UPPER | BACK,

                  CORNER_COUNT,
                  CORNER_MIN   = 0,
				  CORNER_MASK  = 1 | 2 | 4
				};

    static inline Corner     opposite(Corner c)                                                             { return (Corner)(c ^ MASK); }
	static inline point_t    point   (const point_t &min, const point_t &max, Corner c)                     { return point_t((c & RIGHT) ? max.x : min.x, (c & UPPER) ? max.y : min.y, (c & BACK) ? max.z : min.z); }
	static inline triangle_t triangle(const point_t &min, const point_t &max, Corner a, Corner b, Corner c) { return triangle_t(point(min, max, a), point(min, max, b), point(min, max, c)); }

    inline bool operator==(const AABoxT<T> &other) const { return mn == other.mn && mx == other.mx; }
    inline bool operator!=(const AABoxT<T> &other) const { return mn != other.mn || mx != other.mx; }

    inline point_t size() const { return point_t(mx.x - mn.x, mx.y - mn.y, mx.z - mn.z); }

    inline void insert(const point_t   &p) { mn = math::minpoint(p,    mn); mx = math::maxpoint(p,    mx); }
	inline void insert(const AABoxT<T> &b) { mn = math::minpoint(b.mn, mn); mx = math::maxpoint(b.mx, mx); }
    void        insert(const std::vector<point_t> &points)
	{
        std::vector<point_t>::const_iterator i = points.begin(), e = points.end();
	    for (; i != e; ++i) insert(*i);
	}
    inline Corner intersection(PlanePosition a, PlanePosition b, PlanePosition c) const
	{
	    return (Corner)(m_corner(a) | m_corner(b) | m_corner(c));
	}
	inline point_t center(void) const
	{
        return (mx + mn) / 2;
	}
	inline bool inside(const point_t &p)
	{
        return math::gte(p.x, mn.x) && math::lte(p.x, mn.x) &&
		       math::gte(p.y, mn.y) && math::lte(p.y, mn.y) &&
		       math::gte(p.z, mn.z) && math::lte(p.z, mn.z);
	}
	inline bool valid() const
	{
        return math::lte(mn.x, mx.x) && math::lte(mn.y, mx.y) && math::lte(mn.z, mx.z);
	}
	inline T volume() const
	{
        return (mx.x - mn.x) * (mx.y - mn.y) * (mx.z - mn.z);
	}
    plane_t plane(PlanePosition p) const
	{
		switch (p)
		{
        case PLANE_LEFT:  return plane_t(point(LUB), point(LLB), point(LLF));//plane_t(point(LLF), point(LLB), point(LUB));
        case PLANE_RIGHT: return plane_t(point(RLF), point(RLB), point(RUB));//plane_t(point(RUB), point(RLB), point(RLF));
        default:
		case PLANE_UPPER: return plane_t(point(LUF), point(LUB), point(RUB));//plane_t(point(RUB), point(LUB), point(LUF));
		case PLANE_LOWER: return plane_t(point(RLB), point(LLB), point(LLF));//plane_t(point(LLF), point(LLB), point(RLB));

		case PLANE_FRONT: return plane_t(point(RLF), point(LUF), point(RUF));//plane_t(point(RUF), point(LUF), point(LLF));
		case PLANE_BACK:  return plane_t(point(RUB), point(LUB), point(LLB));//plane_t(point(RLB), point(LUB), point(RUB));
		}
	}
    static point_t facenormal(PlanePosition p)
	{
		switch (p)
		{
        case PLANE_LEFT:  return point_t(-1,  0,  0);
        case PLANE_RIGHT: return point_t( 1,  0,  0);
        default:
		case PLANE_UPPER: return point_t( 0,  1,  0);
		case PLANE_LOWER: return point_t( 0, -1,  0);

		case PLANE_FRONT: return point_t( 0,  0, -1);
		case PLANE_BACK:  return point_t( 0,  0,  1);
		}
	}
	point_t getPlaneCenter(PlanePosition p) const
	{
	    point_t c;
		for (int i = 0; i < 4; i++)
		    c += point(corner(p, i));
		return c / 4;
	}
	PlanePosition getLeastParallel(const plane_t &p) const
	{
        return getLeastParallel(p.normal);
	}
	PlanePosition getLeastParallel(const Tuple3T<T> &normal) const
	{
		PlanePosition mp     = PLANE_UPPER;
		T             m      = FLT_MAX,
		              dot    = 0;
		for (int i = PLANE_MIN; i < PLANE_COUNT; i++)
		{
		    plane_t &pp = plane((PlanePosition)i);
			dot = normal.dot(pp.normal);
			if (dot < m) { m = dot; mp = (PlanePosition)i; }
		}
		return mp;
	}
    Tuple2T<T> get2dPoint(PlanePosition p, const Tuple3T<T> &t) const
	{
		switch (p)
		{
		default:
		case PLANE_LEFT:  case PLANE_RIGHT: return Tuple2T<T>(t.y, t.z);
		case PLANE_LOWER: case PLANE_UPPER: return Tuple2T<T>(t.x, t.z);
		case PLANE_FRONT: case PLANE_BACK:  return Tuple2T<T>(t.x, t.y);
		}
	}
	void set2dPoint(PlanePosition p, Tuple3T<T> &t3, const Tuple2T<T> &t2) const
	{
		switch (p)
		{
		default:
		case PLANE_LEFT:  case PLANE_RIGHT: t3.y = t2.x; t3.z = t2.y; break;
		case PLANE_LOWER: case PLANE_UPPER: t3.x = t2.x; t3.z = t2.y; break;
		case PLANE_FRONT: case PLANE_BACK:  t3.x = t2.x; t3.z = t2.y; break;
		}
	}
	PlanePosition getMostParallel(const plane_t &p) const
	{
        return getMostParallel(p.normal);
	}
	PlanePosition getMostParallel(const Tuple3T<T> &normal) const
	{
		PlanePosition mp     = PLANE_UPPER;
		T             m      = -FLT_MAX,
		              dot    = 0;
		for (int i = PLANE_MIN; i < PLANE_COUNT; i++)
		{
		    plane_t &pp = plane((PlanePosition)i);
			dot = normal.dot(pp.normal);
			if (dot > m) { m = dot; mp = (PlanePosition)i; }
		}
		return mp;
	}
	inline plane_t getAdjacentPlane(PlanePosition p, int index) const
	{
        return plane(getAdjacentPlanePosition(p, index));
	}
	PlanePosition getAdjacentPlanePosition(PlanePosition p, int index) const
	{
        static const PlanePosition ar[] = {
											PLANE_UPPER, PLANE_BACK,  PLANE_LOWER, PLANE_FRONT,
		                                    PLANE_UPPER, PLANE_FRONT, PLANE_LOWER, PLANE_BACK,
									        PLANE_BACK,  PLANE_RIGHT, PLANE_FRONT, PLANE_LEFT,
						                    PLANE_BACK,  PLANE_LEFT,  PLANE_FRONT, PLANE_RIGHT,
											PLANE_UPPER, PLANE_LEFT,  PLANE_LOWER, PLANE_RIGHT,
									        PLANE_UPPER, PLANE_RIGHT, PLANE_LOWER, PLANE_LEFT   };

        return ar[(int)p * 4 + m_mod(index, 4)];
	}
    inline Corner corner(PlanePosition p, int i) const
    {
	    return intersection(p, getAdjacentPlanePosition(p, i - 1), getAdjacentPlanePosition(p, i));
    }
    void getPlaneIntersection(const plane_t &p, std::vector<point_t> &points) const
    {
        PlanePosition mp = getMostParallel(p);
		point_t       at;
		for (int i = 0; i < 4; i++)
            if (getAdjacentPlane(mp, i - 1).intersect(getAdjacentPlane(mp, i), p, at))
			    points.push_back(at);
    }
	void getAdjacentPlanes(PlanePosition p, std::vector<plane_t> &ps) const
	{
	    for (int i = 0; i < 4; i++)
		    ps.push_back(getAdjacentPlane(p, i));
	}
	inline T getLargestDimension(void) const
	{
	    T v = 0;
	    if (mx.x - mn.x > v) v = mx.x - mn.x;
	    if (mx.y - mn.y > v) v = mx.y - mn.y;
	    if (mx.z - mn.z > v) v = mx.z - mn.z;
        return v;
	}

    inline point_t    getQuadPoint(PlanePosition p, int id, bool flip = false)  const { return point(corner(p, flip ? (id % 4) : 3 - (id % 4))); }

	inline point_t    point(Corner c)                                           const { return point(mn, mx, c); }
	inline triangle_t triangle(Corner a, Corner b, Corner c, bool flip = false) const { return triangle(mn, mx, flip ? c : a, b, flip ? a : c); }
	inline void       transform(const Matrix4T<T> &m, std::vector<point_t> &points) const
	{
	    for (int i = CORNER_MIN; i < CORNER_COUNT; i++)
		    points.push_back(m.point(point((Corner)i)));
	}

    inline void triangulate(PlanePosition p, std::vector<triangle_t> &tris, bool flip = false) const
	{
	    tris.push_back(triangle(corner(p, 0), corner(p, 1), corner(p, 2), flip));
	    tris.push_back(triangle(corner(p, 0), corner(p, 2), corner(p, 3), flip));
	}
	inline void quad(PlanePosition p, std::vector<point_t> &points, bool flip = false) const
	{
	    points.push_back(getQuadPoint(p, 0, flip)); points.push_back(getQuadPoint(p, 1, flip));
		points.push_back(getQuadPoint(p, 2, flip)); points.push_back(getQuadPoint(p, 3, flip));
	}
	void triangulate(std::vector<triangle_t> &tris, bool flip = false) const
	{
	    for (int i = PLANE_MIN; i < PLANE_COUNT; i++)
	        triangulate((PlanePosition)i, tris, flip);
	}
	void quad(std::vector<point_t> &points, bool flip = false) const
	{
	    for (int i = PLANE_MIN; i < PLANE_COUNT; i++)
	        quad((PlanePosition)i, points, flip);
	}
	inline void enlarge(const point_t &p)
	{
	    point_t q = p / 2;
	    mn -= q; mx += q;
	}
    inline void resize(const point_t &r)
    {
        enlarge((mx - mn) * (r - 1));
    }
    inline void resize(const T &r)
    {
        enlarge((mx - mn) * (r - 1));
    }
	inline void scale(const point_t &p)
	{
        mn *= p; mx *= p;
	}
    self transform(const Matrix4T<T> &m) const
    {
        self nb;
        for (int i = CORNER_MIN; i < CORNER_COUNT; i++)
            nb.insert(m.point(point((Corner)i)));
        return nb;
    }

private:

    inline int m_corner(PlanePosition a) const
	{
        static const int m[] = { LEFT, RIGHT, LOWER, UPPER, FRONT, BACK };
		return m[a];
	}
	inline int m_mod(int v, int m) const
	{
	    while (v < 0) v += m;
		return v % m;
	}
};

//typedef AABoxT<float> AABox;

// template implementation

template <class T> inline AABoxT<T>::AABoxT()                                   : mn(FLT_MAX, FLT_MAX, FLT_MAX), mx(-FLT_MAX, -FLT_MAX, -FLT_MAX) {}
template <class T> inline AABoxT<T>::AABoxT(const point_t &a, const point_t &b) : mn(math::minpoint(a, b)), mx(math::maxpoint(a, b)) {}
template <class T> inline AABoxT<T>::AABoxT(T ax, T ay, T az, T bx, T by, T bz) : mn(math::minpoint(point_t(ax, ay, az), point_t(bx, by, bz))), mx(math::maxpoint(point_t(ax, ay, az), point_t(bx, by, bz))) {}
template <class T> inline AABoxT<T>::AABoxT(const std::vector<point_t> &points) : mn(FLT_MAX, FLT_MAX, FLT_MAX), mx(-FLT_MAX, -FLT_MAX, -FLT_MAX) { insert(points); }
template <class T> inline AABoxT<T>::AABoxT(const AABoxT<T> &box)               : mn(box.mn), mx(box.mx) {}

#endif  /** _AABOXT_H */