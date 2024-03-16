#ifndef _MATRIX4T_H
#define _MATRIX4T_H

#include "math.h"
#include "vectors3.h"
#include "tuple3t.h"
#include "tuple4t.h"
#include "planet.h"

#include <iostream>
#include <vector>

#define ROW_COL_ORDER 1 // 1 for <row, col?>-major, 0 for <col, row?>-major

template <typename T>
class Matrix4T
{
public:

    typedef Matrix4T<T> self;
    typedef Tuple3T<T>  vec3_t;
    typedef Tuple4T<T>  vec4_t;

    T data[16];

    inline Matrix4T()                                      { identity(); }
    inline Matrix4T(const self &m)                         { memcpy((T *)data, (const T *)m.data, sizeof(data)); }

	inline self     operator*(const T &p)            const { self r(*this); r *= p; return r; }
	inline self     operator/(const T &p)            const { self r(*this); r /= p; return r; }

	inline self     operator*(const self &p)         const { self r(*this); r *= p; return r; }
	inline vec3_t   operator*(const vec3_t &p)       const { return point(p); }
	inline vec3_t   operator*(const vec4_t &p)       const { return transform(p); }

	inline self    &operator*=(T s)                        { scale(s);     return *this; }
	inline self    &operator/=(T s)                        { scale(1 / s); return *this; }
	inline self    &operator*=(const self &m)              { self r(*this); m_mmatrix4((T *)data, (const T *)r.data, (const T *)m.data); return *this; }

    inline bool     operator==(const self &m)        const { bool f = false; for (int i = 0; i < 16 && !f; i++) f = math::eq(m.data[i], data[i]); return !f; }
    inline bool     operator!=(const self &m)        const { return !operator==(m); }

#if (ROW_COL_ORDER == 1)
    inline T       &operator()(size_t i, size_t j)         { return data[(j << 2) + i]; }
    inline const T &operator()(size_t i, size_t j)   const { return data[(j << 2) + i]; }    
#else       
    inline T       &operator()(size_t i, size_t j)         { return data[(i << 2) + j]; }
    inline const T &operator()(size_t i, size_t j)   const { return data[(i << 2) + j]; }
#endif

    inline vec4_t               transform(const vec4_t &v)          const { vec4_t r; m_mvector4x4((T *)&r, (const T *)data, (const T *)&v); return r; }
    inline vec4_t               transformtranspose(const vec4_t &v) const { vec4_t r; m_mvector4x4transpose((T *)&r, (const T *)data, (const T *)&v); return r; }
	inline vec3_t               point (const vec3_t &p)             const { vec3_t r; m_mvector4x3w1((T *)&r, (const T *)data, (const T *)&p); return r; }
	inline vec3_t               vector(const vec3_t &p)             const { vec3_t r; m_mvector4x3w0((T *)&r, (const T *)data, (const T *)&p); return r; }
	inline vec3_t               pointtranspose(const vec3_t &p)     const { vec3_t r; m_mvector4x3w1transpose((T *)&r, (const T *)data, (const T *)&p); return r; }
	inline vec3_t               vectortranspose(const vec3_t &p)    const { vec3_t r; m_mvector4x3w0transpose((T *)&r, (const T *)data, (const T *)&p); return r; }
	inline PlaneT<T>            planetranspose(const PlaneT<T> &p)  const { PlaneT<T> r; m_mvector4x4transpose((T *)(void *)&r, (const T *)data, (const T *)(void *)&p); return r; }

    inline std::vector<vec3_t> &points (std::vector<vec3_t> &v) const;
    inline std::vector<vec3_t> &vectors(std::vector<vec3_t> &v) const;
    inline std::vector<vec3_t> &pointstranspose (std::vector<vec3_t> &v) const;
    inline std::vector<vec3_t> &vectorstranspose(std::vector<vec3_t> &v) const;

    inline void    identity(void)
    {
        static const T idn[16] = {  1, 0, 0, 0,
	                                0, 1, 0, 0,
			                        0, 0, 1, 0,
					                0, 0, 0, 1  };
        memcpy(data, idn, sizeof(idn));
    }

    inline void    set(T a, T b, T c, T d,
                       T e, T f, T g, T h,
                       T i, T j, T k, T l,
                       T m, T n, T o, T p);

    inline void    scale(T s);
    inline void    scale(const vec3_t &s);
    inline void    scale(const vec3_t &s, const vec3_t &origin);
	inline void    translate(const vec3_t &p);
    inline void    rotation(const vec3_t &s, const vec3_t &t);
    inline void    rotationx(T theta, bool rh = false);
    inline void    rotationy(T theta, bool rh = false);
    inline void    rotationz(T theta, bool rh = false);
	inline void    eulerrotation(const vec3_t &p, bool rh = false) { eulerrotation(p.y, p.x, p.z, rh); }
    inline void    eulerrotation(T yaw, T pitch, T roll, bool rh = false);
    inline void    rotationaxis(const vec3_t &p, T theta, bool rh = false);
    inline void    reoriginate(const vec3_t &origin);
	inline void    transpose();

    inline void    diagonal(vec3_t &v) const;
    inline void    diagonal(vec4_t &v) const;

    inline vec3_t &getTranslation(vec3_t &p);
    inline self   &setTranslation(const vec3_t &p);

    inline void    lookat(const vec3_t &eye, const vec3_t &up, const vec3_t &at);
    inline void    fov(T fov, T aspectratio, T znear, T zfar, bool rh = false);
    inline void    perspective(T viewvolwidth, T viewvolheight, T znear, T zfar, bool rh = false);
    inline void    orthogonal(T viewvolwidth, T viewvolheight, T znear, T zfar, bool rh = false);
    inline void    shadow(const vec3_t &light, const PlaneT<T> &plane, T vanish);

    inline void    fliprows();
    inline void    flipcols();

    inline void    clear();

	std::ostream  &print(std::ostream &s) const;

	inline void    transform(PlaneT<T> &plane) const
	{
	    Tuple3T<T> p(plane.member());
	    plane.normal = vector(plane.normal);
		p = point(p); plane.dist = -plane.normal.dot(p);
	}

	inline void toxy(const PlaneT<T> &p)    { rotation(plane.normal, Tuple3T<T>(0, 0, 1)); }
	inline void fromxy(const PlaneT<T> &p)  { rotation(Tuple3T<T>(0, 0, 1), plane.normal); }
    inline void reflect(const PlaneT<T> &p);

private:

    inline static void m_rotx(T *m, T cos_t, T sin_t);
    inline static void m_roty(T *m, T cos_t, T sin_t);
    inline static void m_rotz(T *m, T cos_t, T sin_t);
    inline static void m_rotaxis(T *m, T x, T y, T z, T cos_t, T sin_t);
    inline static void m_scale(T *m, T a, T b, T c, T d);
    inline static void m_translate(T *m, T a, T b, T c);
    inline static void m_identity(T *m);
    inline static void m_mvector4x4(T *v, const T *m, const T *u);
    inline static void m_mvector4x4transpose(T *v, const T *m, const T *u);
    inline static void m_mvector4x3wn(T *v, const T *m, const T *u, float w);
    inline static void m_mvector4x3w1(T *v, const T *m, const T *u);
    inline static void m_mvector4x3w0(T *v, const T *m, const T *u);
    inline static void m_mvector4x3wntranspose(T *v, const T *m, const T *u, float w);
    inline static void m_mvector4x3w1transpose(T *v, const T *m, const T *u);
    inline static void m_mvector4x3w0transpose(T *v, const T *m, const T *u);
    inline static void m_mmatrix3transpose(T *m, const T *a, const T *b);
    inline static void m_mmatrix4(T *m, const T *a, const T *b);
	inline static void m_mmatrix4transpose(T *m, const T *a, const T *b);
};

typedef Matrix4T<float> Matrix4;

// template implementation

template <typename T>
inline std::ostream &operator<<(std::ostream &s, const Matrix4T<T> &m) { return m.print(s); }

template <typename T>
inline Tuple3T<T> &operator*=(Tuple3T<T> &p, const Matrix4T<T> &m)     { return p = m.point(p); }

template <typename T>
inline std::vector< Tuple3T<T> > &Matrix4T<T>::points(std::vector< Tuple3T<T> > &v) const
{
    std::vector< Tuple3T<T> >::iterator i = v.begin(), e = v.end();
    for (; i != e; ++i) *i = point(*i);
    return v;
}

template <typename T>
inline std::vector< Tuple3T<T> > &Matrix4T<T>::vectors(std::vector< Tuple3T<T> > &v) const
{
    std::vector< Tuple3T<T> >::iterator i = v.begin(), e = v.end();
	for (; i != e; ++i) *i = vector(*i);
	return v;
}

template <typename T>
inline std::vector< Tuple3T<T> > &Matrix4T<T>::pointstranspose(std::vector< Tuple3T<T> > &v) const
{
    std::vector< Tuple3T<T> >::iterator i = v.begin(), e = v.end();
    for (; i != e; ++i) *i = pointtranspose(*i);
    return v;
}

template <typename T>
inline std::vector< Tuple3T<T> > &Matrix4T<T>::vectorstranspose(std::vector< Tuple3T<T> > &v) const
{
    std::vector< Tuple3T<T> >::iterator i = v.begin(), e = v.end();
	for (; i != e; ++i) *i = vectortranspose(*i);
	return v;
}

template <typename T>
inline void Matrix4T<T>::lookat(const Tuple3T<T> &eye, const Tuple3T<T> &up, const Tuple3T<T> &at)
{
    Tuple3T<T> zaxis = (at - eye).normalize(),
	           xaxis = up.cross(zaxis).normalize(),
			   yaxis = zaxis.cross(xaxis);

    set(         xaxis.x,          yaxis.x,          zaxis.x, 0,
                 xaxis.y,          yaxis.y,          zaxis.y, 0,
                 xaxis.z,          yaxis.z,          zaxis.z, 0,
         -xaxis.dot(eye),  -yaxis.dot(eye),  -zaxis.dot(eye), 1);
}

template <typename T>
inline void Matrix4T<T>::scale(const vec3_t &s, const vec3_t &origin)
{
    scale(s); reoriginate(origin);
}

template <typename T>
inline void Matrix4T<T>::reoriginate(const vec3_t &origin)
{
    Matrix4T<T> n, m;
	m.translate(-origin); m *= *this;
	n.translate(origin);  m *= n;
	*this = m;
}

template <typename T>
inline void Matrix4T<T>::translate(const vec3_t &p)
{
    m_translate((T *)data, p.x, p.y, p.z);
}

template <typename T>
inline void Matrix4T<T>::scale(const vec3_t &p)
{
    m_scale((T *)data, p.x, p.y, p.z, 1);
}

template <typename T>
inline void Matrix4T<T>::scale(T s)
{
    m_scale((T *)data, s, s, s, 1);
}

template <typename T>
inline void Matrix4T<T>::rotation(const vec3_t &s, const vec3_t &t)
{
    if (s == t)
	{
	    identity();
		return;
	}

    vec3_t cr(s.cross(t));
    T e = s.dot(t), f = T(1) - e, g = cr.length();
    vec3_t u(cr / g);
    /*

    rotate vec s to vec t

    where e == cos(2 * theta) == s.dot(t)            &&
          f == 1 - e                                 &&
          g == sin(2 * theta) == s.cross(t).length() &&
          u == s.cross(t).normal();

    e + f * u.x * u.x         f * u.x * u.y - g * u.z    f * u.x * u.z + g * u.y    0
    f * u.x * u.y + g * u.z   e + f * u.y * u.y          f * u.y * u.z - g * u.x    0
    f * u.x * u.z - g * u.y   f * u.y * u.z + g * u.x    e + f * u.z * u.z          0
    0                         0                          0                          1

    transposed:

    */
    set(e + f * u.x * u.x,        f * u.x * u.y + g * u.z,    f * u.x * u.z - g * u.y,    0,
        f * u.x * u.y - g * u.z,  e + f * u.y * u.y,          f * u.y * u.z + g * u.x,    0,
        f * u.x * u.z + g * u.y,  f * u.y * u.z - g * u.x,    e + f * u.z * u.z,          0,
        0,                        0,                          0,                          1);
}

#define _M(a, b) (*(((T *)data) + ((b) * 4 + (a))))

template <typename T>
inline Matrix4T<T> &Matrix4T<T>::setTranslation(const Tuple3T<T> &p)
{
    /*
    translate --

    | r11 r12 r13 r11*x + r12*y + r13*z |
    | r21 r22 r23 r21*x + r22*y + r23*z |
    | r31 r32 r33 r31*x + r32*y + r33*z |
    |  0   0   0            1           |
    */
	_M(3, 0) = _M(0, 0) * p.x + _M(1, 0) * p.y + _M(2, 0) * p.z;
	_M(3, 1) = _M(0, 1) * p.x + _M(1, 1) * p.y + _M(2, 1) * p.z;
	_M(3, 2) = _M(0, 2) * p.x + _M(1, 2) * p.y + _M(2, 2) * p.z;
	return *this;
}

template <typename T>
inline Tuple3T<T> &Matrix4T<T>::getTranslation(Tuple3T<T> &p)
{
    /*
    translate --

    | r11 r12 r13 r11*x + r12*y + r13*z |
    | r21 r22 r23 r21*x + r22*y + r23*z |
    | r31 r32 r33 r31*x + r32*y + r33*z |
    |  0   0   0            1           |
    */
	p.x = _M(0, 0) * _M(3, 0) + _M(1, 0) * _M(3, 1) + _M(2, 0) * _M(3, 2);
	p.y = _M(0, 1) * _M(3, 0) + _M(1, 1) * _M(3, 1) + _M(2, 1) * _M(3, 2);
	p.z = _M(0, 2) * _M(3, 0) + _M(1, 2) * _M(3, 1) + _M(2, 2) * _M(3, 2);
	return p;
}

template <typename T>
inline void Matrix4T<T>::set(T a, T b, T c, T d,
                             T e, T f, T g, T h,
                             T i, T j, T k, T l,
                             T m, T n, T o, T p)
{
    T *x = (T *)data;

    *(x + 0)  = a; *(x + 1)  = b; *(x + 2)  = c;  *(x + 3)  = d;
    *(x + 4)  = e; *(x + 5)  = f; *(x + 6)  = g;  *(x + 7)  = h;
    *(x + 8)  = i; *(x + 9)  = j; *(x + 10) = k;  *(x + 11) = l;
    *(x + 12) = m; *(x + 13) = n; *(x + 14) = o;  *(x + 15) = p;
}

template <typename T>
inline void Matrix4T<T>::rotationx(T theta, bool rh)
{
	m_rotx((T *)data, (T)cos(theta), (T)(rh ? -sin(theta) : sin(theta)));
}

template <typename T>
inline void Matrix4T<T>::rotationy(T theta, bool rh)
{
	m_roty((T *)data, (T)cos(theta), (T)(rh ? -sin(theta) : sin(theta)));
}

template <typename T>
inline void Matrix4T<T>::rotationz(T theta, bool rh)
{
	m_rotz((T *)data, (T)cos(theta), (T)(rh ? -sin(theta) : sin(theta)));
}

template <typename T>
inline void Matrix4T<T>::rotationaxis(const vec3_t &p, T theta, bool rh)
{
    m_rotaxis((T *)data, p.x, p.y, p.z, (T)cos(theta), (T)(rh ? -sin(theta) : sin(theta)));
}

template <typename T>
inline void Matrix4T<T>::eulerrotation(T yaw, T pitch, T roll, bool rh)
{
    Matrix4T<T> t;
	  rotationz(roll,  rh);
	t.rotationx(pitch, rh); *this *= t;
	t.rotationy(yaw,   rh); *this *= t;
}

// private implementation..

template <typename T>
inline void Matrix4T<T>::m_rotx(T *m, T cos_t, T sin_t)
{
    *(m + 0)  =   1.0f; *(m + 1)  =  0.0f;  *(m + 2)  =  0.0f;   *(m + 3)  = 0.0f;
    *(m + 4)  =   0.0f; *(m + 5)  =  cos_t; *(m + 6)  = -sin_t;  *(m + 7)  = 0.0f;
    *(m + 8)  =   0.0f; *(m + 9)  =  sin_t; *(m + 10) =  cos_t;  *(m + 11) = 0.0f;
    *(m + 12) =   0.0f; *(m + 13) =  0.0f;  *(m + 14) =  0.0f;   *(m + 15) = 1.0f;
}

template <typename T>
inline void Matrix4T<T>::m_roty(T *m, T cos_t, T sin_t)
{
    *(m + 0)  =  cos_t; *(m + 1)  =  0.0f;  *(m + 2)  =  sin_t;  *(m + 3)  = 0.0f;
    *(m + 4)  =  0.0f;  *(m + 5)  =  1.0f;  *(m + 6)  =  0.0f;   *(m + 7)  = 0.0f;
    *(m + 8)  = -sin_t; *(m + 9)  =  0.0f;  *(m + 10) =  cos_t;  *(m + 11) = 0.0f;
    *(m + 12) =  0.0f;  *(m + 13) =  0.0f;  *(m + 14) =  0.0f;   *(m + 15) = 1.0f;
}

template <typename T>
inline void Matrix4T<T>::m_rotz(T *m, T cos_t, T sin_t)
{
    *(m + 0)  =  cos_t; *(m + 1)  = -sin_t; *(m + 2)  =  0.0f;   *(m + 3)  = 0.0f;
    *(m + 4)  =  sin_t; *(m + 5)  =  cos_t; *(m + 6)  =  0.0f;   *(m + 7)  = 0.0f;
    *(m + 8)  =  0.0f;  *(m + 9)  =  0.0f;  *(m + 10) =  1.0f;   *(m + 11) = 0.0f;
    *(m + 12) =  0.0f;  *(m + 13) =  0.0f;  *(m + 14) =  0.0f;   *(m + 15) = 1.0f;
}

template <typename T>
inline void Matrix4T<T>::m_rotaxis(T *m, T x, T y, T z, T cos_t, T sin_t)
{
    T     t   = 1 - cos_t,
          sx  = sin_t * x, sy  = sin_t * y, sz  = sin_t * z,
          tx  = t     * x, ty  = t     * y, tz  = t     * z,
          txx = tx    * x, txy = tx    * y, tyy = ty    * y,
          tyz = ty    * z, tzz = tz    * z, txz = tx    * z;

    *(m + 0)  = txx + cos_t; *(m + 1)  =  txy - sz;    *(m + 2)  = txz + sy;    *(m + 3)  = 0.0f;
    *(m + 4)  = txy + sz;    *(m + 5)  =  tyy + cos_t; *(m + 6)  = tyz - sx;    *(m + 7)  = 0.0f;
    *(m + 8)  = txz - sy;    *(m + 9)  =  tyz + sx;    *(m + 10) = tzz + cos_t; *(m + 11) = 0.0f;
    *(m + 12) = 0.0f;        *(m + 13) =  0.0f;        *(m + 14) = 0.0f;        *(m + 15) = 1.0f;
}

template <typename T>
inline void Matrix4T<T>::m_scale(T *m, T a, T b, T c, T d)
{
    *(m + 0)  =  a;     *(m + 1)  =  0.0f;  *(m + 2)  =  0.0f;   *(m + 3)  = 0.0f;
    *(m + 4)  =  0.0f;  *(m + 5)  =  b;     *(m + 6)  =  0.0f;   *(m + 7)  = 0.0f;
    *(m + 8)  =  0.0f;  *(m + 9)  =  0.0f;  *(m + 10) =  c;      *(m + 11) = 0.0f;
    *(m + 12) =  0.0f;  *(m + 13) =  0.0f;  *(m + 14) =  0.0f;   *(m + 15) = d;
}

template <typename T>
inline void Matrix4T<T>::m_translate(T *m, T a, T b, T c)
{
    *(m + 0)  =  1.0f;  *(m + 1)  =  0.0f;  *(m + 2)  =  0.0f;   *(m + 3)  = 0;
    *(m + 4)  =  0.0f;  *(m + 5)  =  1.0f;  *(m + 6)  =  0.0f;   *(m + 7)  = 0;
    *(m + 8)  =  0.0f;  *(m + 9)  =  0.0f;  *(m + 10) =  1.0f;   *(m + 11) = 0;
    *(m + 12) =     a;  *(m + 13) =     b;  *(m + 14) =     c;   *(m + 15) = 1.0f;
}

template <typename T>
inline void Matrix4T<T>::m_mvector4x4(T *v, const T *m, const T *u)
{
    *(v + 0) = *(m + 0) * *(u + 0) + *(m + 4) * *(u + 1) + *(m + 8)  * *(u + 2) + *(m + 12) * *(u + 3);
    *(v + 1) = *(m + 1) * *(u + 0) + *(m + 5) * *(u + 1) + *(m + 9)  * *(u + 2) + *(m + 13) * *(u + 3);
    *(v + 2) = *(m + 2) * *(u + 0) + *(m + 6) * *(u + 1) + *(m + 10) * *(u + 2) + *(m + 14) * *(u + 3);
    *(v + 3) = *(m + 3) * *(u + 0) + *(m + 7) * *(u + 1) + *(m + 11) * *(u + 2) + *(m + 15) * *(u + 3);
}

template <typename T>
inline void Matrix4T<T>::m_mvector4x4transpose(T *v, const T *m, const T *u)
{
    *(v + 0) = *(m + 0)  * *(u + 0) + *(m + 1)  * *(u + 1) + *(m + 2)  * *(u + 2) + *(m + 3)  * *(u + 3);
    *(v + 1) = *(m + 4)  * *(u + 0) + *(m + 5)  * *(u + 1) + *(m + 6)  * *(u + 2) + *(m + 7)  * *(u + 3);
    *(v + 2) = *(m + 8)  * *(u + 0) + *(m + 9)  * *(u + 1) + *(m + 10) * *(u + 2) + *(m + 11) * *(u + 3);
    *(v + 3) = *(m + 12) * *(u + 0) + *(m + 13) * *(u + 1) + *(m + 14) * *(u + 2) + *(m + 15) * *(u + 3);
}

template <typename T>
inline void Matrix4T<T>::m_mvector4x3wn(T *v, const T *m, const T *u, float w)
{
    *(v + 0) = *(m + 0)  * *(u + 0) + *(m + 4)  * *(u + 1) + *(m + 8)  * *(u + 2) + *(m + 12) * w;
    *(v + 1) = *(m + 1)  * *(u + 0) + *(m + 5)  * *(u + 1) + *(m + 9)  * *(u + 2) + *(m + 13) * w;
    *(v + 2) = *(m + 2)  * *(u + 0) + *(m + 6)  * *(u + 1) + *(m + 10) * *(u + 2) + *(m + 14) * w;
}

template <typename T>
inline void Matrix4T<T>::m_mvector4x3w1(T *v, const T *m, const T *u)
{
    *(v + 0) = *(m + 0)  * *(u + 0) + *(m + 4)  * *(u + 1) + *(m + 8)  * *(u + 2) + *(m + 12);
    *(v + 1) = *(m + 1)  * *(u + 0) + *(m + 5)  * *(u + 1) + *(m + 9)  * *(u + 2) + *(m + 13);
    *(v + 2) = *(m + 2)  * *(u + 0) + *(m + 6)  * *(u + 1) + *(m + 10) * *(u + 2) + *(m + 14);
}

template <typename T>
inline void Matrix4T<T>::m_mvector4x3w0(T *v, const T *m, const T *u)
{
    *(v + 0) = *(m + 0)  * *(u + 0) + *(m + 4) * *(u + 1) + *(m + 8)  * *(u + 2);
    *(v + 1) = *(m + 1)  * *(u + 0) + *(m + 5) * *(u + 1) + *(m + 9)  * *(u + 2);
    *(v + 2) = *(m + 2)  * *(u + 0) + *(m + 6) * *(u + 1) + *(m + 10) * *(u + 2);
}

template <typename T>
inline void Matrix4T<T>::m_mvector4x3w1transpose(T *v, const T *m, const T *u)
{
    *(v + 0) = *(m + 0)  * *(u + 0) + *(m + 1)  * *(u + 1) + *(m + 2)  * *(u + 2) + *(m + 3);
    *(v + 1) = *(m + 4)  * *(u + 0) + *(m + 5)  * *(u + 1) + *(m + 6)  * *(u + 2) + *(m + 7);
    *(v + 2) = *(m + 8)  * *(u + 0) + *(m + 9)  * *(u + 1) + *(m + 10) * *(u + 2) + *(m + 11);
}

template <typename T>
inline void Matrix4T<T>::m_mvector4x3wntranspose(T *v, const T *m, const T *u, float w)
{
    *(v + 0) = *(m + 0)  * *(u + 0) + *(m + 1)  * *(u + 1) + *(m + 2)  * *(u + 2) + *(m + 3) * w;
    *(v + 1) = *(m + 4)  * *(u + 0) + *(m + 5)  * *(u + 1) + *(m + 6)  * *(u + 2) + *(m + 7) * w;
    *(v + 2) = *(m + 8)  * *(u + 0) + *(m + 9)  * *(u + 1) + *(m + 10) * *(u + 2) + *(m + 11) * w;
}

template <typename T>
inline void Matrix4T<T>::m_mvector4x3w0transpose(T *v, const T *m, const T *u)
{
    *(v + 0) = *(m + 0)  * *(u + 0) + *(m + 1) * *(u + 1) + *(m + 2)  * *(u + 2);
    *(v + 1) = *(m + 4)  * *(u + 0) + *(m + 5) * *(u + 1) + *(m + 6)  * *(u + 2);
    *(v + 2) = *(m + 8)  * *(u + 0) + *(m + 9) * *(u + 1) + *(m + 10) * *(u + 2);
}

template <typename T>
inline void Matrix4T<T>::m_mmatrix4transpose(T *m, const T *b, const T *a)
{
    *(m + 0)  = *(b + 0)  * *(a + 0) + *(b + 1)  * *(a + 4) + *(b + 2)  * *(a + 8)  + *(b + 3)  * *(a + 12);
    *(m + 4)  = *(b + 0)  * *(a + 1) + *(b + 1)  * *(a + 5) + *(b + 2)  * *(a + 9)  + *(b + 3)  * *(a + 13);
    *(m + 8)  = *(b + 0)  * *(a + 2) + *(b + 1)  * *(a + 6) + *(b + 2)  * *(a + 10) + *(b + 3)  * *(a + 14);
    *(m + 12) = *(b + 0)  * *(a + 3) + *(b + 1)  * *(a + 7) + *(b + 2)  * *(a + 11) + *(b + 3)  * *(a + 15);
    *(m + 1)  = *(b + 4)  * *(a + 0) + *(b + 5)  * *(a + 4) + *(b + 6)  * *(a + 8)  + *(b + 7)  * *(a + 12);
    *(m + 5)  = *(b + 4)  * *(a + 1) + *(b + 5)  * *(a + 5) + *(b + 6)  * *(a + 9)  + *(b + 7)  * *(a + 13);
    *(m + 9)  = *(b + 4)  * *(a + 2) + *(b + 5)  * *(a + 6) + *(b + 6)  * *(a + 10) + *(b + 7)  * *(a + 14);
    *(m + 13) = *(b + 4)  * *(a + 3) + *(b + 5)  * *(a + 7) + *(b + 6)  * *(a + 11) + *(b + 7)  * *(a + 15);
    *(m + 2)  = *(b + 8)  * *(a + 0) + *(b + 9)  * *(a + 4) + *(b + 10) * *(a + 8)  + *(b + 11) * *(a + 12);
    *(m + 6)  = *(b + 8)  * *(a + 1) + *(b + 9)  * *(a + 5) + *(b + 10) * *(a + 9)  + *(b + 11) * *(a + 13);
    *(m + 10) = *(b + 8)  * *(a + 2) + *(b + 9)  * *(a + 6) + *(b + 10) * *(a + 10) + *(b + 11) * *(a + 14);
    *(m + 14) = *(b + 8)  * *(a + 3) + *(b + 9)  * *(a + 7) + *(b + 10) * *(a + 11) + *(b + 11) * *(a + 15);
    *(m + 3)  = *(b + 12) * *(a + 0) + *(b + 13) * *(a + 4) + *(b + 14) * *(a + 8)  + *(b + 15) * *(a + 12);
    *(m + 7)  = *(b + 12) * *(a + 1) + *(b + 13) * *(a + 5) + *(b + 14) * *(a + 9)  + *(b + 15) * *(a + 13);
    *(m + 11) = *(b + 12) * *(a + 2) + *(b + 13) * *(a + 6) + *(b + 14) * *(a + 10) + *(b + 15) * *(a + 14);
    *(m + 15) = *(b + 12) * *(a + 3) + *(b + 13) * *(a + 7) + *(b + 14) * *(a + 11) + *(b + 15) * *(a + 15);
}

template <typename T>
inline void Matrix4T<T>::m_mmatrix4(T *m, const T *a, const T *b)
{
    *(m + 0)  = *(a + 0)  * *(b + 0) + *(a + 1)  * *(b + 4) + *(a + 2)  * *(b + 8)  + *(a + 3)  * *(b + 12);
    *(m + 1)  = *(a + 0)  * *(b + 1) + *(a + 1)  * *(b + 5) + *(a + 2)  * *(b + 9)  + *(a + 3)  * *(b + 13);
    *(m + 2)  = *(a + 0)  * *(b + 2) + *(a + 1)  * *(b + 6) + *(a + 2)  * *(b + 10) + *(a + 3)  * *(b + 14);
    *(m + 3)  = *(a + 0)  * *(b + 3) + *(a + 1)  * *(b + 7) + *(a + 2)  * *(b + 11) + *(a + 3)  * *(b + 15);
    *(m + 4)  = *(a + 4)  * *(b + 0) + *(a + 5)  * *(b + 4) + *(a + 6)  * *(b + 8)  + *(a + 7)  * *(b + 12);
    *(m + 5)  = *(a + 4)  * *(b + 1) + *(a + 5)  * *(b + 5) + *(a + 6)  * *(b + 9)  + *(a + 7)  * *(b + 13);
    *(m + 6)  = *(a + 4)  * *(b + 2) + *(a + 5)  * *(b + 6) + *(a + 6)  * *(b + 10) + *(a + 7)  * *(b + 14);
    *(m + 7)  = *(a + 4)  * *(b + 3) + *(a + 5)  * *(b + 7) + *(a + 6)  * *(b + 11) + *(a + 7)  * *(b + 15);
    *(m + 8)  = *(a + 8)  * *(b + 0) + *(a + 9)  * *(b + 4) + *(a + 10) * *(b + 8)  + *(a + 11) * *(b + 12);
    *(m + 9)  = *(a + 8)  * *(b + 1) + *(a + 9)  * *(b + 5) + *(a + 10) * *(b + 9)  + *(a + 11) * *(b + 13);
    *(m + 10) = *(a + 8)  * *(b + 2) + *(a + 9)  * *(b + 6) + *(a + 10) * *(b + 10) + *(a + 11) * *(b + 14);
    *(m + 11) = *(a + 8)  * *(b + 3) + *(a + 9)  * *(b + 7) + *(a + 10) * *(b + 11) + *(a + 11) * *(b + 15);
    *(m + 12) = *(a + 12) * *(b + 0) + *(a + 13) * *(b + 4) + *(a + 14) * *(b + 8)  + *(a + 15) * *(b + 12);
    *(m + 13) = *(a + 12) * *(b + 1) + *(a + 13) * *(b + 5) + *(a + 14) * *(b + 9)  + *(a + 15) * *(b + 13);
    *(m + 14) = *(a + 12) * *(b + 2) + *(a + 13) * *(b + 6) + *(a + 14) * *(b + 10) + *(a + 15) * *(b + 14);
    *(m + 15) = *(a + 12) * *(b + 3) + *(a + 13) * *(b + 7) + *(a + 14) * *(b + 11) + *(a + 15) * *(b + 15);
}

template <typename T>
std::ostream &Matrix4T<T>::print(std::ostream &s) const
{
	s << '[' << std::endl;
    for (int i = 0; i < 4; i++)
	{
	    for (int j = 0; j < 4; j++)
		{
		    s.width(4);
			s << data[i * 4 + j] << ' ';
		}
		s << std::endl;
	}
	s << ']' << std::endl;
	return s;
}

template <typename T>
inline void Matrix4T<T>::transpose()
{
    T d[16];
    d[0] = data[0];  d[4] = data[1];  d[8]  = data[2];  d[12] = data[3];
    d[1] = data[4];  d[5] = data[5];  d[9]  = data[6];  d[13] = data[7];
    d[2] = data[8];  d[6] = data[9];  d[10] = data[10]; d[14] = data[11];
    d[3] = data[12]; d[7] = data[13]; d[11] = data[14]; d[15] = data[15];
	memcpy((T *)data, (const T *)d, sizeof(d));
}

template <typename T>
inline void Matrix4T<T>::diagonal(vec3_t &v) const
{
    v.set(data[0], data[5], data[10]);
}

template <typename T>
inline void Matrix4T<T>::diagonal(vec4_t &v) const
{
    v.set(data[0], data[5], data[10], data[15]);
}

template <typename T>
inline void Matrix4T<T>::flipcols(void)
{
    T d[16];
    d[0] = data[12]; d[4] = data[8];  d[8]  = data[4]; d[12] = data[0];
    d[1] = data[13]; d[5] = data[9];  d[9]  = data[5]; d[13] = data[1];
    d[2] = data[14]; d[6] = data[10]; d[10] = data[6]; d[14] = data[2];
    d[3] = data[15]; d[7] = data[11]; d[11] = data[7]; d[15] = data[3];
	memcpy((T *)data, (const T *)d, sizeof(d));
}

template <typename T>
inline void Matrix4T<T>::fliprows(void)
{
    T d[16];
    d[0] = data[3]; d[4] = data[7]; d[8]  = data[11]; d[12] = data[15];
    d[1] = data[2]; d[5] = data[6]; d[9]  = data[10]; d[13] = data[14];
    d[2] = data[1]; d[6] = data[5]; d[10] = data[9];  d[14] = data[13];
    d[3] = data[0]; d[7] = data[4]; d[11] = data[8];  d[15] = data[12];
	memcpy((T *)data, (const T *)d, sizeof(d));
}

template <typename T>
inline void Matrix4T<T>::clear(void)
{
    memset(data, 0, sizeof(T) * 4 * 4);
}

template <typename T>
inline void Matrix4T<T>::fov(T fov, T aspectratio, T znear, T zfar, bool rh)
{
    T h = (T)(cos(fov / 2) / sin(fov / 2));
    perspective(h / aspectratio, h, znear, zfar, rh);
}

template <typename T>
inline void Matrix4T<T>::orthogonal(T w, T h, T znear, T zfar, bool rh)
{
    T zd = znear - zfar;
    set(2 / w, 0,     0,          0,
	    0,     2 / h, 0,          0,
		0,     0,     1 / zd,     0,
		0,     0,     znear / zd, rh ? -1.0f : 1.0f);
}

template <typename T>
inline void Matrix4T<T>::perspective(T w, T h, T znear, T zfar, bool rh)
{
    T zvol = zfar / (zfar - znear);
    set(w,  0,   0,            0,
        0,  h,   0,            0,
        0,  0,   zvol,         rh ? -1.0f : 1.0f,
        0,  0,  -znear * zvol, 0);
}


template <typename T>
inline void Matrix4T<T>::reflect(const PlaneT<T> &p)
{
    set(-2 * p.a * p.a + 1,  -2 * p.b * p.a,      -2 * p.c * p.a,       0,
        -2 * p.a * p.b,      -2 * p.b * p.b + 1,  -2 * p.c * p.b,       0,
        -2 * p.a * p.c,      -2 * p.b * p.c,      -2 * p.c * p.c + 1,   0,
        -2 * p.a * p.d,      -2 * p.b * p.d,      -2 * p.c * p.d,       1);
}

template <typename T>
inline void Matrix4T<T>::shadow(const vec3_t &light, const PlaneT<T> &plane, T vanish)
{
    vec3_t pn(plane.normal);
    T d = light.dot(pn), dist = plane.dist;
    set(pn.x * light.x + d, pn.x * light.y,     pn.x * light.z,     pn.x * vanish,
        pn.y * light.x,     pn.y * light.y + d, pn.y * light.z,     pn.y * vanish,
        pn.z * light.x,     pn.z * light.y,     pn.z * light.z + d, pn.z * vanish,
        dist * light.x + d, dist * light.y,     dist * light.z,     dist * vanish);
}

#undef _M

#endif  // _MATRIX4T_H