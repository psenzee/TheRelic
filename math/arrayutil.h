#ifndef _ARRAYUTIL_H
#define _ARRAYUTIL_H

template <class T>
class ArrayUtil
{
    ArrayUtil() {} // no construction

public:

	inline static T    clamp(T e, T lo, T hi) { return e < lo ? lo : (e > hi ? hi : e); }
	inline static T    min(T a, T b)          { return a < b ? a : b; }
	inline static T    max(T a, T b)          { return a > b ? a : b; }

    inline static T    scalar(const T *d, const T *size, int c)
    {
        T sc = 0;
        for (int i = c - 1; i >= 0; i--)
            sc += (i == 0 ? 1 : size[i - 1]) * d[i];
        return sc;
    }
    inline static void clamp(T *d, const T *e, T lo, T hi, int c)
	{
		for (int i = 0; i < c; i++)
			d[i] = clamp(e[i]. lo, hi);
	}
    inline static void clamp(T *d, const T *e, const T *lo, const T *hi, int c)
	{
		for (int i = 0; i < c; i++)
			d[i] = clamp(e[i]. lo[i], hi[i]);
	}

    inline static bool eq(const T *d, const T *v, int c)         { for (int i = 0; i < c; i++) if (d[i] != v[i]) return false; return true; }
    inline static bool eq(const T *d, T v, int c)                { for (int i = 0; i < c; i++) if (d[i] != v) return false; return true; }

    inline static T    product(const T *d, int c)                { T p = 1; for (int i = 0; i < c; i++) p *= d[i]; return p; }
    inline static T    sum(const T *d, int c)                    { T s = 0; for (int i = 0; i < c; i++) s += d[i]; return s; }
    inline static T    productsum(const T *d, const T *e, int c) { T s = 0; for (int i = 0; i < c; i++) s += d[i] * e[i]; return s; }

    inline static void set(T *d, const T *v, int c)              { for (int i = 0; i < c; i++) d[i] = v[i]; }
    inline static void set(T *d, const T  v, int c)              { for (int i = 0; i < c; i++) d[i] = v;    }
    inline static void add(T *d, const T *e, const T *v, int c)  { for (int i = 0; i < c; i++) d[i] = e[i] + v[i]; }
    inline static void add(T *d, const T *e, const T  v, int c)  { for (int i = 0; i < c; i++) d[i] = e[i] + v;    }
    inline static void sub(T *d, const T *e, const T *v, int c)  { for (int i = 0; i < c; i++) d[i] = e[i] - v[i]; }
    inline static void sub(T *d, const T *e, const T  v, int c)  { for (int i = 0; i < c; i++) d[i] = e[i] - v;    }
    inline static void mul(T *d, const T *e, const T *v, int c)  { for (int i = 0; i < c; i++) d[i] = e[i] * v[i]; }
    inline static void mul(T *d, const T *e, const T  v, int c)  { for (int i = 0; i < c; i++) d[i] = e[i] * v;    }
    inline static void div(T *d, const T *e, const T *v, int c)  { for (int i = 0; i < c; i++) d[i] = e[i] / v[i]; }
    inline static void div(T *d, const T *e, const T  v, int c)  { for (int i = 0; i < c; i++) d[i] = e[i] / v;    }
    inline static void mod(T *d, const T *e, const T *v, int c)  { for (int i = 0; i < c; i++) d[i] = e[i] % v[i]; }
    inline static void mod(T *d, const T *e, const T  v, int c)  { for (int i = 0; i < c; i++) d[i] = e[i] % v;    }
    inline static void min(T *d, const T *e, const T *v, int c)  { for (int i = 0; i < c; i++) d[i] = min(e[i], v[i]); }
    inline static void min(T *d, const T *e, const T  v, int c)  { for (int i = 0; i < c; i++) d[i] = min(e[i], v);    }
    inline static void max(T *d, const T *e, const T *v, int c)  { for (int i = 0; i < c; i++) d[i] = max(e[i], v[i]); }
    inline static void max(T *d, const T *e, const T  v, int c)  { for (int i = 0; i < c; i++) d[i] = max(e[i], v);    }

    inline static void add(T *d, const T *v, int c)              { add(d, d, v, c); }
    inline static void add(T *d, const T  v, int c)              { add(d, d, v, c); }
    inline static void sub(T *d, const T *v, int c)              { sub(d, d, v, c); }
    inline static void sub(T *d, const T  v, int c)              { sub(d, d, v, c); }
    inline static void mul(T *d, const T *v, int c)              { mul(d, d, v, c); }
    inline static void mul(T *d, const T  v, int c)              { mul(d, d, v, c); }
    inline static void div(T *d, const T *v, int c)              { div(d, d, v, c); }
    inline static void div(T *d, const T  v, int c)              { div(d, d, v, c); }
    inline static void mod(T *d, const T *v, int c)              { mod(d, d, v, c); }
    inline static void mod(T *d, const T  v, int c)              { mod(d, d, v, c); }
    inline static void min(T *d, const T *v, int c)              { min(d, d, v, c); }
    inline static void min(T *d, const T  v, int c)              { min(d, d, v, c); }
    inline static void max(T *d, const T *v, int c)              { max(d, d, v, c); }
    inline static void max(T *d, const T  v, int c)              { max(d, d, v, c); }
};

typedef ArrayUtil<float> arrays;

#endif  /** _ARRAYUTIL_H */