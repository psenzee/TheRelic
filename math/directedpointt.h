#ifndef _DIRECTEDPOINTT_H
#define _DIRECTEDPOINTT_H

#include "math/tuple3t.h"
#include "math/transformt.h"

template <typename T>
class DirectedPointT
{
public:
    typedef Tuple3T<T> vector_t;

    vector_t position,
             vector;

    inline DirectedPointT(const vector_t &p, const vector_t &v) : position(p), vector(v) {}

    inline void transform(const TransformT<T> &xf)
    {
        position = xf.point(position);
        vector   = xf.invvectortranspose(vector);
    }
};

#endif  /** _DIRECTEDPOINTT_H */