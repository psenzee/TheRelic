#ifndef _MATRICES_H
#define _MATRICES_H

#include <string.h>
#include <math.h>

template <class T>
class Matrices
{
public:

    enum DIR { RIGHT = 0, UP = 4, FORWARD = 8 };

	// matrices..
	inline static void multiply(T m[2][2], const T a[2][2], const T b[2][2]);
	inline static void multiply(T m[3][3], const T a[3][3], const T b[3][3]);
	inline static void multiply(T m[4][4], const T a[4][4], const T b[4][4]);

	// vectors..
	inline static void multiply(T v[2], const T m[2][2], const T u[2]);
	inline static void multiply(T v[3], const T m[3][3], const T u[3]);
	inline static void multiply(T v[4], const T m[4][4], const T u[4]);
	inline static void multiply(T v[3], const T m[4][4], const T u[3], T w);

    inline static void copy(T m[2][2], const T n[2][2]);
    inline static void copy(T m[3][3], const T n[3][3]);
    inline static void copy(T m[4][4], const T n[4][4]);

    inline static void copy(T *m, const T *n, int count);

    inline static void identity(T m[2][2]);
	inline static void identity(T m[3][3]);
	inline static void identity(T m[4][4]);

    inline static void rotation(T m[2][2], T theta);

    inline static void rotationX(T m[3][3], T theta, bool left = true);
    inline static void rotationX(T m[4][4], T theta, bool left = true);

    inline static void rotationY(T m[3][3], T theta, bool left = true);    
    inline static void rotationY(T m[4][4], T theta, bool left = true);

    inline static void rotationZ(T m[3][3], T theta, bool left = true);    
    inline static void rotationZ(T m[4][4], T theta, bool left = true);

    inline static void rotationAxis(T m[3][3], T x, T y, T z, T theta, bool left = true);
    inline static void rotationAxis(T m[4][4], T x, T y, T z, T theta, bool left = true);

    inline static void scale(T m[2][2], T s) { ArrayUtil<T>::mul((T *)m, s, 4);  }
    inline static void scale(T m[3][3], T s) { ArrayUtil<T>::mul((T *)m, s, 9);  }
    inline static void scale(T m[4][4], T s) { ArrayUtil<T>::mul((T *)m, s, 16); }

    inline static void scale(T m[4][4], T a, T b, T c);
    
    inline static void translate(T m[4][4], T a, T b, T c);
    inline static void translate(T m[4][4], const T rot[4][4], T a, T b, T c);
    inline static void translate(T m[4][4], const T rot[4][4], T d, DIR dir)
    {
        T *r = (T *)rot + (int)dir;
        translate(m, *(r + 0) * d, *(r + 1) * d, *(r + 2) * d);
    }
    inline static bool orthogonal(const T m[4][4])
    {
		T *n = (T *)m;
        return *(n + 3) + *(n + 7) + *(n + 11) + *(n + 12) + *(n + 13) + *(n + 14) == 0.0f &&
			   *(n + 15) == 1.0f;
    }
	inline static bool isIdentity(const T m[4][4])
	{
		T *n = (T *)m;
		return *(n +  0) == 1.0f && *(n +  1) == 0.0f && *(n +  2) == 0.0f && *(n +  3) == 0.0f &&
			   *(n +  4) == 0.0f && *(n +  5) == 1.0f && *(n +  6) == 0.0f && *(n +  7) == 0.0f &&
			   *(n +  8) == 0.0f && *(n +  9) == 0.0f && *(n + 10) == 1.0f && *(n + 11) == 0.0f &&
			   *(n + 12) == 0.0f && *(n + 13) == 0.0f && *(n + 14) == 0.0f && *(n + 15) == 1.0f;
	}
/*
	inline static void perspectiveFovProjection(float m[4][4], float fovrad, float aspectRatio, float zNear, float zFar, bool rh)
	{
	    float handed = rh ? -1.0f : 1.0f,
		      h      = (float)(cos(fovrad / 2) / sin(fovrad / 2)),
		      w      = h / aspectRatio;

        memset(&m, 0, sizeof(m));

		m.data[0][0] = 2 * zNear / w;
		m.data[1][1] = 2 * zNear / h;
		m.data[2][2] = zFar / (zFar - zNear);
		m.data[3][2] = handed;
		m.data[2][3] = zNear * zFar / (zNear - zFar);
	}
	inline static void orthogonalProjection(float m[4][4], float widthOfViewVolume, float heightOfViewVolume, float zNear, float zFar, bool rh)
	{
	    float handed = rh ? -1.0f : 1.0f;
        memset(&m, 0, sizeof(m));

		m.data[0][0] = 2/widthOfViewVolume;
		m.data[1][1] = 2/heightOfViewVolume;
		m.data[2][2] = 1/(zNear-zFar);
		m.data[2][3] = zNear/(zNear-zFar);
		m.data[3][3] = handed;
	}
	inline static void perspectiveProjection(float m[4][4], float widthOfViewVolume, float heightOfViewVolume, float zNear, float zFar, bool rh)
	{
	    float handed = rh ? -1.0f : 1.0f;
        memset(&m, 0, sizeof(m));

		m.data[0][0] = 2*zNear/widthOfViewVolume;
		m.data[1][1] = 2*zNear/heightOfViewVolume;
		m.data[2][2] = zFar / ((zFar - zNear) * handed);
		m.data[3][2] = handed;
		m.data[2][3] = zNear*zFar/(zNear-zFar);
	}
	//! Builds a matrix that flattens geometry into a plane.
	inline static void shadowProjection(float m[4][4], const vec3 &light, const PlaneT<vec3> &plane, float point)
	{
	    vec3 norm = plane.normal;
		norm.normalize();
		float d = norm.dot(light);

		m.data[0][0] = norm.x * light.x + d;
		m.data[1][0] = norm.x * light.y;
		m.data[2][0] = norm.x * light.z;
		m.data[3][0] = norm.x * point;

		m.data[0][1] = norm.y * light.x;
		m.data[1][1] = norm.y * light.y + d;
		m.data[2][1] = norm.y * light.z;
		m.data[3][1] = norm.y * point;

		m.data[0][2] = norm.z * light.x;
		m.data[1][2] = norm.z * light.y;
		m.data[2][2] = norm.z * light.z + d;
		m.data[3][2] = norm.z * point;

		m.data[0][3] = plane.distance * light.x + d;
		m.data[1][3] = plane.distance * light.y;
		m.data[2][3] = plane.distance * light.z;
		m.data[3][3] = plane.distance * point;
	}
	inline static void lookAt(float &m[4][4], const vec3 &pos, const vec3 &target, const vec3 &up, bool rh)
	{
	    float handed = rh ? -1.0f : 1.0f;

		vec3 zaxis = (target - pos) * handed;
		zaxis.normalize();

		vec3 xaxis = up.cross(zaxis);
		xaxis.normalize();

		vec3 yaxis = zaxis.cross(xaxis);

        memset(&m, 0, sizeof(m));

		m.data[0][0] = xaxis.x;	m.data[1][0] = yaxis.x;	m.data[2][0] = zaxis.x;
		m.data[0][1] = xaxis.y;	m.data[1][1] = yaxis.y;	m.data[2][1] = zaxis.y;
		m.data[0][2] = xaxis.z;	m.data[1][2] = yaxis.z;	m.data[2][2] = zaxis.z;

		m.data[0][3] = -xaxis.dot(pos);
		m.data[1][3] = -yaxis.dot(pos);
		m.data[2][3] = -zaxis.dot(pos);
		m.data[3][3] = 1.0f;
	}*/

private:

    inline static void m_rot2x2(T *m, T cos_t, T sin_t);
    inline static void m_rotx3x3(T *m, T cos_t, T sin_t);
    inline static void m_roty3x3(T *m, T cos_t, T sin_t);
    inline static void m_rotz3x3(T *m, T cos_t, T sin_t);
    inline static void m_rotAxis3x3(T *m, T x, T y, T z, T cos_t, T sin_t);
    inline static void m_rotx4x4(T *m, T cos_t, T sin_t);
    inline static void m_roty4x4(T *m, T cos_t, T sin_t);
    inline static void m_rotz4x4(T *m, T cos_t, T sin_t);
    inline static void m_rotAxis4x4(T *m, T x, T y, T z, T cos_t, T sin_t);
    inline static void m_scale4x4(T *m, T a, T b, T c);
    inline static void m_trans4x4(T *m, T a, T b, T c);
    inline static void m_trans4x4(T *m, const T *r, T a, T b, T c);
    inline static void m_identity2x2(T *m);
    inline static void m_identity3x3(T *m);
    inline static void m_identity4x4(T *m);
    inline static void m_mvector2x2(T *v, T *m, T *u);
    inline static void m_mvector3x3(T *v, T *m, T *u);
    inline static void m_mvector4x4(T *v, T *m, T *u);
    inline static void m_mvector4x4x3(T *v, T *m, T *u, float w);
    inline static void m_mvector4x4x3w0(T *v, T *m, T *u);
    inline static void m_mvector4x4x3w1(T *v, T *m, T *u);
    inline static void m_mvector4x4x3wn(T *v, T *m, T *u, float w);
    inline static void m_mmatrix2x2(T *m, T *a, T *b);
    inline static void m_mmatrix3x3(T *m, T *a, T *b);
    inline static void m_mmatrix4x4(T *m, T *a, T *b);
};

// INLINE FUNCTION IMPLEMENTATION

template <class T>
inline void Matrices<T>::multiply(T m[2][2], const T a[2][2], const T b[2][2])
{
    m_mmatrix2x2((T *)m, (T *)a, (T *)b);
}

template <class T>
inline void Matrices<T>::multiply(T m[3][3], const T a[3][3], const T b[3][3])
{
    m_mmatrix3x3((T *)m, (T *)a, (T *)b);
}

template <class T>
inline void Matrices<T>::multiply(T m[4][4], const T a[4][4], const T b[4][4])
{
    m_mmatrix4x4((T *)m, (T *)a, (T *)b);
}

template <class T>
inline void Matrices<T>::multiply(T v[2], const T m[2][2], const T u[2])
{
    m_mvector2x2((T *)v, (T *)m, (T *)u);
}

template <class T>
inline void Matrices<T>::multiply(T v[3], const T m[3][3], const T u[3])
{
    m_mvector3x3((T *)v, (T *)m, (T *)u);
}

template <class T>
inline void Matrices<T>::multiply(T v[4], const T m[4][4], const T u[4])
{
    m_mvector4x4((T *)v, (T *)m, (T *)u);
}

template <class T>
inline void Matrices<T>::multiply(T v[3], const T m[4][4], const T u[3], T w)
{
    m_mvector4x4x3((T *)v, (T *)m, (T *)u, T w);
}

template <class T>
inline void Matrices<T>::copy(T m[2][2], const T n[2][2])
{
	memcpy(m, n, sizeof(2 * 2 * sizeof(T)));
}

template <class T>
inline void Matrices<T>::copy(T m[3][3], const T n[3][3])
{
	memcpy(m, n, sizeof(3 * 3 * sizeof(T)));
}

template <class T>
inline void Matrices<T>::copy(T m[4][4], const T n[4][4])
{
	memcpy(m, n, sizeof(4 * 4 * sizeof(T)));
}

template <class T>
inline void Matrices<T>::copy(T *m, const T *n, int count)
{
	memcpy(m, n, sizeof(count * sizeof(T)));
}

template <class T>
inline void Matrices<T>::identity(T m[2][2])
{
    m_identity2x2((T *)m);
}

template <class T>
inline void Matrices<T>::identity(T m[3][3])
{
    m_identity3x3((T *)m);
}

template <class T>
inline void Matrices<T>::identity(T m[4][4])
{
    m_identity4x4((T *)m);
}

template <class T>
inline void Matrices<T>::rotationX(T m[4][4], T theta, bool left)
{
	m_rotx4x4((T *)m, cos(theta), (left ? sin(theta) : -sin(theta)));
}

template <class T>
inline void Matrices<T>::rotationY(T m[4][4], T theta, bool left)
{
	m_roty4x4((T *)m, cos(theta), (left ? sin(theta) : -sin(theta)));
}

template <class T>
inline void Matrices<T>::rotationZ(T m[4][4], T theta, bool left)
{
	m_rotz4x4((T *)m, cos(theta), (left ? sin(theta) : -sin(theta)));
}

template <class T>
inline void Matrices<T>::rotationAxis(T m[4][4], T x, T y, T z, T theta, bool left)
{
    m_rotAxis4x4((T *)m, x, y, z, cos(theta), (left ? sin(theta) : -sin(theta)));
}

template <class T>
inline void Matrices<T>::scale(T m[4][4], T a, T b, T c)
{
    m_scale4x4((T *)m, a, b, c);
}

template <class T>
inline void Matrices<T>::translate(T m[4][4], T a, T b, T c)
{
    m_trans4x4((T *)m, a, b, c);
}

template <class T>
inline void Matrices<T>::translate(T m[4][4], const T rotation[4][4], T a, T b, T c)
{
	m_trans4x4((T *)m, (T *)rotation, a, b, c);
}

template <class T>
inline void Matrices<T>::m_rot2x2(T *m, T cos_t, T sin_t)
{
    *(m + 0) =  cos_t; *(m + 1) = -sin_t;
    *(m + 2) =  sin_t; *(m + 3) =  cos_t;
}

template <class T>
inline void Matrices<T>::m_rotx3x3(T *m, T cos_t, T sin_t)
{
    *(m + 0) =  1.0f;  *(m + 1) =  0.0f;  *(m + 2) =  0.0f;
    *(m + 3) =  0.0f;  *(m + 4) =  cos_t; *(m + 5) = -sin_t;
    *(m + 6) =  0.0f;  *(m + 7) =  sin_t; *(m + 8) =  cos_t;
}

template <class T>
inline void Matrices<T>::m_roty3x3(T *m, T cos_t, T sin_t)
{
    *(m + 0) =  cos_t; *(m + 1) =  0.0f;  *(m + 2) = sin_t;
    *(m + 3) =  0.0f;  *(m + 4) =  1.0f;  *(m + 5) = 0.0f;
    *(m + 6) = -sin_t; *(m + 7) =  0.0f;  *(m + 8) = cos_t;
}

template <class T>
inline void Matrices<T>::m_rotz3x3(T *m, T cos_t, T sin_t)
{
    *(m + 0) =  cos_t; *(m + 1) = -sin_t; *(m + 2) = 0.0f;
    *(m + 3) =  sin_t; *(m + 4) =  cos_t; *(m + 5) = 0.0f;
    *(m + 6) =  0.0f;  *(m + 7) =  0.0f;  *(m + 8) = 1.0f;
}

template <class T>
inline void Matrices<T>::m_rotAxis3x3(T *m, T x, T y, T z, T cos_t, T sin_t)
{
    T     t   = 1 - cos_t,
          sx  = sin_t * x, sy  = sin_t * y, sz  = sin_t * z,
          tx  = t     * x, ty  = t     * y, tz  = t     * z,
          txx = tx    * x, txy = tx    * y, tyy = ty    * y,
          tyz = ty    * z, tzz = tz    * z, txz = tx    * z;

    *(m + 0) =  txx + cos_t; *(m + 1) =  txy - sz;    *(m + 2) = txz + sy;
    *(m + 3) =  txy + sz;    *(m + 4) =  tyy + cos_t; *(m + 5) = tyz - sx;
    *(m + 6) =  txz - sy;    *(m + 7) =  tyz + sx;    *(m + 8) = tzz + cos_t;
}

template <class T>
inline void Matrices<T>::m_rotx4x4(T *m, T cos_t, T sin_t)
{
    *(m + 0)  =   1.0f; *(m + 1)  =  0.0f;  *(m + 2)  =  0.0f;   *(m + 3)  = 0.0f;
    *(m + 4)  =   0.0f; *(m + 5)  =  cos_t; *(m + 6)  = -sin_t;  *(m + 7)  = 0.0f;
    *(m + 8)  =   0.0f; *(m + 9)  =  sin_t; *(m + 10) =  cos_t;  *(m + 11) = 0.0f;
    *(m + 12) =   0.0f; *(m + 13) =  0.0f;  *(m + 14) =  0.0f;   *(m + 15) = 1.0f;
}

template <class T>
inline void Matrices<T>::m_roty4x4(T *m, T cos_t, T sin_t)
{
    *(m + 0)  =  cos_t; *(m + 1)  =  0.0f;  *(m + 2)  =  sin_t;  *(m + 3)  = 0.0f;
    *(m + 4)  =  0.0f;  *(m + 5)  =  1.0f;  *(m + 6)  =  0.0f;   *(m + 7)  = 0.0f;
    *(m + 8)  = -sin_t; *(m + 9)  =  0.0f;  *(m + 10) =  cos_t;  *(m + 11) = 0.0f;
    *(m + 12) =  0.0f;  *(m + 13) =  0.0f;  *(m + 14) =  0.0f;   *(m + 15) = 1.0f;
}

template <class T>
inline void Matrices<T>::m_rotz4x4(T *m, T cos_t, T sin_t)
{
    *(m + 0)  =  cos_t; *(m + 1)  = -sin_t; *(m + 2)  =  0.0f;   *(m + 3)  = 0.0f;
    *(m + 4)  =  sin_t; *(m + 5)  =  cos_t; *(m + 6)  =  0.0f;   *(m + 7)  = 0.0f;
    *(m + 8)  =  0.0f;  *(m + 9)  =  0.0f;  *(m + 10) =  1.0f;   *(m + 11) = 0.0f;
    *(m + 12) =  0.0f;  *(m + 13) =  0.0f;  *(m + 14) =  0.0f;   *(m + 15) = 1.0f;
}

template <class T>
inline void Matrices<T>::m_rotAxis4x4(T *m, T x, T y, T z, T cos_t, T sin_t)
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

template <class T>
inline void Matrices<T>::m_scale4x4(T *m, T a, T b, T c)
{
    *(m + 0)  =  a;     *(m + 1)  =  0.0f;  *(m + 2)  =  0.0f;   *(m + 3)  = 0.0f;
    *(m + 4)  =  0.0f;  *(m + 5)  =  b;     *(m + 6)  =  0.0f;   *(m + 7)  = 0.0f;
    *(m + 8)  =  0.0f;  *(m + 9)  =  0.0f;  *(m + 10) =  c;      *(m + 11) = 0.0f;
    *(m + 12) =  0.0f;  *(m + 13) =  0.0f;  *(m + 14) =  0.0f;   *(m + 15) = 1.0f;
}

template <class T>
inline void Matrices<T>::m_trans4x4(T *m, T a, T b, T c)
{
    *(m + 0)  =  1.0f;  *(m + 1)  =  0.0f;  *(m + 2)  =  0.0f;   *(m + 3)  = a;
    *(m + 4)  =  0.0f;  *(m + 5)  =  1.0f;  *(m + 6)  =  0.0f;   *(m + 7)  = b;
    *(m + 8)  =  0.0f;  *(m + 9)  =  0.0f;  *(m + 10) =  1.0f;   *(m + 11) = c;
    *(m + 12) =  0.0f;  *(m + 13) =  0.0f;  *(m + 14) =  0.0f;   *(m + 15) = 1.0f;
}

template <class T>
inline void Matrices<T>::m_trans4x4(T *m, const T *r, T a, T b, T c)
{
    /*
    transform --

    | r11 r12 r13 r11*x + r12*y + r13*z |
    | r21 r22 r23 r21*x + r22*y + r23*z |
    | r31 r32 r33 r31*x + r32*y + r33*z |
    |  0   0   0            1           |
    */

    copy(m, r, 4 * 4);

    *(m + 3)  = *(m + 0) * a + (*m + 1) * b + (*m + 2)  * c;
    *(m + 7)  = *(m + 4) * a + (*m + 5) * b + (*m + 6)  * c;
    *(m + 12) = *(m + 8) * a + (*m + 9) * b + (*m + 10) * c;
}

template <class T>
inline void Matrices<T>::m_identity2x2(T *m)
{
	*(m + 0) = 1.0f; *(m + 1) = 0.0f;
	*(m + 2) = 0.0f; *(m + 3) = 1.0f;
}

template <class T>
inline void Matrices<T>::m_identity3x3(T *m)
{
	*(m + 0) = 1.0f; *(m + 1) = 0.0f; *(m + 2) = 0.0f;
	*(m + 3) = 0.0f; *(m + 4) = 1.0f; *(m + 5) = 0.0f;
	*(m + 6) = 0.0f; *(m + 7) = 0.0f; *(m + 8) = 1.0f;
}

template <class T>
inline void Matrices<T>::m_identity4x4(T *m)
{
	*(m + 0)  = 1.0f; *(m + 1)  = 0.0f; *(m + 2)  = 0.0f; *(m + 3)  = 0.0f;
	*(m + 4)  = 0.0f; *(m + 5)  = 1.0f; *(m + 6)  = 0.0f; *(m + 7)  = 0.0f;
	*(m + 8)  = 0.0f; *(m + 9)  = 0.0f; *(m + 10) = 1.0f; *(m + 11) = 0.0f;
	*(m + 12) = 0.0f; *(m + 13) = 0.0f; *(m + 14) = 0.0f; *(m + 15) = 1.0f;
}

template <class T>
inline void Matrices<T>::m_mvector2x2(T *v, T *m, T *u)
{
    *(v + 0) = *(m + 0) * *(u + 0) + *(m + 2) * *(u + 1);
    *(v + 1) = *(m + 1) * *(u + 0) + *(m + 3) * *(u + 1);
}

template <class T>
inline void Matrices<T>::m_mvector3x3(T *v, T *m, T *u)
{
    *(v + 0) = *(m + 0) * *(u + 0) + *(m + 3) * *(u + 1) + *(m + 6) * *(u + 2);
    *(v + 1) = *(m + 1) * *(u + 0) + *(m + 4) * *(u + 1) + *(m + 7) * *(u + 2);
    *(v + 2) = *(m + 2) * *(u + 0) + *(m + 5) * *(u + 1) + *(m + 8) * *(u + 2);
}

template <class T>
inline void Matrices<T>::m_mvector4x4(T *v, T *m, T *u)
{
    *(v + 0) = *(m + 0) * *(u + 0) + *(m + 4) * *(u + 1) + *(m + 8)  * *(u + 2) + *(m + 12) * *(u + 3);
    *(v + 1) = *(m + 1) * *(u + 0) + *(m + 5) * *(u + 1) + *(m + 9)  * *(u + 2) + *(m + 13) * *(u + 3);
    *(v + 2) = *(m + 2) * *(u + 0) + *(m + 6) * *(u + 1) + *(m + 10) * *(u + 2) + *(m + 14) * *(u + 3);
    *(v + 3) = *(m + 3) * *(u + 0) + *(m + 7) * *(u + 1) + *(m + 11) * *(u + 2) + *(m + 15) * *(u + 3);
}

template <class T>
inline void Matrices<T>::m_mvector4x4x3(T *v, T *m, T *u, float w)
{
	if      (w == 0.0f) m_mvector4x4x3w0(v, m, u);
	else if (w == 1.0f) m_mvector4x4x3w1(v, m, y);
	else                m_mvector4x4x3wn(v, m, y, w);
}

template <class T>
inline void Matrices<T>::m_mvector4x4x3wn(T *v, T *m, T *u, float w)
{
    *(v + 0) = *(m + 0) * *(u + 0) + *(m + 4) * *(u + 1) + *(m + 8)  * *(u + 2) + *(m + 12) * w;
    *(v + 1) = *(m + 1) * *(u + 0) + *(m + 5) * *(u + 1) + *(m + 9)  * *(u + 2) + *(m + 13) * w;
    *(v + 2) = *(m + 2) * *(u + 0) + *(m + 6) * *(u + 1) + *(m + 10) * *(u + 2) + *(m + 14) * w;
}

template <class T>
inline void Matrices<T>::m_mvector4x4x3w1(T *v, T *m, T *u)
{
    *(v + 0) = *(m + 0) * *(u + 0) + *(m + 4) * *(u + 1) + *(m + 8)  * *(u + 2) + *(m + 12);
    *(v + 1) = *(m + 1) * *(u + 0) + *(m + 5) * *(u + 1) + *(m + 9)  * *(u + 2) + *(m + 13);
    *(v + 2) = *(m + 2) * *(u + 0) + *(m + 6) * *(u + 1) + *(m + 10) * *(u + 2) + *(m + 14);
}

template <class T>
inline void Matrices<T>::m_mvector4x4x3w0(T *v, T *m, T *u)
{
    *(v + 0) = *(m + 0) * *(u + 0) + *(m + 4) * *(u + 1) + *(m + 8)  * *(u + 2);
    *(v + 1) = *(m + 1) * *(u + 0) + *(m + 5) * *(u + 1) + *(m + 9)  * *(u + 2);
    *(v + 2) = *(m + 2) * *(u + 0) + *(m + 6) * *(u + 1) + *(m + 10) * *(u + 2);
}

template <class T>
inline void Matrices<T>::m_mmatrix2x2(T *m, T *a, T *b)
{
    *(m + 0) = *(a + 0) * *(b + 0) + *(a + 1) * *(b + 2);
    *(m + 1) = *(a + 0) * *(b + 1) + *(a + 1) * *(b + 3);
    *(m + 2) = *(a + 2) * *(b + 0) + *(a + 3) * *(b + 2);
    *(m + 3) = *(a + 2) * *(b + 1) + *(a + 3) * *(b + 3);
}

template <class T>
inline void Matrices<T>::m_mmatrix3x3(T *m, T *a, T *b)
{
    *(m + 0) = *(a + 0) * *(b + 0) + *(a + 1) * *(b + 3) + *(a + 2) * *(b + 6);
    *(m + 1) = *(a + 0) * *(b + 1) + *(a + 1) * *(b + 4) + *(a + 2) * *(b + 7);
    *(m + 2) = *(a + 0) * *(b + 2) + *(a + 1) * *(b + 5) + *(a + 2) * *(b + 8);
    *(m + 3) = *(a + 3) * *(b + 0) + *(a + 4) * *(b + 3) + *(a + 5) * *(b + 6);
    *(m + 4) = *(a + 3) * *(b + 1) + *(a + 4) * *(b + 4) + *(a + 5) * *(b + 7);
    *(m + 5) = *(a + 3) * *(b + 2) + *(a + 4) * *(b + 5) + *(a + 5) * *(b + 8);
    *(m + 6) = *(a + 6) * *(b + 0) + *(a + 7) * *(b + 3) + *(a + 8) * *(b + 6);
    *(m + 7) = *(a + 6) * *(b + 1) + *(a + 7) * *(b + 4) + *(a + 8) * *(b + 7);
    *(m + 8) = *(a + 6) * *(b + 2) + *(a + 7) * *(b + 5) + *(a + 8) * *(b + 8);
}

template <class T>
inline void Matrices<T>::m_mmatrix4x4(T *m, T *a, T *b)
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

typedef Matrices<float>  matrices;
typedef Matrices<double> matricesd;

#endif  /** _MATRICES_H */