#ifndef _TUPLE3F_H
#define _TUPLE3F_H

#include "mathcore.h"
#include "tuple2f.h"

#include <float.h>
#include <cmath>
#include <iostream>
#include <iomanip>

class Tuple3f
{
public:

    typedef Tuple3f self_t;

    union
    {
        struct { float x, y, z; };
        float data[3];
    };

    inline explicit Tuple3f(float x = 0.f, float y = 0.f, float z = 0.f) : x(x), y(y), z(z)        {}
//    inline explicit Tuple3f(const self_t &p) : x(p.x), y(p.y), z(p.z)              {}

    inline self_t        cross(const self_t &p)                              const { return self_t(y * p.z - z * p.y, z * p.x - x * p.z,  x * p.y - y * p.x); }
    inline float         dot(const self_t &p)                                const { return x * p.x + y * p.y + z * p.z; }

    inline bool          operator> (const self_t &p)                         const { return m_gt(p);  }
    inline bool          operator< (const self_t &p)                         const { return m_lt(p);  }
    inline bool          operator>=(const self_t &p)                         const { return m_gte(p); }
    inline bool          operator<=(const self_t &p)                         const { return m_lte(p); }

    inline bool          exact_gt  (const self_t &p)                         const { return m_exact_gt(p);  }
    inline bool          exact_lt  (const self_t &p)                         const { return m_exact_lt(p);  }
    inline bool          exact_gte (const self_t &p)                         const { return m_exact_gte(p); }
    inline bool          exact_lte (const self_t &p)                         const { return m_exact_lte(p); }

    inline bool          operator!=(const self_t &p)                         const { return !math::eq(x, p.x) || !math::eq(y, p.y) || !math::eq(z, p.z); }
    inline bool          operator!=(float p)                                 const { return !math::eq(x, p)   || !math::eq(y, p)   || !math::eq(z, p);   }

    inline self_t       &operator= (float u)                                       { x = y = z = u; return *this; }
    inline self_t       &operator= (const self_t &p)                               { x = p.x; y = p.y; z = p.z; return *this; }

    inline bool          operator==(const self_t &p)                         const { return math::eq(x, p.x) && math::eq(y, p.y) && math::eq(z, p.z); }
    inline bool          exact_eq  (const self_t &p)                         const { return x == p.x && y == p.y && z == p.z; }
    inline bool          exact_neq (const self_t &p)                         const { return x != p.x || y != p.y || z != p.z; }
    inline bool          exact_neq (float p)                                 const { return x != p || y != p || z != p; }

    inline self_t        operator-(const self_t &p)                          const { return self_t(x - p.x, y - p.y, z - p.z); }
    inline self_t        operator*(const self_t &p)                          const { return self_t(x * p.x, y * p.y, z * p.z); }
    inline self_t        operator+(const self_t &p)                          const { return self_t(x + p.x, y + p.y, z + p.z); }
    inline self_t        operator/(const self_t &p)                          const { return self_t(x / p.x, y / p.y, z / p.z); }

    inline self_t       &operator-=(const self_t &p)                               { x -= p.x; y -= p.y; z -= p.z; return *this; }
    inline self_t       &operator*=(const self_t &p)                               { x *= p.x; y *= p.y; z *= p.z; return *this; }
    inline self_t       &operator+=(const self_t &p)                               { x += p.x; y += p.y; z += p.z; return *this; }
    inline self_t       &operator/=(const self_t &p)                               { x /= p.x; y /= p.y; z /= p.z; return *this; }

    inline bool          operator==(float p)                                 const { return !operator!=(p); }

    inline self_t        operator-()                                         const { return self_t(-x, -y, -z); }
    inline self_t        inverse()                                           const { return self_t(1 / x, 1 / y, 1 / z); }

    inline self_t        operator-(float p)                                  const { return self_t(x - p, y - p, z - p); }
    inline self_t        operator*(float p)                                  const { return self_t(x * p, y * p, z * p); }
    inline self_t        operator+(float p)                                  const { return self_t(x + p, y + p, z + p); }
    inline self_t        operator/(float p)                                  const { float q = 1.0f / p; return self_t(x * q, y * q, z * q); }

    inline self_t       &operator-=(float p)                                       { x -= p; y -= p; z -= p; return *this; }
    inline self_t       &operator*=(float p)                                       { x *= p; y *= p; z *= p; return *this; }
    inline self_t       &operator+=(float p)                                       { x += p; y += p; z += p; return *this; }
    inline self_t       &operator/=(float p)                                       { float q = 1.0f / p; x *= q; y *= q; z *= q; return *this; }

    inline float         lengthsq()                                          const { return x * x + y * y + z * z; }
    inline float         length()                                            const { return std::sqrt(lengthsq()); }
    inline float         distancesq(const self_t &p)                         const { float dx = p.x - x, dy = p.y - y, dz = p.z - z; return dx * dx + dy * dy + dz * dz; }
    inline float         distance(const self_t &p)                           const { return std::sqrt(distancesq(p)); }

    inline void          set(float x, float y, float z)                            { this->x = x; this->y = y; this->z = z; }

    inline self_t       &normalize()                                               { float d = length(); if (d != 0.0f) { d = 1.0f / d; x *= d; y *= d; z *= d; } return *this; }
    inline self_t        normal()                                            const { self_t r(*this); r.normalize(); return r; }

    inline self_t        minimum(const self_t &u)                            const { return self_t(std::min(x, u.x), std::min(y, u.y), std::min(z, u.z)); }
    inline self_t        maximum(const self_t &u)                            const { return self_t(std::max(x, u.x), std::max(y, u.y), std::max(z, u.z)); }
  
    inline Tuple2f       xy()                                                const { return Tuple2f(x, y); }
    inline Tuple2f       xz()                                                const { return Tuple2f(x, z); }
    inline Tuple2f       yz()                                                const { return Tuple2f(y, z); }
    inline Tuple2f       yx()                                                const { return Tuple2f(y, x); }
    inline Tuple2f       zx()                                                const { return Tuple2f(z, x); }
    inline Tuple2f       zy()                                                const { return Tuple2f(z, y); }
    
    inline self_t        xyz()                                               const { return self_t(x, y, z); }
    inline self_t        xzy()                                               const { return self_t(x, z, y); }
    inline self_t        yxz()                                               const { return self_t(y, x, z); }
    inline self_t        yzx()                                               const { return self_t(y, z, x); }
    inline self_t        zyx()                                               const { return self_t(z, y, x); }
    inline self_t        zxy()                                               const { return self_t(z, x, y); }
  
    inline size_t        hash()                                              const { return math::hash(data, 3); }
    
    static inline const Tuple3f &zero()  { static Tuple3f u(0.0f, 0.0f, 0.0f); return u; }
    static inline const Tuple3f &one()   { static Tuple3f u(1.0f, 1.0f, 1.0f); return u; }
    static inline const Tuple3f &unitx() { static Tuple3f u(1.0f, 0.0f, 0.0f); return u; }
    static inline const Tuple3f &unity() { static Tuple3f u(0.0f, 1.0f, 0.0f); return u; }
    static inline const Tuple3f &unitz() { static Tuple3f u(0.0f, 0.0f, 1.0f); return u; }
    /*
    inline bool operator< (const self_t &a)             const { if (x != a.x) return x < a.x; if (y != a.y) return y < a.y; return z < a.z; }
    inline bool operator> (const self_t &a)             const { if (x != a.x) return x > a.x; if (y != a.y) return y > a.y; return z > a.z; }
    inline bool operator<=(const self_t &a)             const { return !(*this > a); }
    inline bool operator>=(const self_t &a)             const { return !(*this < a); }
    inline bool operator==(const self_t &a)             const { return x == a.x && y == a.y && z == a.z; }
    inline bool operator!=(const self_t &a)             const { return x != a.x || y != a.y || z != a.z; }
     */
    
private:

    inline bool          m_lt(const self_t &t)  const;
    inline bool          m_lte(const self_t &t) const;
    inline bool          m_gt(const self_t &t)  const;
    inline bool          m_gte(const self_t &t) const;
    inline bool          m_exact_lt(const self_t &t)  const;
    inline bool          m_exact_lte(const self_t &t) const;
    inline bool          m_exact_gt(const self_t &t)  const;
    inline bool          m_exact_gte(const self_t &t) const;
};

inline std::ostream &operator<<(std::ostream &os, const Tuple3f &v)
{
    return os << std::setw(5) << std::setprecision(4) << std::fixed << v.x << "," << v.y << "," << v.z;
}

// template implementation..
inline Tuple3f operator/(float p, const Tuple3f &v) { return Tuple3f(p / v.x, p / v.y, p / v.z); }
inline Tuple3f operator*(float p, const Tuple3f &v) { return Tuple3f(p * v.x, p * v.y, p * v.z); }
inline Tuple3f operator+(float p, const Tuple3f &v) { return Tuple3f(p + v.x, p + v.y, p + v.z); }
inline Tuple3f operator-(float p, const Tuple3f &v) { return Tuple3f(p - v.x, p - v.y, p - v.z); }

bool Tuple3f::m_lt(const self_t &t) const
{
    if (math::lt (x, t.x)) return true; else if (math::gt(x, t.x)) return false;
    if (math::lt (y, t.y)) return true; else if (math::gt(y, t.y)) return false;
    if (math::lt (z, t.z)) return true; else                       return false;
}

bool Tuple3f::m_lte(const self_t &t) const
{
    if (math::lt (x, t.x)) return true; else if (math::gt(x, t.x)) return false;
    if (math::lt (y, t.y)) return true; else if (math::gt(y, t.y)) return false;
    if (math::lte(z, t.z)) return true; else                       return false;
}

bool Tuple3f::m_gt(const self_t &t) const
{
    if (math::gt (x, t.x)) return true; else if (math::lt(x, t.x)) return false;
    if (math::gt (y, t.y)) return true; else if (math::lt(y, t.y)) return false;
    if (math::gt (z, t.z)) return true; else                       return false;
}

bool Tuple3f::m_gte(const self_t &t) const
{
    if (math::gt (x, t.x)) return true; else if (math::lt(x, t.x)) return false;
    if (math::gt (y, t.y)) return true; else if (math::lt(y, t.y)) return false;
    if (math::gte(z, t.z)) return true; else                       return false;
}

bool Tuple3f::m_exact_lt(const self_t &t) const
{
    if (x < t.x) return true; else if (x > t.x) return false;
    if (y < t.y) return true; else if (y > t.y) return false;
    if (z < t.z) return true; else              return false;
}

bool Tuple3f::m_exact_lte(const self_t &t) const
{
    if (x < t.x)  return true; else if (x > t.x) return false;
    if (y < t.y)  return true; else if (y > t.y) return false;
    if (z <= t.z) return true; else              return false;
}

bool Tuple3f::m_exact_gt(const self_t &t) const
{
    if (x > t.x) return true; else if (x < t.x) return false;
    if (y > t.y) return true; else if (y < t.y) return false;
    if (z > t.z) return true; else              return false;
}

bool Tuple3f::m_exact_gte(const self_t &t) const
{
    if (x > t.x)  return true; else if (x < t.x) return false;
    if (y > t.y)  return true; else if (y < t.y) return false;
    if (z >= t.z) return true; else              return false;
}

#endif  // _TUPLE3F_H
