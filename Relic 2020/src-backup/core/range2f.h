#ifndef _RANGE2F_H
#define _RANGE2F_H

#include "mathcore.h"

extern "C" double sqrt(double);

class Range2f
{
public:

    typedef Range2f self;

    union
    {
        struct { float min, max; };
        float data[2];
    };

    inline explicit Range2f(float min = 0.f, float max = 0.f) : min(min), max(max) {}

    inline bool          operator==(const self &p)                         const { return math::eq(min, p.min) && math::eq(max, p.max); }
    inline bool          operator==(float p)                               const { return math::eq(min, p)   && math::eq(max, p);   }
    inline bool          operator> (const self &p)                         const { return m_gt(p);  }
    inline bool          operator< (const self &p)                         const { return m_lt(p);  }
    inline bool          operator>=(const self &p)                         const { return m_gte(p); }
    inline bool          operator<=(const self &p)                         const { return m_lte(p); }
    inline bool          operator!=(const self &p)                         const { return !math::eq(min, p.min) || !math::eq(max, p.max); }
    inline bool          operator!=(float p)                               const { return !math::eq(min, p)   || !math::eq(max, p);   }
    inline self         &operator= (float u)                                     { min = max = u; return *this; }
    inline self         &operator= (const self &p)                               { min = p.min; max = p.max; return *this; }

    inline self operator-(const self &p)   const { return self(min - p.min, max - p.max); }
    inline self operator*(const self &p)   const { return self(min * p.min, max * p.max); }
    inline self operator+(const self &p)   const { return self(min + p.min, max + p.max); }
    inline self operator/(const self &p)   const { return self(min / p.min, max / p.max); }

    inline self &operator-=(const self &p)       { min -= p.min; max -= p.max; return *this; }
    inline self &operator*=(const self &p)       { min *= p.min; max *= p.max; return *this; }
    inline self &operator+=(const self &p)       { min += p.min; max += p.max; return *this; }
    inline self &operator/=(const self &p)       { min /= p.min; max /= p.max; return *this; }

    inline self operator-(float p)         const { return self(min - p, max - p); }
    inline self operator*(float p)         const { return self(min * p, max * p); }
    inline self operator+(float p)         const { return self(min + p, max + p); }
    inline self operator/(float p)         const { return self(min / p, max / p); }

    inline self &operator-=(float p)             { min -= p; max -= p; return *this; }
    inline self &operator*=(float p)             { min *= p; max *= p; return *this; }
    inline self &operator+=(float p)             { min += p; max += p; return *this; }
    inline self &operator/=(float p)             { min /= p; max /= p; return *this; }

    inline void set(float min, float max)        { this->min = min; this->max = max; }

    inline self &normalize()
    {
        float mn = this->min, mx = this->max;
        if (mx < mn) {
            this->max = mn;
            this->min = mx;
        }
        return *this;
    }

    inline self          minimum(const self &u)                            const { return self(min < u.min ? min : u.min, max < u.max ? max : u.max); }
    inline self          maximum(const self &u)                            const { return self(min > u.min ? min : u.min, max > u.max ? max : u.max); }

    static inline const self &zero()  { static self u(0.0f, 0.0f); return u; }
    static inline const self &one()   { static self u(1.0f, 1.0f); return u; }
    
private:

    inline bool          m_lt(const self &t)  const;
    inline bool          m_lte(const self &t) const;
    inline bool          m_gt(const self &t)  const;
    inline bool          m_gte(const self &t) const;
};

bool Range2f::m_lt(const self &t) const
{
    if (math::lt (min, t.min)) return true; else if (math::gt(min, t.min)) return false;
    if (math::lt (max, t.max)) return true; else                           return false;
}

bool Range2f::m_lte(const self &t) const
{
    if (math::lt (min, t.min)) return true; else if (math::gt(min, t.min)) return false;
    if (math::lte(max, t.max)) return true; else                           return false;
}

bool Range2f::m_gt(const self &t) const
{
    if (math::gt (min, t.min)) return true; else if (math::lt(min, t.min)) return false;
    if (math::gt (max, t.max)) return true; else                           return false;
}

bool Range2f::m_gte(const self &t) const
{
    if (math::gt (min, t.min)) return true; else if (math::lt(min, t.min)) return false;
    if (math::gte(max, t.max)) return true; else                           return false;
}

#endif  // _RANGE2F_H
