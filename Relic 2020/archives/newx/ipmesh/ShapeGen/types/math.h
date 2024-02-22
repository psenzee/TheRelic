#ifndef _MATH_H
#define _MATH_H

#include <math.h>
#include <stdlib.h>
#include <vector>

namespace math
{
    
#define EXPLICIT_INLINE __forceinline

enum Sign { NEGATIVE = -1, ZERO = 0, POSITIVE = 1 };

const double                    PI      = 3.1415926535897931;
const double                    TWOPI   = PI * 2.0;
const double                    FOURPI  = PI * 4.0;
const double                    HALFPI  = PI / 2.0;
const double                    EPSILON = 0.0000076;

const unsigned int              I32MAX  = ~((unsigned int)    1    << 31);
const unsigned __int64          I64MAX  = ~((unsigned __int64)1    << 63);
const unsigned __int64          DBLINF  =  ((unsigned __int64)2047 << 52);
const unsigned __int64          DBLNINF =  ((unsigned __int64)1    << 63) | DBLINF;
const unsigned int              FLTINF  =  ((unsigned int)    255  << 23);
const unsigned int              FLTNINF =  ((unsigned int)    1    << 31) | FLTINF;

EXPLICIT_INLINE unsigned int             toint(float a)                               { return *(unsigned int     *)((void *)&a); }
EXPLICIT_INLINE unsigned __int64         toint(double a)                              { return *(unsigned __int64 *)((void *)&a); }
EXPLICIT_INLINE unsigned int            *toint(float *a)                              { return  (unsigned int     *)((void *)a); }
EXPLICIT_INLINE unsigned __int64        *toint(double *a)                             { return  (unsigned __int64 *)((void *)a); }

EXPLICIT_INLINE float                    toval(unsigned int a)                        { return *(float   *)((void *)&a); }
EXPLICIT_INLINE double                   toval(unsigned __int64 a)                    { return *(double  *)((void *)&a); }
EXPLICIT_INLINE float                   *toval(unsigned int *a)                       { return  (float   *)((void *)a); }
EXPLICIT_INLINE double                  *toval(unsigned __int64 *a)                   { return  (double  *)((void *)a); }

EXPLICIT_INLINE float                    abs(float  a)                                { return *toval(&(*toint(&a) &= I32MAX)); }
EXPLICIT_INLINE double                   abs(double a)                                { return *toval(&(*toint(&a) &= I64MAX)); }

EXPLICIT_INLINE bool                     isinf(float  a)                              { return (I32MAX & toint(a)) == FLTINF; }
EXPLICIT_INLINE bool                     isinf(double a)                              { return (I64MAX & toint(a)) == DBLINF; }

EXPLICIT_INLINE double                   random(double lo, double hi)                 { double r = (double)rand() / (double)RAND_MAX; return (hi - lo) * r + lo; }
EXPLICIT_INLINE int                      randsign(void)                               { return rand() <= (RAND_MAX / 2) ? -1 : 1; }

EXPLICIT_INLINE double eps(double a)
{
    const double aa = abs(a) + 1;
	return isinf(aa) ? EPSILON : EPSILON * aa;
}

EXPLICIT_INLINE float eps(float a)
{
    const float aa = abs(a) + 1;
	return isinf(aa) ? (float)EPSILON : (float)EPSILON * aa;
}

EXPLICIT_INLINE bool approxeq(double a, double b)
{
    return (a == b) || (abs(a - b) <= eps(a));
}

EXPLICIT_INLINE bool approxeq(float a, float b)
{
    return (a == b) || (abs(a - b) <= eps(a));
}

EXPLICIT_INLINE bool isnan(double x)
{
    bool b1 = (x < 0.0), b2 = (x >= 0.0), b3 = !(b1 || b2);
	return b3;
}

EXPLICIT_INLINE bool isnan(float x)
{
    bool b1 = (x < 0.0f), b2 = (x >= 0.0f), b3 = !(b1 || b2);
	return b3;
}

EXPLICIT_INLINE Sign sign(float  a)                  { return (a < -eps(a)) ? NEGATIVE : (a > eps(a) ? POSITIVE : ZERO); }
EXPLICIT_INLINE Sign sign(double a)                  { return (a < -eps(a)) ? NEGATIVE : (a > eps(a) ? POSITIVE : ZERO); }

EXPLICIT_INLINE bool eq(float a,  float b)           { return approxeq(a, b); }
EXPLICIT_INLINE bool eq(double a, double b)          { return approxeq(a, b); }

EXPLICIT_INLINE bool lt(float a,  float b)           { return a - b < -eps(a); }
EXPLICIT_INLINE bool lt(double a, double b)          { return a - b < -eps(a); }

EXPLICIT_INLINE bool gt(float a,  float b)           { return a - b >  eps(a); }
EXPLICIT_INLINE bool gt(double a, double b)          { return a - b >  eps(a); }

EXPLICIT_INLINE bool lte(float a,  float b)          { return a - b <  eps(a); }
EXPLICIT_INLINE bool lte(double a, double b)         { return a - b <  eps(a); }

EXPLICIT_INLINE bool gte(float a,  float b)          { return a - b > -eps(a); }
EXPLICIT_INLINE bool gte(double a, double b)         { return a - b > -eps(a); }

EXPLICIT_INLINE bool gt_zero(float a)                { return a > eps(a); }
EXPLICIT_INLINE bool gt_zero(double a)               { return a > eps(a); }

EXPLICIT_INLINE bool lt_zero(float a)                { return a < -eps(a); }
EXPLICIT_INLINE bool lt_zero(double a)               { return a < -eps(a); }

EXPLICIT_INLINE bool zero(float a)                   { return abs(a) <= eps(a); }
EXPLICIT_INLINE bool zero(double a)                  { return abs(a) <= eps(a); }

template <typename T> EXPLICIT_INLINE bool   signeq(const T &a, const T &b)               { return sign(a) == sign(b); }
template <typename T> EXPLICIT_INLINE T      clamp(const T &a, const T &mn, const T &mx)  { return a < mn ? mn : (a > mx ? mx : a); }
template <typename T> EXPLICIT_INLINE T      min  (const T &a, const T &b)                { return a < b ? a : b; }
template <typename T> EXPLICIT_INLINE T      max  (const T &a, const T &b)                { return a > b ? a : b; }
template <typename T> EXPLICIT_INLINE void   swap (T &a, T &b)                            { T t(a); a = b; b = t; }
template <typename T> EXPLICIT_INLINE void   sort2(T &a, T &b)                            { if (a > b) swap(a, b); }
template <typename T> EXPLICIT_INLINE void   sort3(T &a, T &b, T &c)                      { sort2(a, b); sort2(b, c); sort2(a, b); }

template <class T>
std::ostream &printbinary(std::ostream &s, T val)
{
	for (unsigned int i = sizeof(T) * 8; i--;)
        s << ((((T)1 << i) & val) ? "1" : "0");
	return s;
}

//This implementation makes use of the function frexp
//which extracts from a double value its mantissa m
//and exponent e by doing efficient bit manipulations

EXPLICIT_INLINE unsigned int hash(double v)
{
    if (v != 0)
    {
        int exp;
        double mantissa = frexp(v, &exp);
        return (unsigned int)((2 * fabs(mantissa) - 1) * ~0u);
    }
    return 0;
}

EXPLICIT_INLINE unsigned int hash(float v)
{
    if (v != 0)
    {
        int exp;
        float mantissa = (float)frexp(v, &exp);
        return (unsigned int)((2 * fabs(mantissa) - 1) * ~0u);
    }
    return 0;
}

};

#endif  // _MATH_H