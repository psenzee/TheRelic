#ifndef _FIXED_H
#define _FIXED_H

class fixed
{
    int m_v;
    enum { SHIFT = sizeof(unsigned short) * 8, LOMASK = (1 << SHIFT) - 1 };

    static const int     MUL;
	static const __int64 RANGE;
    static const double  IMUL;

public:

    inline fixed()               : m_v(0)                              {}
    inline fixed(int v)          : m_v(v << SHIFT)                     {}
    inline fixed(float v)        : m_v((int)((double)MUL * (double)v)) {}
    inline fixed(double v)       : m_v((int)((double)MUL * v))         {}
    inline fixed(const fixed &v) : m_v(v.m_v)                          {}

    inline fixed &operator*=(const fixed &v)
	{
        __int64 u = ((__int64)m_v * (__int64)v.m_v) >> SHIFT;
#ifdef CHECK_RANGES
		if (u > RANGE || u < -RANGE)
		    throw std::exception("fixed::operator*=() : overflow.");
#endif
		m_v = (int)u;
		return *this;
    }
    inline fixed  operator* (const fixed &v) const { fixed f(*this); return f *= v; }
    inline fixed &operator/=(const fixed &v)
	{
	    __int64 u = (((__int64)m_v << SHIFT) / (__int64)v.m_v);
#ifdef CHECK_RANGES
		if (u > RANGE || u < -RANGE)
		    throw std::exception("fixed::operator/=() : overflow.");
#endif
	    m_v = (int)u;
		return *this;
    }
    inline fixed  operator/ (const fixed &v) const { fixed f(*this); return f /= v; }
    inline fixed &operator+=(const fixed &v)       { m_v += v.m_v; return *this; }
    inline fixed  operator+ (const fixed &v) const { fixed f(*this); return f += v; }
    inline fixed &operator-=(const fixed &v)       { m_v -= v.m_v; return *this; }
    inline fixed  operator- (const fixed &v) const { fixed f(*this); return f -= v; }

    inline bool   operator==(const fixed &v) const { return m_v == v.m_v; }
    inline bool   operator!=(const fixed &v) const { return m_v != v.m_v; }
    inline bool   operator< (const fixed &v) const { return m_v <  v.m_v; }
    inline bool   operator<=(const fixed &v) const { return m_v <= v.m_v; }
    inline bool   operator> (const fixed &v) const { return m_v >  v.m_v; }
    inline bool   operator>=(const fixed &v) const { return m_v >= v.m_v; }

    inline fixed &operator= (int v)                { m_v = v << SHIFT; return *this; }
    inline fixed &operator= (float v)              { m_v = (int)(MUL * (double)v); return *this; }
    inline fixed &operator= (double v)             { m_v = (int)(MUL * v); return *this; }
    inline fixed &operator= (const fixed &v)       { m_v = v.m_v; return *this; }

    inline operator int  ()                  const { return m_v >> SHIFT; }
    inline operator float  ()                const { return (float)(double)(*this); }
    inline operator double  ()               const { return (m_v >> SHIFT) + (double)(m_v & LOMASK) * IMUL; }
};

inline std::ostream &operator<<(std::ostream &s, const fixed &v) { return s << (double)v; }

const int     fixed::MUL   = (int)1 << fixed::SHIFT;
const __int64 fixed::RANGE = (__int64)1 << (sizeof(int) * 8 - 1);
const double  fixed::IMUL  = 1.0 / (double)fixed::MUL;

#endif  /** _FIXED_H */