#ifndef _TRANSFORM_H
#define _TRANSFORM_H

#include "core/core.h"

class Transform
{
public:
        
    Matrix matrix;

    inline Transform(const Transform &t)    { matrix = t.matrix; }
    inline Transform(const Matrix &m)       { matrix = m; }
    inline Transform()                      { matrix.identity(); }

    static Transform identity;
};

#endif // _TRANSFORM_H