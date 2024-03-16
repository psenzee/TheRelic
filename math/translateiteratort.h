#ifndef _TRANSLATEITERATORT_H
#define _TRANSLATEITERATORT_H

#include <math.h>
#include "tuple3t.h"

template <class PointIteratorT>
class TranslateIteratorT
{
    PointIteratorT m_i;
    Tuple3T<float> m_translation;
    
public:

    inline TranslateIteratorT(const PointIteratorT &i) : m_i(i), m_translation(0, 0, 0) {}
    inline TranslateIteratorT(const PointIteratorT &i, const point &translation) : m_i(i), m_translation(translation) {}
    inline TranslateIteratorT<PointIteratorT> &operator=(const TranslateIteratorT<PointIteratorT> &i)
    {
        if (&i != this)
        {
            m_i           = i.m_i;
            m_translation = i.m_translation;
        }
        return *this;
    }
    inline Tuple3T<float> operator*(void) const
    {
        Tuple3T<float> p = *m_i;
        return Tuple3T<float>(p.x + m_translation.x, p.y + m_translation.y, p.z + m_translation.z);
    }
    inline bool operator==(const PointIteratorT &i)                     const    { return m_i == i; }
    inline bool operator!=(const PointIteratorT &i)                     const    { return m_i != i; }
    inline bool operator==(const TranslateIteratorT<PointIteratorT> &i) const    { return m_i == i.m_i; }
    inline bool operator!=(const TranslateIteratorT<PointIteratorT> &i) const    { return m_i != i.m_i; }
    inline TranslateIteratorT<PointIteratorT> &operator++(void)     { ++m_i;       return *this; }
    inline TranslateIteratorT<PointIteratorT> &operator--(void)     { --m_i;       return *this; }
    inline TranslateIteratorT<PointIteratorT> &operator++(int)      { m_i++;       return *this; }
    inline TranslateIteratorT<PointIteratorT> &operator--(int)      { m_i--;       return *this; }
    inline TranslateIteratorT<PointIteratorT> &operator+=(int diff) { m_i += diff; return *this; }
    inline TranslateIteratorT<PointIteratorT> &operator-=(int diff) { m_i -= diff; return *this; }
};

#endif  /** _TRANSLATEITERATORT_H */