#ifndef _XNA_H
#define _XNA_H

// types named like XNA types

#include "math.h"
#include "tuple2f.h"
#include "tuple3f.h"
#include "tuple4f.h"
#include "matrix4f.h"
#include "point.h"
#include "size.h"
#include "rectangle.h"
#include "gametime.h"

typedef Tuple2f  Vector2;
typedef Tuple3f  Vector3;
typedef Tuple4f  Vector4;

typedef Matrix4f Matrix;

inline Vector2 v2(const Vector3 &v)                 { return Vector2(v.x, v.y);    }
inline Vector3 v3(const Vector2 &v, float z = 0.0f) { return Vector3(v.x, v.y, z); }
inline Vector4 v4(const Vector3 &v, float w = 0.0f) { return Vector4(v.x, v.y, v.z, w); }

#endif  // _XNA_H 