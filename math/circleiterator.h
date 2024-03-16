#ifndef _CIRCLEITERATOR_H
#define _CIRCLEITERATOR_H

#include <math.h>
#include "tuple3t.h"

class CircleIterator
{
    double       m_dc;
    unsigned int m_steps,
                 m_at;
    bool         m_ccw;

public:

    inline CircleIterator(const CircleIterator &i) : m_steps(i.m_steps), m_at(i.m_at), m_dc(i.m_dc), m_ccw(true) {}
    inline CircleIterator(unsigned int steps, unsigned int at = 0) : m_steps(steps), m_at(at), m_dc(2.0 * PI / (double)steps), m_ccw(true) {}

    inline CircleIterator &operator=(const CircleIterator &i)
    {
        m_steps = i.m_steps;
        m_at    = i.m_at;
        m_dc    = i.m_dc;
        m_ccw   = i.m_ccw;
        return *this;
    }
    inline bool operator==(const CircleIterator &i) const { return m_steps == i.m_steps && m_at == i.m_at && m_ccw == i.m_ccw; }
    inline bool operator!=(const CircleIterator &i) const { return m_steps != i.m_steps || m_at != i.m_at || m_ccw != i.m_ccw; }
    inline Tuple3T<float> operator*(void) const
    {
        double u = m_dc * (m_ccw ? (double)m_at : (double)(m_steps - m_at));
        return point((float)sin(u), (float)cos(u), 0);
    }
    inline CircleIterator &operator++(void)               { m_at++; return *this; }
    inline CircleIterator &operator--(void)               { m_at--; return *this; }
    inline CircleIterator  operator++(int)                { CircleIterator copy(*this); m_at++; return copy; }
    inline CircleIterator  operator--(int)                { CircleIterator copy(*this); m_at--; return copy; }
    inline CircleIterator &operator+=(int diff)           { m_at = (m_at + diff) % m_steps; return *this; }
    inline CircleIterator &operator-=(int diff)           { m_at = (m_at - diff) % m_steps; return *this; }
};

#endif  /** _CIRCLEITERATOR_H */