#pragma once

#include <type_traits>

#define IF_FLOAT_T(RETURN_T) std::enable_if_t<std::is_floating_point_v<T>, RETURN_T>
#define IF_INT_T(RETURN_T)   std::enable_if_t<std::is_integral_v<T>, RETURN_T>
