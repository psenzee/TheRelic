#ifndef _TUPLE3T_H
#define _TUPLE3T_H

#include "math.h"
//#include "vectors3.h"

#include <iostream>
#include <float.h>
#include <math.h>

#include <vector>
#include <string>

#include "io/textstreams.h"

template <class T>
class Tuple3T
{
public:

    typedef Tuple3T<T> self;

//    union
//    {
        struct { T x, y, z; };
        //T data[3]; // took this out so that Tuple3T could work with no trivial types..
//    };

    inline explicit Tuple3T(T x = 0, T y = 0, T z = 0) : x(x), y(y), z(z)        {}
//	inline explicit Tuple3T(const self &p) : x(p.x), y(p.y), z(p.z)              {}
    inline explicit Tuple3T(const std::basic_string<T> &p)                       { size_t s = p.size(); z = s > 2 ? p[2] : 0; y = s > 1 ? p[1] : 0; x = s > 0 ? p[0] : 0; }
    inline explicit Tuple3T(const std::vector<T> &p)                             { size_t s = p.size(); z = s > 2 ? p[2] : 0; y = s > 1 ? p[1] : 0; x = s > 0 ? p[0] : 0; }

	inline self          cross(const self &p)                              const { return self(y * p.z - z * p.y, z * p.x - x * p.z,  x * p.y - y * p.x); }
	inline T             dot(const self &p)                                const { return x * p.x + y * p.y + z * p.z; }

	inline bool          operator> (const self &p)                         const { return m_gt(p);  }
	inline bool          operator< (const self &p)                         const { return m_lt(p);  }
	inline bool          operator>=(const self &p)                         const { return m_gte(p); }
	inline bool          operator<=(const self &p)                         const { return m_lte(p); }

    inline bool          exact_gt  (const self &p)                         const { return m_exact_gt(p);  }
    inline bool          exact_lt  (const self &p)                         const { return m_exact_lt(p);  }
    inline bool          exact_gte (const self &p)                         const { return m_exact_gte(p); }
    inline bool          exact_lte (const self &p)                         const { return m_exact_lte(p); }

	inline bool          operator!=(const self &p)                         const { return !math::eq(x, p.x) || !math::eq(y, p.y) || !math::eq(z, p.z); }
	inline bool          operator!=(const T &p)                            const { return !math::eq(x, p)   || !math::eq(y, p)   || !math::eq(z, p);   }

    inline self         &operator= (T u)                                         { x = y = z = u; return *this; }
    inline self         &operator= (const self &p)                               { x = p.x; y = p.y; z = p.z; return *this; }
    inline self         &operator= (const std::basic_string<float> &p)           { int s = p.size(); z = s > 2 ? p[2] : 0; y = s > 1 ? p[1] : 0; x = s > 0 ? p[0] : 0; return *this; }
//  inline self         &operator= (const std::basic_string<T> &p)               { int s = p.size(); z = s > 2 ? p[2] : 0; y = s > 1 ? p[1] : 0; x = s > 0 ? p[0] : 0; return *this; }
	inline self         &operator= (const std::vector<T> &p)                     { int s = p.size(); z = s > 2 ? p[2] : 0; y = s > 1 ? p[1] : 0; x = s > 0 ? p[0] : 0; return *this; }

	inline bool          operator==(const self &p)                         const { return math::eq(x, p.x) && math::eq(y, p.y) && math::eq(z, p.z); }
    inline bool          exact_eq  (const self &p)                         const { return x == p.x && y == p.y && z == p.z; }
    inline bool          exact_neq (const self &p)                         const { return x != p.x || y != p.y || z != p.z; }
    inline bool          exact_neq (const T &p)                            const { return x != p || y != p || z != p; }

	inline self          operator-(const self &p)                          const { return self(x - p.x, y - p.y, z - p.z); }
	inline self          operator*(const self &p)                          const { return self(x * p.x, y * p.y, z * p.z); }
	inline self          operator+(const self &p)                          const { return self(x + p.x, y + p.y, z + p.z); }
	inline self          operator/(const self &p)                          const { return self(x / p.x, y / p.y, z / p.z); }

	inline self         &operator-=(const self &p)                               { x -= p.x; y -= p.y; z -= p.z; return *this; }
	inline self         &operator*=(const self &p)                               { x *= p.x; y *= p.y; z *= p.z; return *this; }
	inline self         &operator+=(const self &p)                               { x += p.x; y += p.y; z += p.z; return *this; }
	inline self         &operator/=(const self &p)                               { x /= p.x; y /= p.y; z /= p.z; return *this; }

	inline bool          operator==(const T &p)                            const { return !operator!=(p); }

	inline self          operator-()                                       const { return self(-x, -y, -z); }
    inline self          inverse()                                         const { return self(1 / x, 1 / y, 1 / z); }

	inline self          operator-(const T &p)                             const { return self(x - p, y - p, z - p); }
	inline self          operator*(const T &p)                             const { return self(x * p, y * p, z * p); }
	inline self          operator+(const T &p)                             const { return self(x + p, y + p, z + p); }
	inline self          operator/(const T &p)                             const { T q = T(1) / p; return self(x * q, y * q, z * q); }

	inline self         &operator-=(const T &p)                                  { x -= p; y -= p; z -= p; return *this; }
	inline self         &operator*=(const T &p)                                  { x *= p; y *= p; z *= p; return *this; }
	inline self         &operator+=(const T &p)                                  { x += p; y += p; z += p; return *this; }
	inline self         &operator/=(const T &p)                                  { T q = T(1) / p; x *= q; y *= q; z *= q; return *this; }

	inline T             lengthsq()                                        const { return x * x + y * y + z * z; }
    inline T             length()                                          const { return (T)sqrt((T)lengthsq()); }
	inline T             distancesq(const self &p)                         const { T dx = p.x - x, dy = p.y - y, dz = p.z - z; return dx * dx + dy * dy + dz * dz; }
	inline T             distance(const self &p)                           const { return (T)sqrt(distancesq(p)); }

	inline void          set(T x, T y, T z)                                      { this->x = x; this->y = y; this->z = z; }

	inline self         &normalize()                                             { T d = length(); if (d != (T)0) { d = (T)1 / d; x *= d; y *= d; z *= d; } return *this; }
	inline self          normal()                                          const { self r(*this); r.normalize(); return r; }

    inline std::ostream &writexml(std::ostream &s, int depth)              const { return TextStreams::printfi(s, "  ", depth, "<point x=\"%.3f\" y=\"%.3f\" z=\"%.3f\" />\n", x, y, z); }
  
private:

    inline bool          m_lt(const self &t)  const;
    inline bool          m_lte(const self &t) const;
    inline bool          m_gt(const self &t)  const;
    inline bool          m_gte(const self &t) const;
    inline bool          m_exact_lt(const self &t)  const;
    inline bool          m_exact_lte(const self &t) const;
    inline bool          m_exact_gt(const self &t)  const;
    inline bool          m_exact_gte(const self &t) const;
};

// template implementation..

template <class T>
inline std::ostream &operator<<(std::ostream &s, const Tuple3T<T> &p)
{
    return s << "{" << p.x << ", " << p.y << ", " << p.z << "}";
}

template <class T> inline Tuple3T<T> operator/(const T &p, const Tuple3T<float> &v) { return Tuple3T<T>(p / v.x, p / v.y, p / v.z); }
template <class T> inline Tuple3T<T> operator*(const T &p, const Tuple3T<float> &v) { return Tuple3T<T>(p * v.x, p * v.y, p * v.z); }
template <class T> inline Tuple3T<T> operator+(const T &p, const Tuple3T<float> &v) { return Tuple3T<T>(p + v.x, p + v.y, p + v.z); }
template <class T> inline Tuple3T<T> operator-(const T &p, const Tuple3T<float> &v) { return Tuple3T<T>(p - v.x, p - v.y, p - v.z); }

template <class T>
bool Tuple3T<T>::m_lt(const self &t) const
{
    if (math::lt (x, t.x)) return true; else if (math::gt(x, t.x)) return false;
    if (math::lt (y, t.y)) return true; else if (math::gt(y, t.y)) return false;
    if (math::lt (z, t.z)) return true; else                       return false;
}

template <class T>
bool Tuple3T<T>::m_lte(const self &t) const
{
    if (math::lt (x, t.x)) return true; else if (math::gt(x, t.x)) return false;
    if (math::lt (y, t.y)) return true; else if (math::gt(y, t.y)) return false;
    if (math::lte(z, t.z)) return true; else                       return false;
}

template <class T>
bool Tuple3T<T>::m_gt(const self &t) const
{
    if (math::gt (x, t.x)) return true; else if (math::lt(x, t.x)) return false;
    if (math::gt (y, t.y)) return true; else if (math::lt(y, t.y)) return false;
    if (math::gt (z, t.z)) return true; else                       return false;
}

template <class T>
bool Tuple3T<T>::m_gte(const self &t) const
{
    if (math::gt (x, t.x)) return true; else if (math::lt(x, t.x)) return false;
    if (math::gt (y, t.y)) return true; else if (math::lt(y, t.y)) return false;
    if (math::gte(z, t.z)) return true; else                       return false;
}

template <class T>
bool Tuple3T<T>::m_exact_lt(const self &t) const
{
    if (x < t.x) return true; else if (x > t.x) return false;
    if (y < t.y) return true; else if (y > t.y) return false;
    if (z < t.z) return true; else              return false;
}

template <class T>
bool Tuple3T<T>::m_exact_lte(const self &t) const
{
    if (x < t.x)  return true; else if (x > t.x) return false;
    if (y < t.y)  return true; else if (y > t.y) return false;
    if (z <= t.z) return true; else              return false;
}

template <class T>
bool Tuple3T<T>::m_exact_gt(const self &t) const
{
    if (x > t.x) return true; else if (x < t.x) return false;
    if (y > t.y) return true; else if (y < t.y) return false;
    if (z > t.z) return true; else              return false;
}

template <class T>
bool Tuple3T<T>::m_exact_gte(const self &t) const
{
    if (x > t.x)  return true; else if (x < t.x) return false;
    if (y > t.y)  return true; else if (y < t.y) return false;
    if (z >= t.z) return true; else              return false;
}

#endif  /** _TUPLE3T_H */
