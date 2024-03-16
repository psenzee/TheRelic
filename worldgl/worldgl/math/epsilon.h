#pragma once

#include <limits>
#include <cmath>
#include <cstdlib>

#include "if_numeric.h"

// taken from cppreference.com
template <typename T>
constexpr IF_FLOAT_T(bool) equal_within_ulps(T x, T y, std::size_t n)
{
    typedef std::numeric_limits<T> nl_t;

    // Since `epsilon()` is the gap size (ULP, unit in the last place)
    // of floating-point numbers in interval [1, 2), we can scale it to
    // the gap size in interval [2^e, 2^{e+1}), where `e` is the exponent
    // of `x` and `y`.
 
    // If `x` and `y` have different gap sizes (which means they have
    // different exponents), we take the smaller one. Taking the bigger
    // one is also reasonable, I guess.
    const T m = std::min(std::fabs(x), std::fabs(y));
 
    // Subnormal numbers have fixed exponent, which is `min_exponent - 1`.
    const int exp = m < nl_t::min() ? nl_t::min_exponent - 1 : std::ilogb(m);
 
    // We consider `x` and `y` equal if the difference between them is
    // within `n` ULPs.
    return std::fabs(x - y) <= n * std::ldexp(nl_t::epsilon(), exp);
}

template <typename T> constexpr IF_FLOAT_T(T) epsilon()             { return std::numeric_limits<T>::epsilon(); }

template <typename T> constexpr IF_FLOAT_T(T) eps(T a)
{
    constexpr T ea = std::fabs(a) + 1;
    return std::isinf(ea) ? epsilon<T>() : epsilon<T>() * ea;
}

template <typename T> inline IF_FLOAT_T(int)  eps_sign(T a)         { T e = eps(a); return a < -e ? -1 : (a > e ? 1 : 0); }
template <typename T> inline IF_FLOAT_T(bool) eps_sign_eq(T a, T b) { return eps_sign(a) == eps_sign(b); }
template <typename T> inline IF_FLOAT_T(bool) eps_eq(T a, T b)      { return a == b || std::fabs(a - b) <= eps(a); }
template <typename T> inline IF_FLOAT_T(bool) eps_lt(T a, T b)      { return a - b < -eps(a); }
template <typename T> inline IF_FLOAT_T(bool) eps_gt(T a, T b)      { return a - b >  eps(a); }
template <typename T> inline IF_FLOAT_T(bool) eps_lte(T a, T b)     { return a - b <  eps(a); }
template <typename T> inline IF_FLOAT_T(bool) eps_gte(T a, T b)     { return a - b > -eps(a); }

template <typename T> inline IF_FLOAT_T(bool) eps_gt_0(T a)         { return a >  eps(a); }
template <typename T> inline IF_FLOAT_T(bool) eps_gte_0(T a)        { return a > -eps(a); }
template <typename T> inline IF_FLOAT_T(bool) eps_lt_0(T a)         { return a < -eps(a); }
template <typename T> inline IF_FLOAT_T(bool) eps_lte_0(T a)        { return a <  eps(a); }
template <typename T> inline IF_FLOAT_T(bool) eps_eq_0(T a)         { return a == 0 || std::fabs(a) <= eps(a); }
