#ifndef _REALT_H
#define _REALT_H

#include "math.h"

template <class T>
class RealT
{
    T m_value;

public:

    inline RealT() {}
    inline RealT(T u) : m_value(u) {}

    inline           operator T ()                 const { return m_value; }

    inline RealT<T> &operator= (T u)                     { m_value  = u; return *this; }
    inline RealT<T> &operator+=(T u)                     { m_value += u; return *this; }
    inline RealT<T> &operator-=(T u)                     { m_value -= u; return *this; }
    inline RealT<T> &operator*=(T u)                     { m_value *= u; return *this; }
    inline RealT<T> &operator/=(T u)                     { m_value /= u; return *this; }

    inline RealT<T> &operator= (const RealT<T> &u)       { m_value  = u.m_value; return *this; }
    inline RealT<T> &operator+=(const RealT<T> &u)       { m_value += u.m_value; return *this; }
    inline RealT<T> &operator-=(const RealT<T> &u)       { m_value -= u.m_value; return *this; }
    inline RealT<T> &operator*=(const RealT<T> &u)       { m_value *= u.m_value; return *this; }
    inline RealT<T> &operator/=(const RealT<T> &u)       { m_value /= u.m_value; return *this; }

    inline RealT<T> &operator++(void)                    { m_value++; return *this; }
    inline RealT<T> &operator--(void)                    { m_value--; return *this; }

    inline RealT<T>  operator-(void)               const { return -m_value; }

    inline RealT<T>  operator+(T u)                const { return m_value + u; }
    inline RealT<T>  operator-(T u)                const { return m_value - u; }
    inline RealT<T>  operator*(T u)                const { return m_value * u; }
    inline RealT<T>  operator/(T u)                const { return m_value / u; }

    inline RealT<T>  operator+(const RealT<T> &u)  const { return m_value + u.m_value; }
    inline RealT<T>  operator-(const RealT<T> &u)  const { return m_value - u.m_value; }
    inline RealT<T>  operator*(const RealT<T> &u)  const { return m_value * u.m_value; }
    inline RealT<T>  operator/(const RealT<T> &u)  const { return m_value / u.m_value; }

    inline bool      operator==(T u)               const { return  math::eq(m_value, u); }
    inline bool      operator!=(T u)               const { return !math::eq(m_value, u); }
    inline bool      operator< (T u)               const { return  math::lt(m_value, u); }
    inline bool      operator> (T u)               const { return  math::gt(m_value, u); }
    inline bool      operator<=(T u)               const { return math::lte(m_value, u); }
    inline bool      operator>=(T u)               const { return math::gte(m_value, u); }

    inline bool      operator==(const RealT<T> &u) const { return  math::eq(m_value, u.m_value); }
    inline bool      operator!=(const RealT<T> &u) const { return !math::eq(m_value, u.m_value); }
    inline bool      operator< (const RealT<T> &u) const { return  math::lt(m_value, u.m_value); }
    inline bool      operator> (const RealT<T> &u) const { return  math::gt(m_value, u.m_value); }
    inline bool      operator<=(const RealT<T> &u) const { return !math::gt(m_value, u.m_value); }
    inline bool      operator>=(const RealT<T> &u) const { return !math::lt(m_value, u.m_value); }
};

#include <iostream>

template <class T> std::ostream &operator<<(std::ostream &s, const RealT<T> &u) { return s << (T)u; }

typedef RealT<float>  real;
typedef RealT<double> reald;

#endif  /** _REALT_H */