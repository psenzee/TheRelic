#pragma once

#include <cstdint>
#include <array>

namespace math
{
constexpr const double PI        = 3.141592653589793;
constexpr const double E         = 2.718281828459045;
constexpr const double SQRT2     = 1.414213562373095;
constexpr const double SQRT3     = 1.73205080757;
}

template <typename T, typename Float> constexpr T lerp(const T &a, const T &b, Float t) { return (1 - t) * a + t * b; }

// clamps a value to a range, by default 0-1
template <typename T> constexpr T clamp(const T &a, const T &min, const T &max)         { return a < min ? min : (a > max ? max : a); }
template <typename T> constexpr T clamp(const T &a)                                     { return a < T(0) ? T(0) : (a > T(1) ? T(1) : a); }

// maps a value in the range min-max to a value in the range 0-1/-1-1
template <typename T> constexpr T norm(const T &t, const T &min, const T &max)          { return (t - min) / (max - min); }

// maps a value in the range 0-1 to a value in the range min-max
template <typename T> constexpr T denorm(const T &t, const T &min, const T &max)        { return t * (max - min) + min; }
