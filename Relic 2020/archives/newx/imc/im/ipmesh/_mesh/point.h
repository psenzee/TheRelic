#ifndef _POINT_H
#define _POINT_H

#include <math.h>

#include "constants.h"

#include <iostream>

class Point
{
public:
    double x, y, z;
    inline Point(double x = 0, double y = 0, double z = 0) : x(x), y(y), z(z) {}

	inline Point  cross(const Point &p) const { return Point(y * p.z - z * p.y, z * p.x - x * p.z,  x * p.y - y * p.x); }
	inline double dot(const Point &p)   const { return x * p.x + y * p.y + z * p.z; }

	inline bool operator<(const Point &p) const
	{
	    Point d = *this - p;

	    if      (d.x < -EPSILON) return true;
		else if (d.x >  EPSILON) return false;

	    if      (d.y < -EPSILON) return true;
		else if (d.y >  EPSILON) return false;

	    if      (d.z < -EPSILON) return true;
		else if (d.z >  EPSILON) return false;

        return false; // equal
	}
	inline bool operator!=(const Point &p) const
	{
	    Point d = *this - p;
        return d.x < -EPSILON || d.x > EPSILON ||
               d.y < -EPSILON || d.y > EPSILON || 
			   d.z < -EPSILON || d.z > EPSILON;
	}
	inline bool operator==(const Point &p) const	{ return !operator!=(p); }

	inline Point operator-(const Point &p) const	{ return Point(x - p.x, y - p.y, z - p.z); }
	inline Point operator*(const Point &p) const    { return Point(x * p.x, y * p.y, z * p.z); }
	inline Point operator+(const Point &p) const    { return Point(x + p.x, y + p.y, z + p.z); }
	inline Point operator/(const Point &p) const    { return Point(x / p.x, y / p.y, z / p.z); }

	inline Point &operator-=(const Point &p)        { x -= p.x; y -= p.y; z -= p.z; return *this; }
	inline Point &operator*=(const Point &p)        { x *= p.x; y *= p.y; z *= p.z; return *this; }
	inline Point &operator+=(const Point &p)        { x += p.x; y += p.y; z += p.z; return *this; }
	inline Point &operator/=(const Point &p)        { x /= p.x; y /= p.y; z /= p.z; return *this; }

	inline double lengthsq(void)  const
	{
        return x * x + y * y + z * z;
	}
	inline double length(void) const
	{
        return sqrt(lengthsq());
	}
	inline Point &normalize(void)
	{
        double d = length();
		if (d != 0) { x /= d; y /= d; z /= d; }
		return *this;
	}
	inline std::ostream &print(std::ostream &s) const
	{
	    return s << "{" << x << ", " << y << ", " << z << "}";
	}
	inline void set(double x, double y, double z)
	{
        this->x = x; this->y = y; this->z = z;
	}
};

inline std::ostream &operator<<(std::ostream &s, const Point &p)
{
    return p.print(s);
}

#endif  /** _POINT_H */
