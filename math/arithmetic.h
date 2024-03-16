#ifndef _ARITHMETIC_H
#define _ARITHMETIC_H

#include "math.h"
#include "primes64k.h"
#include "bits.h"

#include <exception>

namespace math
{

template <class IntegerT>
inline bool isfactor(IntegerT v, IntegerT factor)
{
    return (v / factor) * factor == v;
}

template <class IntegerT>
inline IntegerT lcm(IntegerT a, IntegerT b)
{
    if (a == b)
        return a;
    else if (a == 0 || b == 0)
        return 0; // ???
    IntegerT inc = max(a, b), other = min(a, b);
    for (IntegerT i = inc, e = a * b; i < e; i += inc)
        if (isfactor(i, other))
            return i;
    return a * b;
}

template <class IntegerT>
inline IntegerT primefactor(IntegerT a, IntegerT b)
{
    IntegerT v = min(a, b), p = 0, t = 1;
    for (IntegerT i = 0; (p = math::primes[i]) <= v; i++)
        if (isfactor(a, p) && isfactor(b, p))
            return p;
	return 1;
}

template <class IntegerT>
inline IntegerT gcf(IntegerT a, IntegerT b)
{
    IntegerT f = 1, pf = 0;
    while (a > 1 && b > 1)
	{
	    pf = primefactor(a, b);
		if (pf == 1)
		    return f;
		f *= pf; a /= pf; b /= pf;
	}
    return f;
}

// generalized
inline unsigned int divide(unsigned int n, unsigned int divisor)
{
    if (!n) return 0;
    if (!divisor)
        throw std::exception("division by zero.");

    int          bits = bithi(n) - (bithi(divisor) + 1);
    unsigned int or   = 1       << bits,
                 div  = divisor << bits,
                 quo  = 0;
    while (n >= divisor)
    {
        if (n >= div)
        {
            n -= div;
            quo |= or;
        }
        div >>= 1;
        or  >>= 1;
    }
    return quo;
}

unsigned int div7(unsigned int n)
{
	return (unsigned int)(((unsigned __int64)n * 613566757) >> 32);
}

}


#include <iostream>

namespace math
{

void arithmetic_test(void)
{
    std::cout << math::divide(141, 7) << std::endl;

    std::cout << math::lcm(3, 4) << std::endl;
	std::cout << math::lcm(24, 4) << std::endl;
	std::cout << math::lcm(64, 56) << std::endl;
	std::cout << math::lcm(7, 8) << std::endl;

	std::cout << math::gcf(3, 4) << std::endl;
	std::cout << math::gcf(24, 4) << std::endl;
	std::cout << math::gcf(1000, 224) << std::endl;

    std::cout << math::ispow2(64) << ' ' << math::ispow2(56) << std::endl;
}

};

#endif  /** _ARITHMETIC_H */