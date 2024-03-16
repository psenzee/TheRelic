#pragma once

#include <array>
#include "if_numeric.h"

template <typename T> constexpr IF_FLOAT_T(T) det(T a, T b, T c, T d) { return a * d - b * c; }

template <typename T> constexpr IF_FLOAT_T(T) det(const std::array<T, 4> &a) { return det(a[0], a[1], a[2], a[3]); }

template <typename T> constexpr IF_FLOAT_T(T) det(T a, T b, T c, T d, T e, T f, T g, T h, T i)
{
    return a * (e * i - f * h) + b * (f * g - d * i) + c * (d * h - e * g);
}

template <typename T> constexpr IF_FLOAT_T(T) det(const std::array<T, 9> &a)
{
    return determinant(a[0], a[1], a[2], a[3], a[4], a[5], a[6], a[7], a[8]);
}

template <typename T> constexpr IF_FLOAT_T(T) det(
    T a, T b, T c, T d,
    T e, T f, T g, T h,
    T i, T j, T k, T l,
    T m, T n, T o, T p
) {
    return 
        a * det(f, g, h, j, k, l, n, o, p) +
        b * det(g, h, e, k, l, i, o, p, m) +
        c * det(h, e, f, l, i, j, p, m, n) +
        d * det(e, f, g, i, j, k, m, n, o);
}

template <typename T> constexpr IF_FLOAT_T(T) det(const std::array<T, 16> &a)
{
    return det(
        a[ 0], a[ 1], a[ 2], a[ 3],
        a[ 4], a[ 5], a[ 6], a[ 7],
        a[ 8], a[ 9], a[10], a[11],
        a[12], a[13], a[14], a[15]
    );
}
