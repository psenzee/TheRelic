#ifndef _POINTLINET_H
#define _POINTLINET_H

#include "tuple3t.h"

template <class T>
class PointLineT
{
public:

    typedef PointLineT<T> self;
    typedef Tuple3T<T>    point_t;

    point_t a, b;

    inline PointLineT() {}
	inline PointLineT(const point_t &a, const point_t &b) : a(a), b(b) {}

	inline void    set(const point_t &a, const point_t &b)       { this->a = a; this->b = b; }
    inline point_t normal(void)                            const { return (b - a).normalize(); }
	inline point_t direction(void)                         const { return b - a; }

	inline const point_t &minimum(void) const { return a < b ? a : b; }
	inline const point_t &maximum(void) const { return a < b ? b : a; }

	inline bool operator<(const self &p) const
	{
	    if (minimum() < p.minimum()) return true;
		if (p.minimum() < minimum()) return false;
		return maximum() < p.maximum();
	}

    inline bool perpendicular(const self &other, const T eps = math::EPSILON)
    {
	    return  math::zero(normal().dot(other.normal()));
    }
    inline bool parallel(const self &other, const T eps = math::EPSILON)
    {
	    return  math::zero(normal().dot(other.normal()) - (T)1.0);
    }
    inline std::ostream &print(std::ostream &s) const
    {
        return s << "[PointLine " << a << "-" << b << "]";
    }
};

typedef PointLineT<float> PointLine;

inline std::ostream &operator<<(std::ostream &s, const PointLine &p)
{
    return p.print(s);
}

#endif  // _POINTLINET_H
