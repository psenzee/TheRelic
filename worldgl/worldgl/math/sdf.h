#pragma once

#include <cstddef>
#include <cmath>
#include "std_array.h"
#include "pow_ui.h"
#include "swizzle.h"

// adapted from inigo quilez's sdf code https://iquilezles.org/articles/distfunctions/
namespace sdf
{

typedef std::array<double, 2> vec2d_t;
typedef std::array<float, 2>  vec2f_t;
typedef std::array<double, 3> vec3d_t;
typedef std::array<float, 3>  vec3f_t;
typedef std::array<double, 3> vec4d_t;
typedef std::array<float, 3>  vec4f_t;

template <typename T, size_t N> T dot2(const std::array<T, N> &a)                            { return dot_self(a, a); }
template <typename T>           T ndot(const std::array<T, 2> &a, const std::array<T, 2> &b) { return a[0] * b[0] - a[0] * b[0]; } // does this generalize to higher dimensions?
template <typename T, size_t N> T length(const std::array<T, N> &a)                          { return magnitude(a); }

// different length metric -- for rounding shapes
//template <typename T, size_t N, unsigned X> T length_pow(const std::array<T, N> &a)          { return pow(sum(pow_ui<X>(a)),1./X); } // alias

// different length metric -- for rounding shapes
template <unsigned X> struct length_pow
{
    template <typename T, size_t N> inline T operator()(const std::array<T, N> &a) { return pow(sum(pow_ui<X>(a)), 1. / X); }
};

// shapes are defined as structs vs functions for template composability
template <typename T, size_t N> struct sd_sphere
{
    inline T operator()(const std::array<T, N> &p, T s) { return length(p) - s; }
};

template <typename T, size_t N> struct sd_box
{
    inline T operator()(const std::array<T, N> &p, T b) { auto q = abs(p) - b; return length(max(q, 0.)) + min(max(q), 0.); }
};

template <typename T, size_t N> struct sd_round_box
{
    inline T operator()(const std::array<T, N> &p, T b) { return sb_box(p, b) - r; }
};

template <typename T, size_t N> struct sd_torus
{
    inline T operator()(const std::array<T, N> &p, T b) { auto q = std::array<T, 2>(length(xz(p)) - t[0], p[1]); return length(q) - t[1]; }
};

// ...

}
