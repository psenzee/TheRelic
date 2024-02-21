#ifndef _VECTORS3_H
#define _VECTORS3_H

#include "math.h"

#include <vector>

#include <float.h>

namespace math
{

template<class T, class VecT> inline T      lengthsq(const VecT &a)                      { return a.x * a.x + a.y * a.y + a.z * a.z; }
template<class T, class VecT> inline T      length(const VecT &a)                        { return (T)sqrt(lengthsq<T, VecT>(a)); }
template<class T, class VecT> inline bool   equal(const VecT &a, const VecT &b)          { return eq<T>(a.x, b.x) && eq<T>(a.y, b.y) && eq<T>(a.z, b.z); }
template<class T, class VecT> inline bool   equal(const VecT &v, T x, T y, T z)          { return eq<T>(v.x, x) && eq<T>(v.y, y) && eq<T>(v.z, z); }
template<class T, class VecT> inline T      dot(const VecT &a, const VecT &b)            { return a.x * b.x + a.y * b.y + a.z * b.z; }
template<class VecT>          inline void   cross(const VecT &a, const VecT &b, VecT &r) { r.x = a.y * b.z - a.z * b.y; r.y = a.z * b.x - a.x * b.z; r.z = a.x * b.y - a.y * b.x; }
template<class T, class VecT> inline void   normalize(VecT &n)                           { T d = (T)1.0 / length<T, VecT>(n); n.x *= d; n.y *= d; n.z *= d; }

template<class VecT>          inline void   sub(VecT &a, const VecT &b)                  { a.x -= b.x; a.y -= b.y; a.z -= b.z; }
template<class VecT>          inline void   add(VecT &a, const VecT &b)                  { a.x += b.x; a.y += b.y; a.z += b.z; }
template<class VecT>          inline void   mul(VecT &a, const VecT &b)                  { a.x *= b.x; a.y *= b.y; a.z *= b.z; }
template<class VecT>          inline void   div(VecT &a, const VecT &b)                  { a.x /= b.x; a.y /= b.y; a.z /= b.z; }

template<class T, class VecT> inline void   subscalar(VecT &a, const T &b)               { a.x -= b; a.y -= b; a.z -= b; }
template<class T, class VecT> inline void   addscalar(VecT &a, const T &b)               { a.x += b; a.y += b; a.z += b; }
template<class T, class VecT> inline void   mulscalar(VecT &a, const T &b)               { a.x *= b; a.y *= b; a.z *= b; }
template<class T, class VecT> inline void   divscalar(VecT &a, const T &b)               { a.x /= b; a.y /= b; a.z /= b; }

template<class T>             inline void   add3(const T *a, const T *b, T *r)           { r[0] = a[0] + b[0]; r[1] = a[1] + b[1]; r[2] = a[2] + b[2]; }
template<class T>             inline void   sub3(const T *a, const T *b, T *r)           { r[0] = a[0] - b[0]; r[1] = a[1] - b[1]; r[2] = a[2] - b[2]; }
template<class T>             inline void   mul3(const T *a, const T *b, T *r)           { r[0] = a[0] * b[0]; r[1] = a[1] * b[1]; r[2] = a[2] * b[2]; }
template<class T>             inline void   div3(const T *a, const T *b, T *r)           { r[0] = a[0] / b[0]; r[1] = a[1] / b[1]; r[2] = a[2] / b[2]; }
template<class T>             inline void   mul3(const T *a, T s, T *r)                  {  r[0] = a[0] * s; r[1] = a[1] * s; r[2] = a[2] * s; }
template<class T>             inline void   div3(const T *a, T s, T *r)                  { mul3(a, 1 / s, r); }
template<class T>             inline void   norm3(const T *v, T *r)                      { T s = (T)sqrt((double)dot3(v, v)); div3(v, s, r); }
template<class T>             inline T      dot3(const T *v, const T *u)                 { return v[0] * u[0] + v[1] * u[1] + v[2] * u[2]; }
template<class T>             inline void   cross3(const T *a, const T *b, T *r)         { r[0] = a[1] * b[2] - a[2] * b[1]; r[1] = a[2] * b[0] - a[0] * b[2]; r[2] = a[0] * b[1] - a[1] * b[0]; }

template<class VecT>
inline void facedirection(const VecT &a, const VecT &b, const VecT &c, VecT &r)
{
    VecT ba(b.x, b.y, b.z), ca(c.x, c.y, c.z);
	sub<VecT>(ba, a); sub<VecT>(ca, a);
    cross<VecT>(ba, ca, r);
}

template<class T, class VecT>
inline void facenormal(const VecT &a, const VecT &b, const VecT &c, VecT &r)
{
    facedirection<VecT>(a, b, c, r); normalize<T, VecT>(r);
}

template<class VecT> inline bool less(const VecT &a, const VecT &b)
{
    if (lt(a.x, b.x)) return true; else if (gt(a.x, b.x)) return false;
    if (lt(a.y, b.y)) return true; else if (gt(a.y, b.y)) return false;
    if (lt(a.z, b.z)) return true; else                   return false;
}

template<class VecT> inline bool greater(const VecT &a, const VecT &b)
{
    if (gt(a.x, b.x)) return true; else if (lt(a.x, b.x)) return false;
    if (gt(a.y, b.y)) return true; else if (lt(a.y, b.y)) return false;
    if (gt(a.z, b.z)) return true; else                   return false;
}

/*
template <class T, class VecT>
inline T area(const VecT &a, const VecT &b, const VecT &c)
{
	//VecT ca(c - a), cb(c - b);
	VecT ca(c.x, c.y, c.z), cb(c.x, c.y, c.z);

	sub<VecT>(ca, a); sub<VecT>(cb, b);

	T lnca = length<T, VecT>(ca),
	  lncb = length<T, VecT>(cb),
	  w    = 0;

	divscalar<T, VecT>(ca, lnca);
	divscalar<T, VecT>(cb, lncb);

    w = dot<T, VecT>(ca, cb);

	return (T)0.5 * lnca * lncb * //sin(acos(w));
		                            sqrt(1 - w * w); // identity..
}
*/

template <class T, class VecT>
inline T area(const VecT &a, const VecT &b, const VecT &c)
{
	VecT ca(c.x, c.y, c.z), cb(c.x, c.y, c.z), r;
	sub<VecT>(ca, a); sub<VecT>(cb, b);

    cross<VecT>(ca, cb, r);
    return length<float, VecT>(r) / 2;
}

template <class T, class VecT>
int maxarea(const VecT &a, const VecT &b, const std::vector<VecT> &vs)
{
	T      dm = 0, f = 0;
	VecT   dp;
	int	   mx = -1;
	for (int i = 0, sz = vs.size(); i < sz; i++)
	{
		f = area<T, VecT>(a, b, vs[i]);
		if (f > dm) { mx = i; dm = f; }
	}
	return mx;
}

template <class T, class VecT>
int furthest(const VecT &a, const std::vector<VecT> &vs)
{
	T      dm = 0, f = 0;
	VecT   dp;
	int	   mx = -1;
	for (int i = 0, sz = vs.size(); i < sz; i++)
	{
		dp = vs[i] - a;
		f = dp.x * dp.x + dp.y * dp.y + dp.z * dp.z;
		if (f > dm) { mx = i; dm = f; }
	}
	return mx;
}

template <class VecT>
VecT average(const std::vector<VecT> &vs)
{
	VecT a(0, 0, 0);
	if (vs.size() > 0)
	{
	    for (int i = 0, sz = vs.size(); i < sz; i++)
	        { a.x += vs[i].x; a.y += vs[i].y; a.z += vs[i].z; }
        a.x /= vs.size(); a.y /= vs.size(); a.z /= vs.size();
	}
	return a;
}

template <class T, class VecT>
void random(int count, const VecT &scale, const VecT &trans, std::vector<VecT> &ps)
{
    for (int i = 0; i < count; i++)
	    ps.push_back(Point(((T)rand() / RAND_MAX) * scale.x + trans.x,
		                   ((T)rand() / RAND_MAX) * scale.y + trans.y,
						   ((T)rand() / RAND_MAX) * scale.z + trans.z));
}

enum PlaneRelation { INPOS, INNEG, INBOTH, ON, ONNEG, ONBOTH };

inline const char *relation(PlaneRelation r)
{
    switch (r)
	{
	case INNEG:   return "INNEG";
	case INPOS:   return "INPOS";
	case INBOTH:  return "INBOTH";
	case ON:      return "ON";
	case ONNEG:   return "ONNEG";
	case ONBOTH:  return "ONBOTH";
	}
	return "UNKNOWN";
}

template <class VecT>
inline VecT minpoint(const VecT &a, const VecT &b)
{
    return VecT(a.x < b.x ? a.x : b.x, a.y < b.y ? a.y : b.y, a.z < b.z ? a.z : b.z);
}

template <class VecT>
inline VecT maxpoint(const VecT &a, const VecT &b)
{
    return VecT(a.x > b.x ? a.x : b.x, a.y > b.y ? a.y : b.y, a.z > b.z ? a.z : b.z);
}

template <class VecT, class IterT, class VecFuncT>
void extremes(IterT begin, IterT end, VecT &minp, VecT &maxp)
{
    IterT i = begin, e = end;
	VecFuncT vf;
    VecT minq(FLT_MAX, FLT_MAX, FLT_MAX), maxq(-FLT_MAX, -FLT_MAX, -FLT_MAX);
    for (; i != e; ++i)
    {
        minq = minpoint<VecT>(vf(*i), minq);
		maxq = maxpoint<VecT>(vf(*i), maxq);
    }
	minp = minq; maxp = maxq;
}

};

#endif  /** _VECTORS3_H */