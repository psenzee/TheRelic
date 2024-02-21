#ifndef _PLANE_H
#define _PLANE_H

#include "point.h"
#include "pointline.h"

class Plane
{
public:

    Point  normal;
	double distance;

    inline Plane() : normal(0, 1, 0), distance(0) {}
    inline Plane(const Point &a, const Point &b, const Point &c)
	{
        normal   = (b - a).cross(c - a).normalize();
        distance = normal.dot(a);
	}
	inline bool operator<(const Plane &p) const
	{
	    if (normal < p.normal) return true;
		if (p.normal < normal) return false;

		double d = distance - p.distance;

	    if      (d < -EPSILON) return true;
		else if (d >  EPSILON) return false;

        return false; // equal
	}
    inline bool inHalfSpace(const Point &p) const
	{
		return normal.dot(p) >= distance;
	}
    inline void flip(void)
	{
        normal   = Point(-normal.x, -normal.y, -normal.z);
        distance = -distance;
	}
	bool intersects(PointLine &at, const Plane &other) const
	{
        if (this == &other || normal == other.normal)
		    return false;

		double fn00 = normal.length(),
		       fn01 = normal.dot(other.normal),
		       fn11 = other.normal.length(),
		       det  = fn00 * fn11 - fn01 * fn01;
		           
		if (det > -EPSILON && det < EPSILON)
			return false;
			
        det = 1 / det;
		double fc0 = (fn11 * -distance       + fn01 * other.distance) * det,
		       fc1 = (fn00 * -other.distance + fn01 * distance)       * det;
		at.a = normal * fc0 + other.normal * fc1;
		at.b = at.a + normal.cross(other.normal);
		return true;
	}
	bool intersectsLine(Point &at, const PointLine &a) const
	{
	    double tt;
		return m_intersects(at, tt, a);
	}
	bool intersectsSegment(Point &at, const PointLine &a) const
	{
	    double tt;
		bool b = m_intersects(at, tt, a);
		return b && tt <= 1 && tt >= 0;
	}
	bool intersectsRay(Point &at, const PointLine &a) const
	{
	    double tt;
		bool b = m_intersects(at, tt, a);
		return b && tt >= 0;
	}

private:

    bool m_intersects(Point &at, double &tt, const PointLine &line) const
	{
        Point  dir    = line.direction(),
		       n      = normal;
		double d      = 0,
		       rate   = dir.dot(n),
               t      = 0;

		if (*(int *)&rate == 0)
			return false;

        d = -distance;
		t = -(d + line.a.dot(n));
		if (t >= -EPSILON && t <= EPSILON)
		    return false;
        t /= rate;
        at = Point(line.a.x + dir.x * t, line.a.y + dir.y * t, line.a.z + dir.z * t);
        tt = (at.x - line.a.x) / (line.b.x - line.a.x);
		return true;
	}
};

#endif  /** _PLANE_H */
