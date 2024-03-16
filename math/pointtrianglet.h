#ifndef _POINTTRIANGLET_H
#define _POINTTRIANGLET_H

#include "tuple3t.h"

template <class T>
class PointTriangleT
{
public:

    typedef PointTriangleT<T> self;
    typedef Tuple3T<T>        point_t;

    point_t a, b, c;

    inline PointTriangleT(const point_t &a, const point_t &b, const point_t &c) : a(a), b(b), c(c) {}

    inline T area(void) { return (c - a).cross(c - b).length() / 2; }
};

typedef PointTriangleT<float> PointTriangle;

#include <iostream>

template <class T>
inline std::ostream &operator<<(std::ostream &s, const PointTriangleT<T> &p) { return s << "[tri a " << a << " b " << b << " c " << c << "]"; }

#include <vector>
#include "matrix4t.h"

template <class T>
inline PointTriangleT<T> &operator*=(PointTriangleT<T> &t, const Matrix4T<T> &m) { t.a *= m; t.b *= m; t.c *= m; return t; }

template <class T>
inline std::vector< PointTriangleT<T> > &operator*=(std::vector< PointTriangleT<T> > &v, const Matrix4T<T> &m)
{
    std::vector< PointTriangleT<T> >::iterator i = v.begin(), e = v.end();
	for (; i != e; ++i) *i *= m;
	return v;
}

#endif  /** _POINTTRIANGLET_H */
