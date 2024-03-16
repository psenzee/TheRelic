#ifndef _SCALEITERATORT_H
#define _SCALEITERATORT_H

#include "tuple3t.h"

template <class PointIteratorT>
class ScaleIteratorT
{
    PointIteratorT m_i;
    Tuple3T<float> m_scale;
    
public:
    inline ScaleIteratorT(const PointIteratorT &i) : m_i(i), m_scale(1, 1, 1) {}
    inline ScaleIteratorT(const PointIteratorT &i, const Tuple3T<float> &scale) : m_i(i), m_scale(scale) {}
    inline ScaleIteratorT<PointIteratorT> &operator=(const ScaleIteratorT<PointIteratorT> &i)
    {
        if (&i != this)
        {
            m_i     = i.m_i;
            m_scale = i.m_scale;
        }
        return *this;
    }
    inline Tuple3T<float> operator*(void) const
    {
        Tuple3T<float> p = *m_i;
        return Tuple3T<float>(p.x * m_scale.x, p.y * m_scale.y, p.z * m_scale.z);
    }
    inline bool operator==(const PointIteratorT &i)                 const    { return m_i == i; }
    inline bool operator!=(const PointIteratorT &i)                 const    { return m_i != i; }
    inline bool operator==(const ScaleIteratorT<PointIteratorT> &i) const    { return m_i == i.m_i; }
    inline bool operator!=(const ScaleIteratorT<PointIteratorT> &i) const    { return m_i != i.m_i; }
    inline ScaleIteratorT<PointIteratorT> &operator++(void)     { ++m_i;       return *this; }
    inline ScaleIteratorT<PointIteratorT> &operator--(void)     { --m_i;       return *this; }
    inline ScaleIteratorT<PointIteratorT> &operator++(int)      { m_i++;       return *this; }
    inline ScaleIteratorT<PointIteratorT> &operator--(int)      { m_i--;       return *this; }
    inline ScaleIteratorT<PointIteratorT> &operator+=(int diff) { m_i += diff; return *this; }
    inline ScaleIteratorT<PointIteratorT> &operator-=(int diff) { m_i -= diff; return *this; }
};

#endif  /** _SCALEITERATORT_H */