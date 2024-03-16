#ifndef _TRANSFORMT_H
#define _TRANSFORMT_H

#include "math.h"
#include "vectors3.h"
#include "tuple3t.h"
#include "matrix4t.h"
#include "planet.h"

#include <iostream>
#include <vector>

template <class T>
class TransformT
{
public:

    typedef TransformT<T>             self;
    typedef Matrix4T<T>               matrix_t;
    typedef Tuple3T<T>                vec3_t;
	typedef std::vector< Tuple3T<T> > vec3arr_t;

    matrix_t matrix,
	         inverse;

    inline TransformT() {}
    inline TransformT(const self &m) : matrix(m.matrix), inverse(m.inverse) {}
    inline TransformT(const Matrix4T<float> &m, const Matrix4T<float> &inv) : matrix(m), inverse(inv) {}

    inline void        invert()                                                { Matrix4T<T> m = matrix; matrix = inverse; inverse = m; }

	inline self        operator*(const T &p)                             const { self r(*this); r *= p; return r; }
	inline self        operator/(const T &p)                             const { self r(*this); r /= p; return r; }

	inline self        operator*(const self &p)                          const { self r(*this); r *= p; return r; }
	inline self        operator/(const self &p)                          const { self r(*this); r /= p; return r; }

	inline vec3_t      operator*(const vec3_t &p)                        const { return matrix.point(p); }
	inline vec3_t      operator/(const vec3_t &p)                        const { return inverse.point(p); }
    inline PlaneT<T>   operator*(const PlaneT<T> &p)                     const { return inverse.planetranspose(p); }
    inline PlaneT<T>   operator/(const PlaneT<T> &p)                     const { return matrix.planetranspose(p); }

	inline self       &operator*=(T s)                                         { matrix *= s;         inverse *= 1 / s;               return *this; }
	inline self       &operator/=(T s)                                         { matrix *= 1 / s;     inverse *= s;                   return *this; }
	inline self       &operator*=(const self &p)                               { matrix *= p.matrix;  inverse  = p.inverse * inverse; return *this; }
	inline self       &operator/=(const self &p)                               { matrix *= p.inverse; inverse *= p.matrix;            return *this; }

	inline vec3_t      point (const vec3_t &p)                           const { return matrix.point(p); }
	inline vec3_t      vector(const vec3_t &p)                           const { return matrix.vector(p); }
	inline vec3_t      pointtranspose(const vec3_t &p)                   const { return matrix.pointtranspose(p); }
	inline vec3_t      vectortranspose(const vec3_t &p)                  const { return matrix.vectortranspose(p); 	}
	inline vec3_t      invpoint (const vec3_t &p)                        const { return inverse.point(p); }
	inline vec3_t      invvector(const vec3_t &p)                        const { return inverse.vector(p); }
	inline vec3_t      invpointtranspose(const vec3_t &p)                const { return inverse.pointtranspose(p); }
	inline vec3_t      invvectortranspose(const vec3_t &p)               const { return inverse.vectortranspose(p); }

    inline vec3arr_t  &points (vec3arr_t &v)                             const { return matrix.points(v);  }
    inline vec3arr_t  &vectors(vec3arr_t &v)                             const { return matrix.vectors(v); }
    inline vec3arr_t  &pointstranspose(vec3arr_t &v)                     const { return matrix.pointstranspose(v);  }
    inline vec3arr_t  &vectorstranspose(vec3arr_t &v)                    const { return matrix.vectorstranspose(v); }
    inline vec3arr_t  &invpoints (vec3arr_t &v)                          const { return inverse.points(v);  }
    inline vec3arr_t  &invvectors(vec3arr_t &v)                          const { return inverse.vectors(v); }
    inline vec3arr_t  &invpointstranspose(vec3arr_t &v)                  const { return inverse.pointstranspose(v);  }
    inline vec3arr_t  &invvectorstranspose(vec3arr_t &v)                 const { return inverse.vectorstranspose(v); }

    inline self       &identity()                                              { matrix.identity();             inverse.identity();                                        return *this; }
    inline self       &scale(T s)                                              { matrix.scale(s);               inverse.scale(T(1) / s);                                   return *this; }
    inline self       &scale(const vec3_t &s)                                  { matrix.scale(s);               inverse.scale(vec3_t(T(1) / s.x, T(1) / s.y, T(1) / s.z)); return *this; }
    inline self       &scale(const vec3_t &s, const vec3_t &origin)            { scale(s); return reoriginate(origin); }
	inline self       &translate(const vec3_t &p)                              { matrix.translate(p);           inverse.translate(-p);                            return *this; }
    inline self       &rotation(const vec3_t &s, const vec3_t &t)              { matrix.rotation(s, t);         (inverse = matrix).transpose();                   return *this; }
    inline self       &rotationx(T theta, bool rh = false)                     { matrix.rotationx(theta, rh);   (inverse = matrix).transpose();                   return *this; }
    inline self       &rotationy(T theta, bool rh = false)                     { matrix.rotationy(theta, rh);   (inverse = matrix).transpose();                   return *this; }
    inline self       &rotationz(T theta, bool rh = false)                     { matrix.rotationz(theta, rh);   (inverse = matrix).transpose();                   return *this; }
    inline self       &rotationaxis(const vec3_t &p, T theta, bool rh = false) { matrix.rotationaxis(p, t, rh); (inverse = matrix).transpose();                   return *this; }
    inline self       &eulerrotation(const vec3_t &p, bool rh = false)         { matrix.eulerrotation(p, rh);   (inverse = matrix).transpose();                   return *this; }
	inline self       &transpose()                                             { matrix.transpose();             inverse.transpose();                             return *this; }
	inline self       &reoriginate(const vec3_t &origin)                       { self tr; tr.translate(-origin); tr *= *this; translate(origin); tr *= *this;     return *this = tr; }

	static inline self getScale(T s)                                           { self tr; tr.scale(s);             return tr; }
	static inline self getScale(const vec3_t &s)                               { self tr; tr.scale(s);             return tr; }
	static inline self getTranslate(const vec3_t &s)                           { self tr; tr.translate(s);         return tr; }
    static inline self getRotation(const vec3_t &s, const vec3_t &t)           { self tr; tr.rotation(s, t);       return tr; }
    static inline self getEulerRotation(const vec3_t &s, bool rh = false)      { self tr; tr.eulerrotation(s, rh); return tr; }

	inline void transform(PlaneT<T> &plane) const
	{
	    Tuple3T<T> p(plane.member());
	    plane.normal = invvectortranspose(plane.normal).normal();
		p = point(p); plane.dist = -plane.normal.dot(p);
	}
};

#endif  // _TRANSFORMT_H
