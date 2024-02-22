#ifndef _UVMAP_H
#define _UVMAP_H

#include "types/tuple3t.h"
#include "types/tuple2t.h"

Tuple2T<float> map_unit_sphere(const Tuple3T<float> &p);   // uv = [angle(x) angle(y)]

#endif // _UVMAP_H