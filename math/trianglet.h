#ifndef _TRIANGLET_H
#define _TRIANGLET_H

template <typename T, typename PointPolicyT>
class TriangleT
{
public:

    typedef TriangleT<T, PointPolicyT> self;
    typedef T                          point_t;
    typedef PointPolicyT               policy_t;

    point_t a, b, c;

    inline TriangleT() {}
    inline TriangleT(const T &a, const T &b, const T &c) : a(a), b(b), c(c) {}

    inline typename PointPolicyT::value_type area(void) { return (policy_t::position(c) - policy_t::position(a)).cross(policy_t::position(c) - policy_t::position(b)).length() / 2; }
};

#include <iostream>

template <typename T, typename PointPolicyT>
inline std::ostream &operator<<(std::ostream &s, const TriangleT<T, PointPolicyT> &p) { return s << "[tri a " << a << " b " << b << " c " << c << "]"; }

#include <vector>
#include "matrix4t.h"

template <typename T, typename PointPolicyT>
inline TriangleT<T, PointPolicyT> &operator*=(TriangleT<T, PointPolicyT> &t, const Matrix4T<T> &m) { t.a = policy_t::position(t.a) * m; t.b = policy_t::position(t.b) * m; t.c = policy_t::position(t.c) * m; return t; }

template <typename T, typename PointPolicyT>
inline std::vector< TriangleT<T, PointPolicyT> > &operator*=(std::vector< TriangleT<T, PointPolicyT> > &v, const Matrix4T<T> &m)
{
    std::vector< TriangleT<T, PointPolicyT> >::iterator i = v.begin(), e = v.end();
	for (; i != e; ++i) *i *= m;
	return v;
}

#endif  /** _TRIANGLET_H */
