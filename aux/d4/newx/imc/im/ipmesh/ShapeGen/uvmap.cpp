#include "uvmap.h"
#include "types/matrix4t.h"
#include "types/math.h"
#include <math.h>

const double PI         = 3.141592653589793;
const double PI2        = PI * 2.0;
const double HALFPI     = PI * 0.5;
const double PI2_INV    = 1.0 / PI2;
const double PI_INV     = 1.0 / PI;
const double HALFPI_INV = 1.0 / HALFPI;

inline double angle(double x, double y)
{
    return x < 0. ? (acos(-y) + PI) : acos(y);
}

inline float sign(float a) { return a < 0.f ? -1.f : 1.f; }

Tuple2T<float> map_unit_sphere(const Tuple3T<float> &point)   // uv = [angle(x) angle(y)]
{
    Tuple3T<float> p(point.normal());
    return Tuple2T<float>((float)(asin(p.x) / PI + 0.5),
                          (float)(asin(p.y) / PI + 0.5));
    /*
    return Tuple2T<float>((float)(angle(p.x, p.z) / PI2),
                          //(float)(angle(p.y, p.z) / PI2));
                          (float)(asin(p.y)       / PI  + 0.5));
    */
}