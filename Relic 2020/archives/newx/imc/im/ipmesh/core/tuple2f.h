#ifndef _TUPLE2F_H
#define _TUPLE2F_H

#include "math.h"
//#include "vectors3.h"

class Tuple2f
{
public:

    typedef Tuple2f self;

    union
    {
        struct { float x, y; };
        float data[2];
    };

    inline Tuple2f(float x = 0, float y = 0) : x(x), y(y) {}

	inline float      cross(const self &p) const { return x * p.y - y * p.x; }
	inline float      dot(const self &p)   const { return x * p.x + y * p.y; }

    inline bool          operator==(const self &p)                         const { return math::eq(x, p.x) && math::eq(y, p.y); }
    inline bool          operator==(float p)                               const { return math::eq(x, p)   && math::eq(y, p);   }
	inline bool          operator> (const self &p)                         const { return m_gt(p);  }
	inline bool          operator< (const self &p)                         const { return m_lt(p);  }
	inline bool          operator>=(const self &p)                         const { return m_gte(p); }
	inline bool          operator<=(const self &p)                         const { return m_lte(p); }
	inline bool          operator!=(const self &p)                         const { return !math::eq(x, p.x) || !math::eq(y, p.y); }
	inline bool          operator!=(float p)                               const { return !math::eq(x, p)   || !math::eq(y, p);   }
    inline self         &operator= (float u)                                         { x = y = u; return *this; }
    inline self         &operator= (const self &p)                               { x = p.x; y = p.y; return *this; }

	inline self operator-(const self &p)   const { return self(x - p.x, y - p.y); }
	inline self operator*(const self &p)   const { return self(x * p.x, y * p.y); }
	inline self operator+(const self &p)   const { return self(x + p.x, y + p.y); }
	inline self operator/(const self &p)   const { return self(x / p.x, y / p.y); }

	inline self &operator-=(const self &p)       { x -= p.x; y -= p.y; return *this; }
	inline self &operator*=(const self &p)       { x *= p.x; y *= p.y; return *this; }
	inline self &operator+=(const self &p)       { x += p.x; y += p.y; return *this; }
	inline self &operator/=(const self &p)       { x /= p.x; y /= p.y; return *this; }

	inline self operator-(float p)         const { return self(x - p, y - p); }
	inline self operator*(float p)         const { return self(x * p, y * p); }
	inline self operator+(float p)         const { return self(x + p, y + p); }
	inline self operator/(float p)         const { return self(x / p, y / p); }

	inline self &operator-=(float p)             { x -= p; y -= p; return *this; }
	inline self &operator*=(float p)             { x *= p; y *= p; return *this; }
	inline self &operator+=(float p)             { x += p; y += p; return *this; }
	inline self &operator/=(float p)             { x /= p; y /= p; return *this; }

	inline float lengthsq()                const { return x * x + y * y; }
	inline float length()                  const { return sqrtf(lengthsq()); }
	inline float distancesq(const self &p) const { float dx = p.x - x, dy = p.y - y; return dx * dx + dy * dy; }
	inline float distance(const self &p)   const { return sqrtf(distancesq(p)); }

	inline void set(float x, float y, float z)   { this->x = x; this->y = y; }

	inline self &normalize()
	{
        float d = length();
		if (d != 0.0f) { x /= d; y /= d; }
		return *this;
	}

private:

    inline bool          m_lt(const self &t)  const;
    inline bool          m_lte(const self &t) const;
    inline bool          m_gt(const self &t)  const;
    inline bool          m_gte(const self &t) const;
};

bool Tuple2f::m_lt(const self &t) const
{
    if (math::lt (x, t.x)) return true; else if (math::gt(x, t.x)) return false;
    if (math::lt (y, t.y)) return true; else                       return false;
}

bool Tuple2f::m_lte(const self &t) const
{
    if (math::lt (x, t.x)) return true; else if (math::gt(x, t.x)) return false;
    if (math::lte(y, t.y)) return true; else                       return false;
}

bool Tuple2f::m_gt(const self &t) const
{
    if (math::gt (x, t.x)) return true; else if (math::lt(x, t.x)) return false;
    if (math::gt (y, t.y)) return true; else                       return false;
}

bool Tuple2f::m_gte(const self &t) const
{
    if (math::gt (x, t.x)) return true; else if (math::lt(x, t.x)) return false;
    if (math::gte(y, t.y)) return true; else                       return false;
}

#endif  // _TUPLE2F_H
