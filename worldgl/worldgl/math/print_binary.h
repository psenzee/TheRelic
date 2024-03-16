#pragma once

#include <iostream>

#include "if_number.h"

template <typename T> IF_INT_T(std::ostream) &print_binary(std::ostream &s, T v)
{
    for (size_t i = sizeof(T) * 8; i--;) {
        s << (((T(1) << i) & v) ? "1" : "0");
    }
	return s;
}
