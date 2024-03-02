#ifndef _TUPLE4F_H
#define _TUPLE4F_H

#include "mathcore.h"
#include <math.h>

//extern "C" double sqrt(double);

class Tuple4f
{
public:

    typedef Tuple4f self_t;
    
    struct { float x, y, z, w; };

    inline constexpr explicit Tuple4f(float x = 0.f, float y = 0.f, float z = 0.f, float w = 0.f) : x(x), y(y), z(z), w(w) {}
    inline constexpr explicit Tuple4f(const Tuple3f &v3, float w = 0.f) : x(v3.x), y(v3.y), z(v3.z), w(w) {}

    inline constexpr self_t  cross3(const self_t &p)              const { return self_t(y * p.z - z * p.y, z * p.x - x * p.z,  x * p.y - y * p.x); }
    inline constexpr float   dot(const self_t &p)                 const { return x * p.x + y * p.y + z * p.z + w * p.w; }

    inline self_t &operator= (float u)                        { x = y = z = w = u; return *this; }
    inline self_t &operator= (const self_t &p)                { x = p.x; y = p.y; z = p.z; w = p.w; return *this; }

    inline self_t  operator-(const self_t &p)           const { return self_t(x - p.x, y - p.y, z - p.z, w - p.w); }
    inline self_t  operator*(const self_t &p)           const { return self_t(x * p.x, y * p.y, z * p.z, w * p.w); }
    inline self_t  operator+(const self_t &p)           const { return self_t(x + p.x, y + p.y, z + p.z, w + p.w); }
    inline self_t  operator/(const self_t &p)           const { return self_t(x / p.x, y / p.y, z / p.z, w / p.w); }

    inline self_t &operator-=(const self_t &p)                { x -= p.x; y -= p.y; z -= p.z; w -= p.w; return *this; }
    inline self_t &operator*=(const self_t &p)                { x *= p.x; y *= p.y; z *= p.z; w *= p.w; return *this; }
    inline self_t &operator+=(const self_t &p)                { x += p.x; y += p.y; z += p.z; w += p.w; return *this; }
    inline self_t &operator/=(const self_t &p)                { x /= p.x; y /= p.y; z /= p.z; w /= p.w; return *this; }

    inline self_t  operator-()                          const { return self_t(-x, -y, -z, -w); }
    inline self_t  inverse()                            const { return self_t(1.0f / x, 1.0f / y, 1.0f / z, 1.0f / w); }

    inline self_t  operator-(float p)                   const { return self_t(x - p, y - p, z - p, w - p); }
    inline self_t  operator*(float p)                   const { return self_t(x * p, y * p, z * p, w * p); }
    inline self_t  operator+(float p)                   const { return self_t(x + p, y + p, z + p, w + p); }
    inline self_t  operator/(float p)                   const { return self_t(x / p, y / p, z / p, w / p); }

    inline self_t &operator-=(float p)                        { x -= p; y -= p; z -= p; w -= p; return *this; }
    inline self_t &operator*=(float p)                        { x *= p; y *= p; z *= p; w *= p; return *this; }
    inline self_t &operator+=(float p)                        { x += p; y += p; z += p; w += p; return *this; }
    inline self_t &operator/=(float p)                        { x /= p; y /= p; z /= p; w /= p; return *this; }
    
    inline const float *data() const                          { return (const float *)this; }
    inline float       *data()                                { return (float *)this; }
    
    inline Tuple3f xyz() const                                { return Tuple3f(x, y, z); }

    inline float   lengthsq()                           const { return x * x + y * y + z * z + w * w; }
    inline float   length()                             const { return sqrtf(lengthsq()); }
    inline float   distance(const self_t &p)            const { float dx = p.x - x, dy = p.y - y, dz = p.z - z, dw = p.w - w; return sqrtf(dx * dx + dy * dy + dz * dz + dw * dw); }

    inline self_t  &set(float x, float y, float z, float w)    { this->x = x; this->y = y; this->z = z; this->w = w; return *this; }
    inline self_t  &set(float v)                               { this->x = this->y = this->z = this->w = v; return *this; }

    inline self_t  &normalize()                                { float d = length(); if (d != 0.0f) { x /= d; y /= d; z /= d; w /= d; } return *this; }
    inline self_t   normal()                                   { self_t r(*this); r.normalize(); return r; }
};

#endif  // _TUPLE4F_H 
