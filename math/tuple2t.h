#ifndef _TUPLE2T_H
#define _TUPLE2T_H

#include "math.h"
#include "vectors3.h"

#include <iostream>

template <class T>
class Tuple2T
{
public:

    typedef Tuple2T<T> self;

    union
    {
        struct { T x, y; };
        T data[2];
    };

    inline Tuple2T(T x = 0, T y = 0) : x(x), y(y) {}

	inline T      cross(const self &p) const { return x * p.y - y * p.x; }
	inline T      dot(const self &p)   const { return x * p.x + y * p.y; }

    inline bool          operator==(const self &p)                         const { return math::eq(x, p.x) && math::eq(y, p.y); }
    inline bool          operator==(const T &p)                            const { return math::eq(x, p)   && math::eq(y, p);   }
	inline bool          operator> (const self &p)                         const { return m_gt(p);  }
	inline bool          operator< (const self &p)                         const { return m_lt(p);  }
	inline bool          operator>=(const self &p)                         const { return m_gte(p); }
	inline bool          operator<=(const self &p)                         const { return m_lte(p); }
	inline bool          operator!=(const self &p)                         const { return !math::eq(x, p.x) || !math::eq(y, p.y); }
	inline bool          operator!=(const T &p)                            const { return !math::eq(x, p)   || !math::eq(y, p);   }
    inline self         &operator= (T u)                                         { x = y = u; return *this; }
    inline self         &operator= (const self &p)                               { x = p.x; y = p.y; return *this; }
    inline self         &operator= (const std::basic_string<float> &p)           { int s = p.size(); y = s > 1 ? p[1] : 0; x = s > 0 ? p[0] : 0; return *this; }
//  inline self         &operator= (const std::basic_string<T> &p)               { int s = p.size(); y = s > 1 ? p[1] : 0; x = s > 0 ? p[0] : 0; return *this; }
	inline self         &operator= (const std::vector<T> &p)                     { int s = p.size(); y = s > 1 ? p[1] : 0; x = s > 0 ? p[0] : 0; return *this; }

	inline self operator-(const self &p)  const	{ return self(x - p.x, y - p.y); }
	inline self operator*(const self &p)  const { return self(x * p.x, y * p.y); }
	inline self operator+(const self &p)  const { return self(x + p.x, y + p.y); }
	inline self operator/(const self &p)  const { return self(x / p.x, y / p.y); }

	inline self &operator-=(const self &p)      { x -= p.x; y -= p.y; return *this; }
	inline self &operator*=(const self &p)      { x *= p.x; y *= p.y; return *this; }
	inline self &operator+=(const self &p)      { x += p.x; y += p.y; return *this; }
	inline self &operator/=(const self &p)      { x /= p.x; y /= p.y; return *this; }

	inline self operator-(const T &p)     const	{ return self(x - p, y - p); }
	inline self operator*(const T &p)     const { return self(x * p, y * p); }
	inline self operator+(const T &p)     const { return self(x + p, y + p); }
	inline self operator/(const T &p)     const { return self(x / p, y / p); }

	inline self &operator-=(const T &p)         { x -= p; y -= p; return *this; }
	inline self &operator*=(const T &p)         { x *= p; y *= p; return *this; }
	inline self &operator+=(const T &p)         { x += p; y += p; return *this; }
	inline self &operator/=(const T &p)         { x /= p; y /= p; return *this; }

	inline T lengthsq()                   const { return x * x + y * y; }
	inline T length()                     const { return (T)sqrt(lengthsq()); }
	inline T distancesq(const self &p)    const { T dx = p.x - x, dy = p.y - y; return dx * dx + dy * dy; }
	inline T distance(const self &p)      const { return (T)sqrt(distancesq(p)); }

	inline void set(T x, T y, T z)              { this->x = x; this->y = y; }

	inline self &normalize()
	{
        T d = length();
		if (d != 0) { x /= d; y /= d; }
		return *this;
	}
	inline std::ostream &print(std::ostream &s) const
	{
	    return s << "{" << x << ", " << y << "}";
	}
    inline std::ostream &writexml(std::ostream &s, int depth) const
	{
	    return TextStreams::printfi(s, "  ", depth, "<point x=\"%.3f\" y=\"%.3f\" />\n", x, y);
    }

private:

    inline bool          m_lt(const self &t)  const;
    inline bool          m_lte(const self &t) const;
    inline bool          m_gt(const self &t)  const;
    inline bool          m_gte(const self &t) const;
};

inline std::ostream &operator<<(std::ostream &s, const Tuple2T<float> &p)
{
    return p.print(s);
}

inline std::ostream &operator<<(std::ostream &s, const Tuple2T<double> &p)
{
    return p.print(s);
}

template <class T>
bool Tuple2T<T>::m_lt(const self &t) const
{
    if (math::lt (x, t.x)) return true; else if (math::gt(x, t.x)) return false;
    if (math::lt (y, t.y)) return true; else                       return false;
}

template <class T>
bool Tuple2T<T>::m_lte(const self &t) const
{
    if (math::lt (x, t.x)) return true; else if (math::gt(x, t.x)) return false;
    if (math::lte(y, t.y)) return true; else                       return false;
}

template <class T>
bool Tuple2T<T>::m_gt(const self &t) const
{
    if (math::gt (x, t.x)) return true; else if (math::lt(x, t.x)) return false;
    if (math::gt (y, t.y)) return true; else                       return false;
}

template <class T>
bool Tuple2T<T>::m_gte(const self &t) const
{
    if (math::gt (x, t.x)) return true; else if (math::lt(x, t.x)) return false;
    if (math::gte(y, t.y)) return true; else                       return false;
}

#endif  /** _TUPLE2T_H */
