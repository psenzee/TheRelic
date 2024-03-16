#ifndef _TUPLE4T_H
#define _TUPLE4T_H

#include "math.h"

#include <iostream>

#include "io/textstreams.h"

template <class T>
class Tuple4T
{
public:

    typedef Tuple4T<T> self;

    struct { T x, y, z, w; };

    inline Tuple4T(T x = 0, T y = 0, T z = 0, T w = 0) : x(x), y(y), z(z), w(w) {}

    inline Tuple4T(const std::basic_string<T> &p)          { int s = p.size(); w = s > 3 ? p[3] : 0; z = s > 2 ? p[2] : 0; y = s > 1 ? p[1] : 0; x = s > 0 ? p[0] : 0; }
    inline Tuple4T(const std::vector<T> &p)                { int s = p.size(); w = s > 3 ? p[3] : 0; z = s > 2 ? p[2] : 0; y = s > 1 ? p[1] : 0; x = s > 0 ? p[0] : 0; }

	inline self  cross3(const self &p)               const { return self(y * p.z - z * p.y, z * p.x - x * p.z,  x * p.y - y * p.x); }
	inline T     dot(const self &p)                  const { return x * p.x + y * p.y + z * p.z + w * p.w; }

    inline self &operator= (T u)                           { x = y = z = w = u; return *this; }
    inline self &operator= (const self &p)                 { x = p.x; y = p.y; z = p.z; w = p.w; return *this; }
    inline self &operator= (const std::basic_string<T> &p) { int s = p.size(); w = s > 3 ? p[3] : 0; z = s > 2 ? p[2] : 0; y = s > 1 ? p[1] : 0; x = s > 0 ? p[0] : 0; return *this; }
	inline self &operator= (const std::vector<T> &p)       { int s = p.size(); w = s > 3 ? p[3] : 0; z = s > 2 ? p[2] : 0; y = s > 1 ? p[1] : 0; x = s > 0 ? p[0] : 0; return *this; }

	inline self  operator-(const self &p)            const { return self(x - p.x, y - p.y, z - p.z, w - p.w); }
	inline self  operator*(const self &p)            const { return self(x * p.x, y * p.y, z * p.z, w * p.w); }
	inline self  operator+(const self &p)            const { return self(x + p.x, y + p.y, z + p.z, w + p.w); }
	inline self  operator/(const self &p)            const { return self(x / p.x, y / p.y, z / p.z, w / p.w); }

	inline self &operator-=(const self &p)                 { x -= p.x; y -= p.y; z -= p.z; w -= p.w; return *this; }
	inline self &operator*=(const self &p)                 { x *= p.x; y *= p.y; z *= p.z; w *= p.w; return *this; }
	inline self &operator+=(const self &p)                 { x += p.x; y += p.y; z += p.z; w += p.w; return *this; }
	inline self &operator/=(const self &p)                 { x /= p.x; y /= p.y; z /= p.z; w /= p.w; return *this; }

	inline self  operator-()                         const { return self(-x, -y, -z, -w); }
	inline self  inverse(void)                       const { return self(1 / x, 1 / y, 1 / z, 1 / w); }

	inline self  operator-(const T &p)               const { return self(x - p, y - p, z - p, w - p); }
	inline self  operator*(const T &p)               const { return self(x * p, y * p, z * p, w * p); }
	inline self  operator+(const T &p)               const { return self(x + p, y + p, z + p, w + p); }
	inline self  operator/(const T &p)               const { return self(x / p, y / p, z / p, w / p); }

	inline self &operator-=(const T &p)                    { x -= p; y -= p; z -= p; w -= p; return *this; }
	inline self &operator*=(const T &p)                    { x *= p; y *= p; z *= p; w *= p; return *this; }
	inline self &operator+=(const T &p)                    { x += p; y += p; z += p; w += p; return *this; }
	inline self &operator/=(const T &p)                    { x /= p; y /= p; z /= p; w /= p; return *this; }

	inline T     lengthsq(void)                      const { return x * x + y * y + z * z + w * w; }
	inline T     length(void)                        const { return (T)sqrt(lengthsq()); }
	inline T     distance(const self &p)             const { T dx = p.x - x, dy = p.y - y, dz = p.z - z; dw = p.w - w; return (T)sqrt(dx * dx + dy * dy + dz * dz + dw * dw); }

	inline void  set(T x, T y, T z, T w)                   { this->x = x; this->y = y; this->z = z; this->w = w; }

	inline self &normalize(void)                           { T d = length(); if (d != 0) { x /= d; y /= d; z /= d; w /= d; } return *this; }
	inline self  normal(void)                              { self r(*this); r.normalize(); return r; }

    inline std::ostream &writexml(std::ostream &s, int depth) const
	{
        return TextStreams::printfi(s, "  ", depth, "<point x=\"%.3f\" y=\"%.3f\" z=\"%.3f\" w=\"%.3f\" />\n", x, y, z, w);
    }
};

template <class T> inline std::ostream &operator<<(std::ostream &s, const Tuple4T<T> &p) { return s << "{" << p.x << ", " << p.y << ", " << p.z << ", " << p.w << "}"; }

#endif  /** _TUPLE4T_H */