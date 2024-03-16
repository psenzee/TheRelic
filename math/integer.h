#ifndef _INTEGER_H
#define _INTEGER_H

#define OVERFLOW_EXCEPTION

#include <exception>

namespace math
{

class overflow_exception_t : public std::exception {};

template <typename UIntT>
inline UIntT multiply(UIntT a, UIntT b)
{
#ifdef OVERFLOW_EXCEPTION
    if (b && ((a * b) / b) != a)
        throw overflow_exception_t();
#endif
    return a * b;
}

unsigned __int64 factorial(unsigned hi, unsigned lo = 0)
{
    unsigned __int64 f = 1;
    if (lo < 1) lo = 1;
    while (hi > lo)
        f = multiply(f, (unsigned __int64)hi--);
    return f;
}

unsigned __int64 choose(unsigned n, unsigned k)
{
    if (k > n || k < 0) return 0;
    unsigned a = n - k, b = k;
    if (k > n - k) { a = k; b = n - k; }
    return factorial(n, a) / factorial(b, 1);
}

unsigned __int64 permutations(unsigned n, unsigned k)
{
    if (k > n || k < 0) return 0;
    return factorial(n, n - k);
}

}

#endif  /** _INTEGER_H */