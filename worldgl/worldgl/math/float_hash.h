#pragma once

#include <cstddef>
#include <cmath>

#include "if_numeric.h"

// This implementation makes use of the function frexp
// which extracts from a double value its mantissa m
// and exponent e by doing efficient bit manipulations

template <typename T> inline IF_FLOAT_T(uint64_t) float_hash(T v)
{
    if (v != 0) {
        int exp = 0;
        return (2 * std::fabs(std::frexp(v, &exp)) - 1) * ~uint64_t(0);
    }
    return 0;
}
