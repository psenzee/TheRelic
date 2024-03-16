#ifndef _PLANET_H
#define _PLANET_H

#include "tuple3t.h"
#include "pointlinet.h"
#include "math.h"

class PlaneException : public std::exception
{
public:
    PlaneException(const char *message) : std::exception(message) {}
};

class PlaneNormalizeException : public PlaneException
{
public:
    PlaneNormalizeException(const char *message) : PlaneException(message) {}
};

class PlaneCreateException : public PlaneException
{
public:
    PlaneCreateException(const char *message) : PlaneException(message) {}
};

template <class T>
class PlaneT
{
public:

    typedef Tuple3T<T>    point_t;
    typedef PointLineT<T> line_t;
    typedef PlaneT<T>     self;

    union
    {
        struct
        {
            point_t normal;
            T       dist;
        };
		struct
		{
            T a, b, c, d;
		};
    };

    inline  PlaneT() : a(0), b(0), c(0), d(0)                            {} // force invalid..
	inline  PlaneT(T a, T b, T c, T d)                                   { set(a, b, c, d); }
    inline  PlaneT(const point_t &a, const point_t &b, const point_t &c) { set(a, b, c);    }

    // operators
	inline bool operator<(const self &p) const
	{
        /*
	    if (normal < p.normal) return true;
		if (p.normal < normal) return false;
	    return math::lt(dist, p.dist);
        */
        if (normal.exact_lt(p.normal)) return true;
        if (p.normal.exact_lt(normal)) return false;
        return math::lt(dist, p.dist);
	}
	inline bool operator>(const self &p) const
	{
        /*
	    if (normal > p.normal) return true;
		if (p.normal > normal) return false;
	    return math::gt(dist, p.dist);
        */
        if (normal.exact_gt(p.normal)) return true;
        if (p.normal.exact_gt(normal)) return false;
        return math::gt(dist, p.dist);
	}
	inline bool operator==(const self &p) const
	{
//	    return (normal == p.normal && math::eq(dist, p.dist));
	    return (normal.exact_eq(p.normal) && math::eq(dist, p.dist));
	}
	inline bool operator!=(const self &p) const
	{
//	    return (normal != p.normal || !math::eq(dist, p.dist));
	    return (normal.exact_neq(p.normal) || !math::eq(dist, p.dist));
	}
	inline PlaneT<T> operator-(void) const
	{
	    return PlaneT<T>(-a, -b, -c, -d);
	}
	/*
	inline double dot(const Tuple3T<T> &p) const
	{
	    return (double)normal.x * p.x + (double)normal.y * p.y + (double)normal.z * p.z;
	}
	*/
	inline T dot(const Tuple3T<T> &p) const
	{
	    return normal.x * p.x + normal.y * p.y + normal.z * p.z;
	}
    bool intersect(const line_t &line, point_t &at)
	{
	/*
	    Tuple3T<double> dat;
	    bool b = intersectdouble(PointLineT<double>(todouble(line.a), todouble(line.b)), dat);
		at = fromdouble(dat);
		return b;
	*/
	    point_t nl    = line.normal();
        T       denom = dot(nl),
	            num   = dot(line.a) + d;
        if (math::eq(denom, 0.0)) return false;
        else at = line.a - nl * (num / denom);
	    return true;
	}
    bool intersect(const line_t &line, T &t)
	{
	/*
	    double dt;
	    bool b = intersectdouble(PointLineT<double>(todouble(line.a), todouble(line.b)), dt);
		t = (T)dt;
		return b;
	*/
	    point_t nl    = line.normal();
        T       denom = dot(nl),
	            num   = dot(line.a) + d;
        if (math::eq(denom, 0.0)) return false;
        else t = num / denom;
	    return true;
	}
	/*
    inline double distance(const point_t &p) const
	{
		return dot(p) + d;
	}
	*/
    inline T distance(const point_t &p) const
	{
		return dot(p) + d;
	}
	inline bool equalOrOpposite(const self &p) const
	{
		return p == *this || p == -*this;
	}
	inline bool inHalfSpace(const point_t &p) const
	{
        return distance(p) < 0;// but bsp uses neg as in hs
	}
	inline bool straddle(const point_t &a, const point_t &b) const
	{
        return math::sign(distance(a)) != math::sign(distance(b));
	}
	inline bool straddle(const line_t &line) const
	{
        return straddle(line.a, line.b);
	}
	inline void set(const point_t &a, const point_t &b, const point_t &c)
	{
	    Tuple3T<double> pa(todouble(a)), pb(todouble(b)), pc(todouble(c)),
		                n((pb - pa).cross(pc - pa));        
	    double          distance = -n.dot(pa),
                        m        =  n.length();
        if (m == 0)
		{
			__asm int 3;
		    throw PlaneCreateException("PlaneT::set() : vertices are invalid");
		}
		m = 1 / m; n *= m; distance *= m;
		this->a = (T)n.x; this->b = (T)n.y; this->c = (T)n.z; this->d = (T)distance;
	}
	inline void set(T a, T b, T c, T d)
	{
	    this->a = a; this->b = b; this->c = c; this->d = d;
        normalize();
	}
	inline point_t member() const
	{
		return normal * -dist; // $TODO i don't think dist should be neg here..
	}
	inline std::ostream &print(std::ostream &s) const
	{
	    return s << "[Plane a " << a << " b " << b << " c " << c << " d " << d << "]";
	}
    inline void flip(void)
	{
	    a = -a; b = -b; c = -c; d = -d;
	}
	bool intersect(const self &other, line_t &at) const
	{
        if (this == &other || math::eq(dot(other.normal), T(1)))
		    return false;

		T fn00 = normal.length(), // should be 1!
		  fn01 = dot(other.normal),
		  fn11 = other.normal.length(), // should be 1!
		  det  = fn00 * fn11 - fn01 * fn01;

		if (math::zero(det))
			return false;

        det = T(1) / det;
		T fc0 = (fn11 * dist        + fn01 * -other.dist) * det,
		  fc1 = (fn00 * other.dist  + fn01 * -dist)       * det;
		at.a = normal * fc0 + other.normal * fc1;
		at.b = at.a + normal.cross(other.normal);
		return true;
	}
    /* Determines if an edge bounded by (x1,y1,z1)->(x2,y2,z2) intersects
     * the plane.
     *
     * If there's an intersection,
     *    the sign of (x1,y1,z1), NEGATIVE or POSITIVE, w.r.t. the plane is
     *    returned with the intersection (ixx,iyy,izz) updated.
     * Otherwise ZERO is returned.
     */
    math::Sign intersect(const point_t &p1, const point_t &p2, point_t &at) const
    {
	/*
	    Tuple3T<double> dat;
		math::Sign sign = intersectdouble(todouble(p1), todouble(p2), dat);
		at = fromdouble(dat);
		return sign;
	*/
        T          t1, t2;
        math::Sign s1, s2, zero = math::ZERO;

        if      ((s1 = math::sign(t1 = distance(p1))) == zero) return zero;
        else if ((s2 = math::sign(t2 = distance(p2))) == zero) { at = p2; return s1; }
        else if (s1 == s2)                                     return zero;

        return intersectat(p1, p2, at) ? s1 : zero;
    }
	bool intersectat(const point_t &p1, const point_t &p2, point_t &at) const // infinite line..
	{
/*
	    Tuple3T<double> dat;
		bool b = intersectatdouble(todouble(p1), todouble(p2), dat);
		at = fromdouble(dat);
		return b;
*/
	    T denom = 0;
        // intersection point
        point_t dp(p2 - p1);
        if (math::eq(denom = dot(dp), (T)0))
            return false;
        at = p1 + (dp * (-distance(p1) / denom));
		return true;
	}
	bool intersect(const self &b, const self &c, point_t &at) const
	{
	    line_t ln;
	    if (!intersect(b, ln)) return false;
	    return c.intersectat(ln.a, ln.b, at);
	}
	inline T angle(const self &other)
	{
	    return (T)acos(dot(other.normal));
	}
	inline void normalize()
	{
	    double da   = a, db = b, dc = c, dd = d,
		       lsq  = da * da + db * db + dc * dc,
			   m;
		if (!math::eq(lsq, 1.0))
		{
            if (lsq == 0)
		        throw PlaneNormalizeException("PlaneT:: normalize() : plane equation is invalid");
		    m = 1.0 / sqrt(lsq);
		    da *= m; db *= m; dc *= m; dd *= m;
		    a = (T)da; b = (T)db; c = (T)dc; d = (T)dd;
		}
	}

    // forced double precision:

	inline Tuple3T<double> todouble(const Tuple3T<T> &p)        const { return Tuple3T<double>((double)p.x, (double)p.y, (double)p.z); }
	inline Tuple3T<T>      fromdouble(const Tuple3T<double> &p) const { return Tuple3T<T>((T)p.x, (T)p.y, (T)p.z); }

	inline double dotdouble(const Tuple3T<double> &p) const
	{
	    return (double)normal.x * p.x + (double)normal.y * p.y + (double)normal.z * p.z;
	}
    inline double distancedouble(const Tuple3T<double> &p) const
	{
		return dotdouble(p) + (double)d;
	}
    // intersect infinite line
    bool intersectdouble(const PointLineT<double> &line, point_t &at)
	{
		Tuple3T<double> nl = (line.b - line.a).normal();
        double  denom = dotdouble(nl),
	            num   = distancedouble(line.a);
        if (math::eq(denom, 0.0)) return false;
        at = line.a - nl * (num / denom);
	    return true;
	}
    math::Sign intersectdouble(const Tuple3T<double> &p1, const Tuple3T<double> &p2, Tuple3T<double> &at) const
    {
        double     t1, t2;
        math::Sign s1, s2, zero = math::ZERO;

        if      ((s1 = math::sign(t1 = distancedouble(p1))) == zero) return zero;
        else if ((s2 = math::sign(t2 = distancedouble(p2))) == zero) { at = p2; return s1; }
        else if  (s1 == s2)                                          return zero;

        return intersectatdouble(p1, p2, at) ? s1 : zero;
    }
	bool intersectatdouble(const Tuple3T<double> &p1, const Tuple3T<double> &p2, Tuple3T<double> &at) const // infinite line..
	{
	    double denom = 0;
        // intersection point
        Tuple3T<double> dp(p2 - p1);
        if (math::eq(denom = dotdouble(dp), 0.0))
            return false;
        at = p1 + (dp * (-distancedouble(p1) / denom));
		return true;
	}
    bool intersectdouble(const PointLineT<double> &line, double &t)
	{
	    Tuple3T<double> nl    = line.normal();
        double          denom = dotdouble(nl),
	                    num   = dotdouble(line.a) + d;
        if (math::eq(denom, 0.0)) return false;
        else t = num / denom;
	    return true;
	}
};

//typedef PlaneT<float> Plane;
template <class T> std::ostream &operator<<(std::ostream &s, const PlaneT<T> &plane) { return plane.print(s); }

#endif  /** _PLANET_H */
