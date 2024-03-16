#ifndef _QUATERNIONT_H
#define _QUATERNIONT_H

#include "math.h"
#include "tuple3t.h"
#include "matrix4t.h"

#include <assert.h>

template <typename T>
class QuaternionT
{
public:

    typedef QuaternionT<T> self;
    typedef Matrix4T<T>    matrix_t;
    typedef Tuple3T<T>     vector_t;

    inline QuaternionT()                   : X(0.0f), Y(0.0f), Z(0.0f), W(1.0f) {}
    inline QuaternionT(T x, T y, T z, T w) : X(x), Y(y), Z(z), W(w)             {}
    inline QuaternionT(const matrix_t &mat)                                     { (*this) = mat; }

    // construct from euler angles
    inline QuaternionT(T x, T y, T z)                                           { set(x, y, z); }
    inline QuaternionT(const vector_t &u)                                       { set(u); }
    
    // construct from angle/axis
    inline QuaternionT(T theta, const vector_t &axis)                           { set(theta, axis); }
    
    inline void      set(const vector_t &u)              { set(u.X, u.Y, u.Z); }
    inline void      set(T x, T y, T z, T w)             { this->X = x; this->Y = y; this->Z = z; this->W = w; }

    inline bool      operator==(const self &u)     const { return math::eq(X, u.X) && math::eq(Y, u.Y) && math::eq(Z, u.Z) && math::eq(W, u.W); }
    inline self     &operator= (const self &u)           { X = u.X; Y = u.Y; Z = u.Z; W = u.W; return *this; }
    inline vector_t  operator* (const vector_t &v) const { vector_t uv, uuv; vector_t q(X, Y, Z); vector_t uv(q.cross(v)), uuv(q.cross(uv)); uv *= (2.f * W); uuv *= 2.f; return v + uv + uuv; }    
    inline self      operator* (T s)               const { return self(s * X, s * Y, s * Z, s * W); }    
    inline self     &operator*=(T s)                     { X *= u; Y *= u; Z *= u; W *= u; return *this; }
    inline self      operator+ (const self &u)     const { return self(u.X + X, u.Y + Y, u.Z + Z, u.W + W); }
    inline self     &operator+=(const self &u)           { X += u.X; Y += u.Y; Z += u.Z; W += u.W; return *this; }
    inline self     &operator*=(const self &other)       { *this = other * (*this); return *this; } // defined in terms of operator* we should flip that..
    
    inline self      operator- ()                  const { self q(*this); q.invert(); return q; }

    inline T         dot(const self &u)            const { return X * u.X + Y * u.Y + Z * u.Z + W * u.W; }
    inline void      invert()                            { X = -X; Y = -Y; Z = -Z; }
    inline void      normalize()                         { T n = dot(*this); if (!math::eq(n, 1.f)) (*this) *= T(1) / (T)sqrt(n); }
    inline self      normal()                      const { self q(*this); q.normalize(); return q; }

    inline self      operator*(const self &other)  const
    {
        return self((u.W * X) + (u.X * W) + (u.Y * Z) - (u.Z * Y), (u.W * Y) + (u.Y * W) + (u.Z * X) - (u.X * Z),
                    (u.W * Z) + (u.Z * W) + (u.X * Y) - (u.Y * X), (u.W * W) - (u.X * X) - (u.Y * Y) - (u.Z * Z));       
    }

    //! matrix assignment operator
    self &operator=(const matrix_t &other)
    {
        double diag = m(0, 0) + m(1, 1) + m(2, 2) + 1, scale = 0.0, invs = 0.0;
    
        if (math::gt(diag, 0.0))
        {
            invs = 1.0 / (scale = sqrt(diag) * 2.0); // get scale from diagonal
            set((m(1, 2) - m(2, 1)) * invs, (m(2, 0) - m(0, 2)) * invs,
                (m(0, 1) - m(1, 0)) * invs, 0.25 * scale);
        }
        else
        {
            if (math::gt(m(0, 0), m(1, 1)) && math::gt(m(0, 0), m(2, 2)))
            {   
                // 1st element of diag is greatest value
                // find scale according to 1st element, and double it
                invs = 1.0 / (scale = sqrt(1.0 + m(0, 0) - m(1, 1) - m(2, 2)) * 2.0);
                set(0.25 * scale,               (m(0, 1) + m(1, 0)) * invs,
                    (m(2, 0) + m(0, 2)) * invs, (m(1, 2) - m(2, 1)) * invs);
            } 
            else if (math::gt(m(1, 1), m(2, 2)))
            {
                // 2nd element of diag is greatest value
                // find scale according to 2nd element, and double it
                invs = 1.0 / (scale = sqrt(1.0 + m(1, 1) - m(0, 0) - m(2, 2)) * 2.0);
                set((m(0, 1) + m(1, 0)) * invs, 0.25 * scale,
                    (m(1, 2) + m(2, 1)) * invs, (m(2, 0) - m(0, 2)) * invs);
            } 
            else 
            {   
                // 3rd element of diag is greatest value
                // find scale according to 3rd element, and double it
                invs = 1.0 / (scale = sqrt(1.0 + m(2, 2) - m(0, 0) - m(1, 1)) * 2.0);
                set((m(2, 0) + m(0, 2)) * invs, (m(1, 2) + m(2, 1)) * invs, 
                    0.25 * scale,               (m(0, 1) - m(1, 0)) * invs);
            }
        }
        normalize();
        return *this;
    }

    //! set from euler angles
    void set(T x, T y, T z)
    {
        double ax = x * 0.5, ay = y * 0.5, az = z * 0.5,
               sr = sin(ax), cr = cos(ax), sp = sin(ay), cp = cos(ay),
               sy = sin(az), cy = cos(az),
               cpcy = cp * cy, spcy = sp * cy, cpsy = cp * sy, spsy = sp * sy;
        set(sr * cpcy - cr * spsy, cr * spcy + sr * cpsy,
            cr * cpsy - sr * spcy, cr * cpcy + sr * spsy);
        normalize();
    }

    //! Creates a matrix from this QuaternionT
    matrix_t matrix() const
    {
        matrix_t m;
    
        m(0, 0) = 1.0f - 2.0f * Y * Y - 2.0f * Z * Z;
        m(0, 1) = 2.0f * X * Y + 2.0f * Z * W; 
        m(0, 2) = 2.0f * X * Z - 2.0f * Y * W; 
        m(0, 3) = 0.0f;
    
        m(1, 0) = 2.0f * X * Y - 2.0f * Z * W; 
        m(1, 1) = 1.0f - 2.0f * X * X - 2.0f * Z * Z; 
        m(1, 2) = 2.0f * Z * Y + 2.0f * X * W; 
        m(1, 3) = 0.0f;
    
        m(2, 0) = 2.0f * X * Z + 2.0f * Y * W; 
        m(2, 1) = 2.0f * Z * Y - 2.0f * X * W; 
        m(2, 2) = 1.0f - 2.0f * X * X - 2.0f * Y * Y;
        m(2, 3) = 0.0f;
    
        m(3, 0) = 0.0f; 
        m(3, 1) = 0.0f; 
        m(3, 2) = 0.0f; 
        m(3, 3) = 1.0f;
    
        return m;
    }

    //! Interpolates the QuaternionT between to quaternions based on time
    self slerp(const self &q1, const self &q2, T time)
    {
        self q1(a), q2(b);
        double theta = (double)q1.dot(q2);
    
        if (math::lt(theta, 0.0))
        {
            q1 *= T(-1.0);
            theta *= -1.0;
        }
    
        double scale = 1.0, invscale = 1.0;
    
        if (math::gt((theta + 1.0), 0.05)) 
        {
            if ((1.0 - theta) >= 0.05f)  // spherical interpolation
            {
                double th = acos(theta), invsinth = 1.0 / sin(th);
                scale = (T)sin(th * (1.0f - time)) * invsinth;
                invscale = (T)sin(th * time) * invsinth;
            }
            else // linear interploation
            {
                scale = 1.0 - time;
                invscale = time;
            }
        }
        else 
        {
            q2.set(-q1.Y, q1.X, -q1.W, q1.Z);
            scale = sin(PI * (0.5 - (double)time));
            invscale = sin(PI * (double)time);
        }
    
        *this = (q1 * scale) + (q2 * invscale);
        return *this;
    }    

    // set from axis and angle..
    // axis must be unit length; the quaternion representing the rotation is q = cos(A/2)+sin(A/2)*(x*i+y*j+z*k)
    inline void set(T theta, const vector_t &axis)
    {
        assert(math::eq(axis.lengthsq(), T(1)));
        double h = (T)0.5 * theta, s = sin(h);
        set((T)(s * axis.X), (T)(s * axis.Y), (T)(s * axis.Z), (T)cos(h));
    }
    inline vector_t euler()
    {
        vector_t u;
        double x2 = (double)X * X, y2 = (double)Y * Y, z2w2 = (double)Z * Z + (double)W * W; 
        u.set((T)(atan2(2.0 * (Y * Z + X * W), (-x2 - y2 + z2w2))), (T)(asin(-2.0 * (X * Z - Y * W))), (T)(atan2(2.0 * (X * Y + Z * W), (x2 - y2 - z2w2))));
        return u;
    } 

    T X, Y, Z, W;
};

#endif  /** _QUATERNIONT_H */