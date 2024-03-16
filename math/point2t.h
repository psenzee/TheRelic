#ifndef _POINT2T_H
#define _POINT2T_H

#include "io/streamst.h"

template <typename T>
class Point2T
{
public:

    T x, y;

    typedef Point2T<T> self;

    inline Point2T()              : x(0), y(0)     {}
    inline Point2T(T x, T y)      : x(x), y(y)     {}
    inline Point2T(const self &p) : x(p.x), y(p.y) {}

	inline int area(void) const { return (int)x * (int)y; }

    inline self &operator-=(const self &p) { x -= p.x; y -= p.y; return *this; }
    inline self  operator- (const self &p) { return self(x - p.x, y - p.y); }
    inline self &operator+=(const self &p) { x += p.x; y += p.y; return *this; }
    inline self  operator+ (const self &p) { return self(x + p.x, y + p.y); }
    inline self &operator*=(const self &p) { x *= p.x; y *= p.y; return *this; }
    inline self  operator* (const self &p) { return self(x * p.x, y * p.y); }
    inline self &operator/=(const self &p) { x /= p.x; y /= p.y; return *this; }
    inline self  operator/ (const self &p) { return self(x / p.x, y / p.y); }

	inline void read(std::istream &s)
	{
		x = StreamsT<T>::read(s);
		y = StreamsT<T>::read(s);
	}
	inline void write(std::ostream &s) const
	{
		StreamsT<T>::write(s, x);
		StreamsT<T>::write(s, y);
    }

    // utilities..
    inline static self minimum(const self &a, const self &b) { return self(a.x < b.x ? a.x : b.x, a.y < b.y ? a.y : b.y); }
    inline static self maximum(const self &a, const self &b) { return self(a.x > b.x ? a.x : b.x, a.y > b.y ? a.y : b.y); }
};

#endif  /** _POINT2T_H */