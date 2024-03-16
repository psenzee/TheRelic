#ifndef _AFFINEELEMENTT_H
#define _AFFINEELEMENTT_H

#include "math/matrix4t.h"
#include "math/tuple3t.h"
#include "math/transformt.h"

template <typename T>
class AffineElementT
{
public:
    typedef Matrix4T<T> matrix_t;
    typedef Tuple3T<T>  vector_t;

    enum Type { NONE, SCALE, ROTATE, TRANSLATE };

    Type     type;
    vector_t vector;

    inline AffineElementT()                             : type(NONE), vector(v) {}
    inline AffineElementT(Type type, const vector_t &v) : type(type), vector(v) {}

    inline vector_t inverse() const { return type == SCALE ? (T(1) / vector) : (-vector); }

    void applyto(matrix_t &matrix, bool inverted) const
    {
        matrix_t m;
        vector_t v(inverted ? inverse() : vector);
        switch (type)
        {
        default:
        case NONE:                           return;
        case SCALE:     m.scale(v);          break;
        case ROTATE:    m.eulerrotation(v);  break;
        case TRANSLATE: m.translate(v);      break;
        }
        matrix *= m;
    }
    void applyto(TransformT<float> &transform) const
    {
        applyto(transform.matrix,  false);
        applyto(transform.inverse, true);
    }
};

#endif  /** _AFFINEELEMENTT_H */
