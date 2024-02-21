#ifndef _MATHUTIL_H
#define _MATHUTIL_H

#include <math.h>
#include "core/core.h"

const double PI         = 3.141592653589793;
const double DEG_TO_RAD = PI / 180.0;
const double RAD_TO_DEG = 180.0 / PI;

class Ray
{
public:
    Vector3 origin,    direction;
        
    inline Ray(const Vector3 &origin, const Vector3 &direction) : origin(origin), direction(direction) {}
    
    inline Vector3 at(float t) const { return origin + direction * t; }
};

class Sphere
{
public:
    Vector3 center;
    float   radius;
    
    inline Sphere(const Vector3 &center, float radius) : center(center), radius(radius) {}
};

class MathUtil
{
public:

    static inline bool CloseTo(float a, float b, float epsilon)
    {
        return a == b || fabsf(a - b) < epsilon;
    }

    static float   GetAngle(const Vector3 &a, const Vector3 &b, float defaultAngle);
    
    static float   NormalizeAngle2Pi(float x);
    static float   NormalizeAnglePiNegPi(float x);
    static bool    AreAnglesClose(float a, float b, float tolerance);
    static float   GetAngleDifference(float a, float b);
    
    static bool    CloseTo(const Vector3 &a, const Vector3 &b, float epsilon);
    static bool    IsEqualAngle(const Vector3 &a, const Vector3 &b, float epsilon);
    
    static Vector4 Unproject(const Vector4 &p, const Matrix &inverseCamera);
    static Vector4 Unproject(const Vector3 &p, const Matrix &inverseCamera);
    static Ray     CreateRayFromNormalizedScreen(const Vector2 &from, const Matrix &inverseCamera);
    static Vector4 CreatePlaneFromVertices(const Vector3 &a, const Vector3 &b, const Vector3 &c);
    static float   PlaneDot(const Vector4 &plane, const Vector3 &v);
    
    // assumes normalized ray and plane
    static bool    Intersection(const Ray &ray, const Vector4 &plane, Vector3 &at);
    static bool    Intersection(const Ray &ray, const Sphere &sphere, float *at);
};
    
#endif // _MATHUTIL_H
