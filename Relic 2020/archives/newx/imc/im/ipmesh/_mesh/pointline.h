#ifndef _POINTLINE_H
#define _POINTLINE_H

#include "point.h"

class PointLine
{
public:

    Point a,
	      b;

    inline PointLine() {}
	inline PointLine(const Point &a, const Point &b) : a(a), b(b) {}

	inline void  set(const Point &a, const Point &b) { this->a = a; this->b = b; }
    inline Point normal(void)    const { return (b - a).normalize(); }
	inline Point direction(void) const { return b - a; }

    inline bool perpendicular(const PointLine &other, const double epsilon = EPSILON)
    {
        Point  d0  = normal(),
	           d1  = normal();
        double dot = d0.dot(d1);
	    return dot > -epsilon && dot < epsilon;
    }
    inline bool parallel(const PointLine &other, const double epsilon = EPSILON)
    {
        Point  d0  = normal(),
	           d1  = normal();
        double dot = d0.dot(d1) - 1.0;
	    return dot > -epsilon && dot < epsilon;
    }
};

inline std::ostream &operator<<(std::ostream &s, const PointLine &p)
{
    return s << "[PointLine " << p.a << "-" << p.b << "]";
}

#endif  /** _POINTLINE_H */
