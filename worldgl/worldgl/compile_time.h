#pragma once

#include <cstddef>
#include <cstdint>

namespace compile_time
{

inline constexpr size_t lg(size_t n) { return n <= 1 ? 0 : (1 + lg(n >> 1)); }

template <typename T> inline constexpr T sq(T u) { return u * u; }

template <typename T> constexpr T isqrt(T num)
{
    T res(0), bit(static_cast<T>(1) << (sizeof(T) * 8 - 2));
    while (bit > num)
        bit >>= 2;
    while (bit) {
        T delta(res + bit);
        if (num >= delta) {
            num -= delta;
            res = (res >> 1) + bit;
        } else {
            res >>= 1;
        }
        bit >>= 2;
    }
    return res;
}

}
