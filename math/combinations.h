#ifndef _COMBINATIONS_H
#define _COMBINATIONS_H

#include "math/integer.h"

namespace math
{

template <typename T>
int enumerate_combinations(int n, int r, T *v)
{
    struct { int n, r; T h; } s[sizeof(T) * 8] = { { n, r, 0 } }, q;
    int si  = 1, i = 0;
    T   one = 1;

    while (si)
    {
        q = s[--si];

    tail:

        if (q.r != 0)
        {
            one = 1;
            if (q.r == 1)
            {
                for (int j = 0; j < q.n; j++)
                {
                    v[i++] = q.h | one;
                    one <<= 1;
                }
            }
            else if (q.r == q.n)
            {
                v[i++] = q.h | (one << q.n) - 1;
            }
            else
            {
                --q.n; s[si++] = q; q.r--;
                q.h |= one << q.n;
                goto tail;
            }
        }
    }

    return i;
}

int enumerate_combinations2(int n, int r, unsigned __int64 *v)
{
    unsigned __int64 smallest, ripple, x;
    unsigned         count = (unsigned)math::choose(n, r);
    v[0] = ((unsigned __int64)1 << r) - 1;
    for (unsigned i = 1; i < count; i++)
    {
        x        = v[i - 1];
        smallest = x & -(__int64)x;
        ripple   = x + smallest;
        v[i]     = ripple | (((x ^ ripple) >> 2) / smallest);
    }
    return count;
}

template <typename T>
class ComboIterator
{
    struct { int n, r; T h; } m_s[sizeof(T) * 8], m_q;
    int m_si, m_i, m_j;

public:

    inline ComboIterator(int n, int r) : m_si(1), m_i(0), m_j(0)
    {
        m_q.n = n; m_q.r = r; m_q.h = 0;
        m_s[0] = m_q;
    }
    inline bool done() const
    {
        return !m_si;
    }
    T next()
    {
        T r = 0;
        while (m_si)
        {
            if (!m_j || m_j >= m_q.n)
            {
                m_j = 0; m_q = m_s[--m_si];
            }
            if (m_q.r != 0)
            {
                T one = 1;
                if (m_q.r == 1)
                {
                    r = m_q.h | (one << m_j++);
                }
                else if (m_q.r == m_q.n)
                {
                    r = m_q.h | (one << m_q.n) - 1;
                }
                else
                {
                    --m_q.n; m_s[m_si++] = m_q; m_q.r--;
                    m_q.h |= one << m_q.n;
                    m_s[m_si++] = m_q;
                    continue;
                }
            }
            break;
        }
        return r;
    }
};

}

#endif  /** _COMBINATIONS_H */