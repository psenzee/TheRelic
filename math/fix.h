#ifndef _FIX_H
#define _FIX_H

// 64 bit fixed point..

#include <iostream>

class fix
{
    enum { PRECISION = 32 };

public:

    inline fix()               : m_v(0)                     {}
    inline fix(__int64 v)      : m_v(fix::from(v))          {}
    inline fix(int v)          : m_v(fix::from((__int64)v)) {}
    inline fix(unsigned int v) : m_v(fix::from((__int64)v)) {}
    inline fix(short v)        : m_v(fix::from((__int64)v)) {}
    inline fix(float v)        : m_v(fix::from((double)v))  {}
    inline fix(double v)       : m_v(fix::from(v))          {}
    inline fix(const fix &v)   : m_v(v.m_v)                 {}

    inline fix   &operator*=(const fix &v)        { m_v = fpmul(m_v, v.m_v); return *this; }
    inline fix    operator* (const fix &v)  const { return fpvalue(fpmul(m_v, v.m_v)); }
    inline fix   &operator/=(const fix &v)        { m_v = fpdiv(m_v, v.m_v); return *this; }
    inline fix    operator/ (const fix &v)  const { return fpvalue(fpdiv(m_v, v.m_v)); }
    inline fix   &operator+=(const fix &v)        { m_v += v.m_v; return *this; }
    inline fix    operator+ (const fix &v)  const { fix f(*this); return f += v; }
    inline fix   &operator-=(const fix &v)        { m_v -= v.m_v; return *this; }
    inline fix    operator- (const fix &v)  const { fix f(*this); return f -= v; }

    inline fix    operator- (void)          const { return fpvalue(-m_v); }

    inline bool   operator==(const fix &v)  const { return m_v == v.m_v; }
    inline bool   operator!=(const fix &v)  const { return m_v != v.m_v; }
    inline bool   operator< (const fix &v)  const { return m_v <  v.m_v; }
    inline bool   operator<=(const fix &v)  const { return m_v <= v.m_v; }
    inline bool   operator> (const fix &v)  const { return m_v >  v.m_v; }
    inline bool   operator>=(const fix &v)  const { return m_v >= v.m_v; }

    inline bool   operator==(int v)         const { return m_v == fix::from((__int64)v); }
    inline bool   operator!=(int v)         const { return m_v != fix::from((__int64)v); }
    inline bool   operator< (int v)         const { return m_v <  fix::from((__int64)v); }
    inline bool   operator<=(int v)         const { return m_v <= fix::from((__int64)v); }
    inline bool   operator> (int v)         const { return m_v >  fix::from((__int64)v); }
    inline bool   operator>=(int v)         const { return m_v >= fix::from((__int64)v); }

    inline bool   operator==(double v)      const { return m_v == fix::from(v); }
    inline bool   operator!=(double v)      const { return m_v != fix::from(v); }
    inline bool   operator< (double v)      const { return m_v <  fix::from(v); }
    inline bool   operator<=(double v)      const { return m_v <= fix::from(v); }
    inline bool   operator> (double v)      const { return m_v >  fix::from(v); }
    inline bool   operator>=(double v)      const { return m_v >= fix::from(v); }

    inline bool   operator==(float v)       const { return m_v == fix::from((double)v); }
    inline bool   operator!=(float v)       const { return m_v != fix::from((double)v); }
    inline bool   operator< (float v)       const { return m_v <  fix::from((double)v); }
    inline bool   operator<=(float v)       const { return m_v <= fix::from((double)v); }
    inline bool   operator> (float v)       const { return m_v >  fix::from((double)v); }
    inline bool   operator>=(float v)       const { return m_v >= fix::from((double)v); }

    inline fix   &operator= (short v)             { m_v = from((__int64)v); return *this; }
    inline fix   &operator= (int v)               { m_v = from((__int64)v); return *this; }
    inline fix   &operator= (__int64 v)           { m_v = from(v);          return *this; }
    inline fix   &operator= (float v)             { m_v = from((double)v);  return *this; }
    inline fix   &operator= (double v)            { m_v = from(v);          return *this; }
    inline fix   &operator= (const fix &v)        { m_v = v.m_v;            return *this; }

    inline operator __int64 ()              const { return toint(m_v); }
    inline operator int     ()              const { return (int)toint(m_v); }
    inline operator short   ()              const { return (short)toint(m_v); }
    inline operator float   ()              const { return (float)toreal(m_v); }
    inline operator double  ()              const { return toreal(m_v); }

	inline __int64    fpvalue(void)         const { return m_v; }
	inline static fix fpvalue(__int64 v)          { fix f; f.m_v = v; return f; }

	inline fix        abs(void)             const { return m_v < 0 ? fpvalue(-m_v) : *this; }

private:

    __int64 m_v;

    inline static double  fmul(void)          { static double v = (double)(((__int64)1) << PRECISION); return v; }
    inline static double  finvmul(void)       { static double v = 1 / fmul(); return v; }

    inline static __int64 from(__int64 v)     { return v << PRECISION; }
    inline static __int64 from(double v)      { return (__int64)(fmul() * v); }

    inline static __int64 toint(__int64 v)    { return v >> PRECISION; }
    inline static double  toreal(__int64 v)   { return finvmul() * (double)v; }

public:

    inline static void fpmul128(__int64 a, __int64 b, unsigned __int64 &c, unsigned __int64 &d, bool &s)
    {
    	unsigned __int64 al, ah, bl, bh, albl, ahbl, albh, ahbh,
		                                 x,     y,    z,    w;
		s = false;
    	if (!a || !b) c = d = 0;
    	if (a < 0) { a = -a; s  = true; }
    	if (b < 0) { b = -b; s ^= true; }

    	al = (unsigned __int64)(a & ~0u);
    	ah = (unsigned __int64)(a >> 32);
    	bl = (unsigned __int64)(b & ~0u);
    	bh = (unsigned __int64)(b >> 32);

        albl = al * bl; ahbl = ah * bl;
        albh = al * bh; ahbh = ah * bh;

		w = albl & ~0u;
		z =             (albl >> 32) + (ahbl & ~0u) + (albh & ~0u);
		y =                            (ahbl >> 32) + (albh >> 32) + (ahbh & ~0u);
		x =                                                                        ahbh >> 32;

        c = 0;
		z <<= 32;
		d = z + w;
		if (d < z || d < w) c++; // carry..
		x <<= 32;
		c += x + y;
    }

    inline static __int64 fpmul(__int64 a, __int64 b)
    {
    	unsigned __int64 c, d;
		bool s;
		fpmul128(a, b, c, d, s);
		return (s * -2 + 1) * ((c << 32) | (d >> 32));
    }

    inline static __int64 fpdiv(__int64 a, __int64 b)
    {
    	unsigned __int64 r;
    	bool sign = false;

    	if (a < 0) { a = -a; sign  = true; }
    	if (b < 0) { b = -b; sign ^= true; }

    	r = a % b;
    	a = a / b;

    	for (int i = 32; i--;)
    	{
    		r <<= 1;
    		a <<= 1;
    		if (r >= (unsigned __int64)b)
    		{
    			r -= (unsigned __int64)b;
    			a++;
    		}
    	}
    	return (sign * -2 + 1) * a;
    }
    inline static double fpmultest(double a, double b)
    {
    	double m = (double)((__int64)1 << 32);
    	return (double)fpmul((__int64)(a * m), (__int64)(b * m)) / m;
    }

    inline static double fpdivtest(double a, double b)
    {
    	double m = (double)((__int64)1 << 32);
    	return (double)fpdiv((__int64)(a * m), (__int64)(b * m)) / m;
    }
};

inline std::ostream &operator<<(std::ostream &s, const fix &v) { return s << (double)v; }

// slow but accurate
inline fix sqrt(const fix &value)
{
    static __int64 one = (int)(fix(1).fpvalue());
    __int64 v = value.fpvalue(), root, next;

    if (v < one) return 0;
    next = v >> 2;

    do { root = next; next = (next + fix::fpdiv(v, next)) >> 1; }
	while (root != next);

    return fix::fpvalue(root);
}

#endif  /** _FIX_H */