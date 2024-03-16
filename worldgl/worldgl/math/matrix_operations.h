#pragma once

#include "determinant.h"

template <typename T, typename U, size_t N>
inline void copy(const std::array<T, N> &a, std::array<U, N> &b)
{
    size_t i = 0;
    for (const auto &v : a) b[i++] = U(v);
}

template <typename T, typename U>
inline void copy(const T *a, U *b, size_t count)
{
    for (const T *e = e + count; a != b; ++a, ++b) *b = *a;
}

template <typename T, typename U>
inline void copy3x3to4x4(T a, T b, T c,
                         T d, T e, T f,
                         T g, T h, T i, U *t)
{
    *t++ =    a;  *t++ =    b;  *t++ =    c;  *t++ = U(0);
    *t++ =    d;  *t++ =    e;  *t++ =    f;  *t++ = U(0);
    *t++ =    g;  *t++ =    h;  *t++ =    i;  *t++ = U(0);
    *t++ = U(0);  *t++ = U(0);  *t++ = U(0);  *t++ = U(1);
}

template <typename T, typename U>
inline void copy3x3to4x4(const T *a, U *b)
{
    copy3x3to4x4(a[0], a[1], a[2], a[3], a[4], a[5], a[6], a[7], a[8], b);
}

template <typename T, typename U>
inline void copy3x3(
    T a, T b, T c,
    T d, T e, T f,
    T g, T h, T i,
    U *t)
{
    *t++ = a; *t++ = b; *t++ = c;
    *t++ = d; *t++ = e; *t++ = f;
    *t++ = g; *t++ = h; *t++ = i;
}

template <typename T, typename U>
inline void copy3x3transpose(
    T a, T b, T c,
    T d, T e, T f,
    T g, T h, T i,
    U *t)
{
    *t++ = a; *t++ = b; *t++ = c;
    *t++ = d; *t++ = e; *t++ = f;
    *t++ = g; *t++ = h; *t++ = i;
}

template <typename T, typename U>
inline void copy4x4(
    T a, T b, T c, T d,
    T e, T f, T g, T h,
    T i, T j, T k, T l,
    T m, T n, T o, T p,
    U *t)
{
    *t++ = a; *t++ = b; *t++ = c; *t++ = d;
    *t++ = e; *t++ = f; *t++ = g; *t++ = h;
    *t++ = i; *t++ = j; *t++ = k; *t++ = l;
    *t++ = m; *t++ = n; *t++ = o; *t++ = p;
}

template <typename T, typename U>
inline void copy4x4transpose(
    T a, T b, T c, T d,
    T e, T f, T g, T h,
    T i, T j, T k, T l,
    T m, T n, T o, T p,
    U *t)
{
    *t++ = a; *t++ = e; *t++ = i; *t++ = m;
    *t++ = b; *t++ = f; *t++ = j; *t++ = n;
    *t++ = c; *t++ = g; *t++ = k; *t++ = o;
    *t++ = d; *t++ = h; *t++ = l; *t++ = p;
}

template <typename T, typename U>
bool invert3x3(T a, T b, T c,
               T d, T e, T f,
               T g, T h, T i, U *mb, bool transpose) // $TODO test this
{
    T di = det(a, b, c,  d, e, f,  g, h, i);
    if (di == T(0))
        return false; // not invertible..
    di = 1. / di;
    T _a =  det(e, f, h, i) * di, _b = -det(d, f, g, i) * di, _c =  det(d, e, g, h) * di,
      _d = -det(b, c, h, i) * di, _e =  det(a, c, g, i) * di, _f = -det(a, b, g, h) * di,
      _g =  det(b, c, e, f) * di, _h = -det(a, c, d, f) * di, _i =  det(a, b, d, e) * di;
    if (transpose) {
        // transpose has to be the opposite here..
        copy3x3(_a, _b, _c,  _d, _e, _f,  _g, _h, _i, mb);
    } else {
        copy3x3transpose(_a, _b, _c,  _d, _e, _f,  _g, _h, _i, mb);
    }
    return true;
}

template <typename T, typename U>
bool invert3x3(const T *a, U *b, bool transpose)
{;
    return invert3x3(a[0], a[1], a[2],  a[3], a[4], a[5],  a[6], a[7], a[8], b, transpose);
}

template <typename T, typename U>
bool invert4x4(T a, T b, T c, T d,
               T e, T f, T g, T h,
               T i, T j, T k, T l,
               T m, T n, T o, T p, U *mb, bool transpose)
{
    if (m == T(0) && n == T(0) && o == T(0) && d == T(0) &&
        h == T(0) && l == T(0) && p == T(1)) { // special case for sub 3x3..
        T x[9] = { 0 };
        if (!invert3x3(a, b, c,  e, f, g,  i, j, k, x, transpose)) {
            return false; // not invertible..
        }
        copy3x3to4x4(x, mb);
        return true;
    }
    T di = det(a, b, c, d,  e, f, g, h,  i, j, k, l,  m, n, o, p);
    if (di == 0.)
        return false; // not invertible..
    di = T(1) / di;
    T _a =  det(f, g, h, j, k, l, n, o, p) * di,
      _b = -det(e, g, h, i, k, l, m, o, p) * di,
      _c =  det(e, f, h, i, j, l, m, n, p) * di,
      _d = -det(e, f, g, i, j, k, m, n, o) * di,
      _e = -det(b, c, d, j, k, l, n, o, p) * di,
      _f =  det(a, c, d, i, k, l, m, o, p) * di,
      _g = -det(a, b, d, i, j, l, m, n, p) * di,
      _h =  det(a, b, c, i, j, k, m, n, o) * di,
      _i =  det(b, c, d, f, g, h, n, o, p) * di,
      _j = -det(a, c, d, e, g, h, m, o, p) * di,
      _k =  det(a, b, d, e, f, h, m, n, p) * di,
      _l = -det(a, b, c, e, f, g, m, n, o) * di,
      _m = -det(b, c, d, f, g, h, j, k, l) * di,
      _n =  det(a, c, d, e, g, h, i, k, l) * di,
      _o = -det(a, b, d, e, f, h, i, j, l) * di,
      _p =  det(a, b, c, e, f, g, i, j, k) * di;
    if (transpose) {
        copy4x4(
            _a, _b, _c, _d,
            _e, _f, _g, _h,
            _i, _j, _k, _l,
            _m, _n, _o, _p, mb); // transpose has to be the opposite here..
    } else {
        copy4x4transpose(
            _a, _b, _c, _d,
            _e, _f, _g, _h,
            _i, _j, _k, _l,
            _m, _n, _o, _p, mb); // transpose has to be the opposite here..
    }
    return true;
}

bool invert4x4(const double *ma, double *mb, bool transpose = false);
bool invert_projection4x4(const double *m, double *n);

bool invert4x4(const float *ma, float *mb, bool transpose = false);
bool invert_projection4x4(const float *m, float *n);

void scale4x4(double a, double b, double c, double d, double *m);

void x_rotate4x4(double theta, double *m, bool rh = false);
void y_rotate4x4(double theta, double *m, bool rh = false);
void z_rotate4x4(double theta, double *m, bool rh = false);

