#ifndef _TUPLE3F_H
#define _TUPLE3F_H

#include "mathcore.h"

#include <float.h>
#include <math.h>

//extern "C" double sqrt(double);

class Tuple3f
{
public:

    typedef Tuple3f self;

    union
    {
        struct { float x, y, z; };
        float data[3];
    };

    inline explicit Tuple3f(float x = 0, float y = 0, float z = 0) : x(x), y(y), z(z)        {}
//    inline explicit Tuple3f(const self &p) : x(p.x), y(p.y), z(p.z)              {}

    inline self          cross(const self &p)                              const { return self(y * p.z - z * p.y, z * p.x - x * p.z,  x * p.y - y * p.x); }
    inline float         dot(const self &p)                                const { return x * p.x + y * p.y + z * p.z; }

    inline bool          operator> (const self &p)                         const { return m_gt(p);  }
    inline bool          operator< (const self &p)                         const { return m_lt(p);  }
    inline bool          operator>=(const self &p)                         const { return m_gte(p); }
    inline bool          operator<=(const self &p)                         const { return m_lte(p); }

    inline bool          exact_gt  (const self &p)                         const { return m_exact_gt(p);  }
    inline bool          exact_lt  (const self &p)                         const { return m_exact_lt(p);  }
    inline bool          exact_gte (const self &p)                         const { return m_exact_gte(p); }
    inline bool          exact_lte (const self &p)                         const { return m_exact_lte(p); }

    inline bool          operator!=(const self &p)                         const { return !math::eq(x, p.x) || !math::eq(y, p.y) || !math::eq(z, p.z); }
    inline bool          operator!=(float p)                               const { return !math::eq(x, p)   || !math::eq(y, p)   || !math::eq(z, p);   }

    inline self         &operator= (float u)                                     { x = y = z = u; return *this; }
    inline self         &operator= (const self &p)                               { x = p.x; y = p.y; z = p.z; return *this; }

    inline bool          operator==(const self &p)                         const { return math::eq(x, p.x) && math::eq(y, p.y) && math::eq(z, p.z); }
    inline bool          exact_eq  (const self &p)                         const { return x == p.x && y == p.y && z == p.z; }
    inline bool          exact_neq (const self &p)                         const { return x != p.x || y != p.y || z != p.z; }
    inline bool          exact_neq (float p)                               const { return x != p || y != p || z != p; }

    inline self          operator-(const self &p)                          const { return self(x - p.x, y - p.y, z - p.z); }
    inline self          operator*(const self &p)                          const { return self(x * p.x, y * p.y, z * p.z); }
    inline self          operator+(const self &p)                          const { return self(x + p.x, y + p.y, z + p.z); }
    inline self          operator/(const self &p)                          const { return self(x / p.x, y / p.y, z / p.z); }

    inline self         &operator-=(const self &p)                               { x -= p.x; y -= p.y; z -= p.z; return *this; }
    inline self         &operator*=(const self &p)                               { x *= p.x; y *= p.y; z *= p.z; return *this; }
    inline self         &operator+=(const self &p)                               { x += p.x; y += p.y; z += p.z; return *this; }
    inline self         &operator/=(const self &p)                               { x /= p.x; y /= p.y; z /= p.z; return *this; }

    inline bool          operator==(float p)                               const { return !operator!=(p); }

    inline self          operator-()                                       const { return self(-x, -y, -z); }
    inline self          inverse()                                         const { return self(1 / x, 1 / y, 1 / z); }

    inline self          operator-(float p)                                const { return self(x - p, y - p, z - p); }
    inline self          operator*(float p)                                const { return self(x * p, y * p, z * p); }
    inline self          operator+(float p)                                const { return self(x + p, y + p, z + p); }
    inline self          operator/(float p)                                const { float q = 1.0f / p; return self(x * q, y * q, z * q); }

    inline self         &operator-=(float p)                                     { x -= p; y -= p; z -= p; return *this; }
    inline self         &operator*=(float p)                                     { x *= p; y *= p; z *= p; return *this; }
    inline self         &operator+=(float p)                                     { x += p; y += p; z += p; return *this; }
    inline self         &operator/=(float p)                                     { float q = 1.0f / p; x *= q; y *= q; z *= q; return *this; }

    inline float         lengthsq()                                        const { return x * x + y * y + z * z; }
    inline float         length()                                          const { return sqrtf(lengthsq()); }
    inline float         distancesq(const self &p)                         const { float dx = p.x - x, dy = p.y - y, dz = p.z - z; return dx * dx + dy * dy + dz * dz; }
    inline float         distance(const self &p)                           const { return sqrtf(distancesq(p)); }

    inline void          set(float x, float y, float z)                          { this->x = x; this->y = y; this->z = z; }

    inline self         &normalize()                                             { float d = length(); if (d != 0.0f) { d = 1.0f / d; x *= d; y *= d; z *= d; } return *this; }
    inline self          normal()                                          const { self r(*this); r.normalize(); return r; }

    inline self          minimum(const self &u)                            const { return self(x < u.x ? x : u.x, y < u.y ? y : u.y, x < u.z ? z : u.z); }
    inline self          maximum(const self &u)                            const { return self(x > u.x ? x : u.x, y > u.y ? y : u.y, x > u.z ? z : u.z); }
  
    static inline const Tuple3f &zero()  { static Tuple3f u(0.0f, 0.0f, 0.0f); return u; }
    static inline const Tuple3f &one()   { static Tuple3f u(1.0f, 1.0f, 1.0f); return u; }
    static inline const Tuple3f &unitX() { static Tuple3f u(1.0f, 0.0f, 0.0f); return u; }    
    static inline const Tuple3f &unitY() { static Tuple3f u(0.0f, 1.0f, 0.0f); return u; }
    static inline const Tuple3f &unitZ() { static Tuple3f u(0.0f, 0.0f, 1.0f); return u; }    
    
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
inline Tuple3f operator/(float p, const Tuple3f &v) { return Tuple3f(p / v.x, p / v.y, p / v.z); }
inline Tuple3f operator*(float p, const Tuple3f &v) { return Tuple3f(p * v.x, p * v.y, p * v.z); }
inline Tuple3f operator+(float p, const Tuple3f &v) { return Tuple3f(p + v.x, p + v.y, p + v.z); }
inline Tuple3f operator-(float p, const Tuple3f &v) { return Tuple3f(p - v.x, p - v.y, p - v.z); }

bool Tuple3f::m_lt(const self &t) const
{
    if (math::lt (x, t.x)) return true; else if (math::gt(x, t.x)) return false;
    if (math::lt (y, t.y)) return true; else if (math::gt(y, t.y)) return false;
    if (math::lt (z, t.z)) return true; else                       return false;
}

bool Tuple3f::m_lte(const self &t) const
{
    if (math::lt (x, t.x)) return true; else if (math::gt(x, t.x)) return false;
    if (math::lt (y, t.y)) return true; else if (math::gt(y, t.y)) return false;
    if (math::lte(z, t.z)) return true; else                       return false;
}

bool Tuple3f::m_gt(const self &t) const
{
    if (math::gt (x, t.x)) return true; else if (math::lt(x, t.x)) return false;
    if (math::gt (y, t.y)) return true; else if (math::lt(y, t.y)) return false;
    if (math::gt (z, t.z)) return true; else                       return false;
}

bool Tuple3f::m_gte(const self &t) const
{
    if (math::gt (x, t.x)) return true; else if (math::lt(x, t.x)) return false;
    if (math::gt (y, t.y)) return true; else if (math::lt(y, t.y)) return false;
    if (math::gte(z, t.z)) return true; else                       return false;
}

bool Tuple3f::m_exact_lt(const self &t) const
{
    if (x < t.x) return true; else if (x > t.x) return false;
    if (y < t.y) return true; else if (y > t.y) return false;
    if (z < t.z) return true; else              return false;
}

bool Tuple3f::m_exact_lte(const self &t) const
{
    if (x < t.x)  return true; else if (x > t.x) return false;
    if (y < t.y)  return true; else if (y > t.y) return false;
    if (z <= t.z) return true; else              return false;
}

bool Tuple3f::m_exact_gt(const self &t) const
{
    if (x > t.x) return true; else if (x < t.x) return false;
    if (y > t.y) return true; else if (y < t.y) return false;
    if (z > t.z) return true; else              return false;
}

bool Tuple3f::m_exact_gte(const self &t) const
{
    if (x > t.x)  return true; else if (x < t.x) return false;
    if (y > t.y)  return true; else if (y < t.y) return false;
    if (z >= t.z) return true; else              return false;
}

#endif  // _TUPLE3F_H
