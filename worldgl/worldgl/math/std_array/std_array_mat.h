#pragma once

#include <array>
#include <cmath>
#include "math_typedefs.h"
#include "std_array_2.h"
#include "std_array_3.h"
#include "std_array_4.h"
#include "determinant.h"
#include "matrix_operations.h"
#include "compile_time.h"

namespace {
template <typename T> constexpr T &M(std::array<T, 9>  &m, size_t col, size_t row)  { return m[row * 3 + col]; }
template <typename T> constexpr T &M(std::array<T, 16> &m, size_t col, size_t row)  { return m[row * 4 + col]; }
template <typename T> constexpr T &GM(std::array<T, 9>  &m, size_t col, size_t row) { return m[row][col]; }
template <typename T> constexpr T &GM(std::array<T, 16> &m, size_t col, size_t row) { return m[row][col]; }
}

#define A2 std::array<T, 2>
#define A3 std::array<T, 3>
#define A4 std::array<T, 4>

template <typename T> constexpr A2 v2(T a, T b)                    { return A2 { a, b }; }
template <typename T> constexpr A3 v3(T a, T b, T c)               { return A3 { a, b, c }; }
template <typename T> constexpr A3 v3(const A2 &a, T b)            { return A3 { a[0], a[1], b }; }
template <typename T> constexpr A3 v3(T a, const A2 &b)            { return A3 { a, b[0], b[1] }; }
template <typename T> constexpr A4 v4(T a, T b, T c, T d)          { return A4 { a, b, c, d }; }
template <typename T> constexpr A4 v4(T a, const A3 &b)            { return A4 { a, b[0], b[1], b[2] }; }
template <typename T> constexpr A4 v4(T a, T b, const A2 &c)       { return A4 { a, b, c[0], c[1] }; }
template <typename T> constexpr A4 v4(const A2 &a, T b, T c)       { return A4 { a[0], a[1], b, c }; }
template <typename T> constexpr A4 v4(const A2 &a, const A2 &b)    { return A4 { a[0], a[1], b[0], b[1] }; }
template <typename T> constexpr A4 v4(const A3 &a, T b)            { return A4 { a[0], a[1], a[2], b }; }

template <typename T> constexpr A3 cross(const A3 &a, const A3 &b) { return v3(a[1] * b[2] - a[2] * b[1], a[2] * b[0] - a[0] * b[2], a[0] * b[1] - a[1] * b[0]); }

//template <typename T> constexpr A3 dot(const A3 &a, const A3 &b)   { return v3(a[1] * b[2] - a[2] * b[1], a[2] * b[0] - a[0] * b[2], a[0] * b[1] - a[1] * b[0]); }
//inline T  dot(const A3 &p)                                const { return x * p.x + y * p.y + z * p.z; }

template <typename T, size_t N, size_t SQRTN = compile_time::isqrt(N)> inline void set_identity(std::array<T, N> &m)
{
    m = std::array<T, N> { T(0) };
    for (size_t i = 0; i < SQRTN; i++) {
        M(m, i, i) = T(1);
    }
}

template <typename T, size_t N> inline void set_scale(std::array<T, N> &m, T x, T y, T z)
{
    M(m, 0, 0) = x; M(m, 1, 1) = y; M(m, 2, 2) = z;
}

template <typename T, size_t N> inline void set_scale(std::array<T, N> &m, T x, T y)
{
    M(m, 0, 0) = x; M(m, 1, 1) = y;
}

template <typename T, size_t N> inline void set_translate(std::array<T, N> &m, T x, T y, T z)
{
    M(m, 0, 3) = x; M(m, 1, 3) = y; M(m, 2, 3) = z;
}

template <typename T, size_t N> inline void set_rotate_x(std::array<T, N> &m, T cos_th, T sin_th)
{
    M(m, 1, 1) = cos_th; M(m, 2, 1) = -sin_th;
    M(m, 1, 2) = sin_th; M(m, 2, 2) =  cos_th;
}

template <typename T, size_t N> inline void set_rotate_y(std::array<T, N> &m, T cos_th, T sin_th)
{
    M(m, 0, 0) =  cos_th; M(m, 2, 0) = sin_th;
    M(m, 0, 2) = -sin_th; M(m, 2, 2) = cos_th;
}

template <typename T, size_t N> inline void set_rotate_z(std::array<T, N> &m, T cos_th, T sin_th)
{
    M(m, 0, 0) = cos_th; M(m, 1, 0) = -sin_th;
    M(m, 0, 1) = sin_th; M(m, 1, 1) =  cos_th;
}

template <typename T, size_t N> inline void set_rotate_x(std::array<T, N> &m, T theta, bool rh)
{
    set_rotate_x(m, std::cos(theta), rh ? -std::sin(theta) : std::sin(theta));
}

template <typename T, size_t N> inline void set_rotate_y(std::array<T, N> &m, T theta, bool rh)
{
    set_rotate_y(m, std::cos(theta), rh ? -std::sin(theta) : std::sin(theta));
}

template <typename T, size_t N> inline void set_rotate_z(std::array<T, N> &m, T theta, bool rh)
{
    set_rotate_z(m, std::cos(theta), rh ? -std::sin(theta) : std::sin(theta));
}

template <typename T, size_t N> inline void set_rotate_axis(std::array<T, N> &m, T x, T y, T z, T cos_th, T sin_th)
{
    T t   = 1 - cos_th,
      sx  = sin_th * x, sy  = sin_th * y, sz  = sin_th * z,
      tx  = t      * x, ty  = t      * y, tz  = t      * z,
      txx = tx     * x, txy = tx     * y, tyy = ty     * y,
      tyz = ty     * z, tzz = tz     * z, txz = tx     * z;

    M(m, 0, 0) = txx + cos_th; M(m, 1, 0) = txy - sz;     M(m, 2, 0) = txz + sy;
    M(m, 0, 1) = txy + sz;     M(m, 1, 1) = tyy + cos_th; M(m, 2, 1) = tyz - sx;
    M(m, 0, 2) = txz - sy;     M(m, 1, 2) = tyz + sx;     M(m, 2, 2) = tzz + cos_th;
}

template <typename T, size_t N> inline void set_rotate_axis(std::array<T, N> &m, T x, T y, T z, T theta, bool rh)
{
    set_rotate_axis(m, x, y, z, std::cos(theta), rh ? -std::sin(theta) : std::sin(theta));
}

template <typename T> inline void mul_mat_vec(std::array<T, 4> &v, const std::array<T, 16> &m, const std::array<T, 4> &u)
{
    v[0] = m[0] * u[0] + m[4] * u[1] + m[ 8] * u[2] + m[12] * u[3];
    v[1] = m[1] * u[0] + m[5] * u[1] + m[ 9] * u[2] + m[13] * u[3];
    v[2] = m[2] * u[0] + m[6] * u[1] + m[10] * u[2] + m[14] * u[3];
    v[3] = m[3] * u[0] + m[7] * u[1] + m[11] * u[2] + m[15] * u[3];
}

template <typename T> inline std::array<T, 4> mul_mat_vec(const std::array<T, 16> &m, const std::array<T, 4> &u)
{
    std::array<T, 4> v;
    mul_mat_vec(v, m, u);
    return v;
}

template <typename T> inline void mul_mat_vec_transpose(std::array<T, 4> &v, const std::array<T, 16> &m, const std::array<T, 4> &u)
{
    v[0] = m[ 0] * u[0] + m[ 1] * u[1] + m[ 2] * u[2] + m[ 3] * u[3];
    v[1] = m[ 4] * u[0] + m[ 5] * u[1] + m[ 6] * u[2] + m[ 7] * u[3];
    v[2] = m[ 8] * u[0] + m[ 9] * u[1] + m[10] * u[2] + m[11] * u[3];
    v[3] = m[12] * u[0] + m[13] * u[1] + m[14] * u[2] + m[15] * u[3];
}

template <typename T> inline std::array<T, 4> mul_mat_vec_transpose(const std::array<T, 16> &m, const std::array<T, 4> &u)
{
    std::array<T, 4> v;
    mul_mat_vec_transpose(v, m, u);
    return v;
}

template <typename T> inline void mul_mat_vec_wn(std::array<T, 3> &v, const std::array<T, 16> &m, const std::array<T, 3> &u, float w)
{
    v[0] = m[0] * u[0] + m[4] * u[1] + m[ 8] * u[2] + m[12] * w;
    v[1] = m[1] * u[0] + m[5] * u[1] + m[ 9] * u[2] + m[13] * w;
    v[2] = m[2] * u[0] + m[6] * u[1] + m[10] * u[2] + m[14] * w;
}

template <typename T> inline std::array<T, 3> mul_mat_vec_wn(const std::array<T, 16> &m, const std::array<T, 3> &u, float w)
{
    std::array<T, 3> v;
    mul_mat_vec_wn(v, m, u);
    return v;
}

template <typename T> inline void mul_mat_vec_w1(std::array<T, 3> &v, const std::array<T, 16> &m, const std::array<T, 3> &u)
{
    v[0] = m[0] * u[0] + m[4] * u[1] + m[ 8] * u[2] + m[12];
    v[1] = m[1] * u[0] + m[5] * u[1] + m[ 9] * u[2] + m[13];
    v[2] = m[2] * u[0] + m[6] * u[1] + m[10] * u[2] + m[14];
}

template <typename T> inline std::array<T, 3> mul_mat_vec_w1(const std::array<T, 16> &m, const std::array<T, 3> &u)
{
    std::array<T, 3> v;
    mul_mat_vec_w1(v, m, u);
    return v;
}

template <typename T> inline void mul_mat_vec_w0(std::array<T, 3> &v, const std::array<T, 16> &m, const std::array<T, 3> &u)
{
    v[0] = m[0] * u[0] + m[4] * u[1] + m[ 8] * u[2];
    v[1] = m[1] * u[0] + m[5] * u[1] + m[ 9] * u[2];
    v[2] = m[2] * u[0] + m[6] * u[1] + m[10] * u[2];
}

template <typename T> inline std::array<T, 3> mul_mat_vec_w0(const std::array<T, 16> &m, const std::array<T, 3> &u)
{
    std::array<T, 3> v;
    mul_mat_vec_w0(v, m, u);
    return v;
}

template <typename T> inline void mul_mat_vec_w1_transpose(std::array<T, 3> &v, const std::array<T, 16> &m, const std::array<T, 3> &u)
{
    v[0] = m[0] * u[0] + m[1] * u[1] + m[ 2] * u[2] + m[ 3];
    v[1] = m[4] * u[0] + m[5] * u[1] + m[ 6] * u[2] + m[ 7];
    v[2] = m[8] * u[0] + m[9] * u[1] + m[10] * u[2] + m[11];
}

template <typename T> inline std::array<T, 3> mul_mat_vec_w1_transpose(const std::array<T, 16> &m, const std::array<T, 3> &u)
{
    std::array<T, 3> v;
    mul_mat_vec_w1_transpose(v, m, u);
    return v;
}

template <typename T> inline void mul_mat_vec_wn_transpose(std::array<T, 3> &v, const std::array<T, 16> &m, const std::array<T, 3> &u, float w)
{
    v[0] = m[0] * u[0] + m[1] * u[1] + m[ 2] * u[2] + m[ 3] * w;
    v[1] = m[4] * u[0] + m[5] * u[1] + m[ 6] * u[2] + m[ 7] * w;
    v[2] = m[8] * u[0] + m[9] * u[1] + m[10] * u[2] + m[11] * w;
}

template <typename T> inline std::array<T, 3> mul_mat_vec_wn_transpose(const std::array<T, 16> &m, const std::array<T, 3> &u)
{
    std::array<T, 3> v;
    mul_mat_vec_wn_transpose(v, m, u);
    return v;
}

template <typename T> inline void mul_mat_vec_w0_transpose(std::array<T, 3> &v, const std::array<T, 16> &m, const std::array<T, 3> &u)
{
    v[0] = m[0] * u[0] + m[1] * u[1] + m[ 2] * u[2];
    v[1] = m[4] * u[0] + m[5] * u[1] + m[ 6] * u[2];
    v[2] = m[8] * u[0] + m[9] * u[1] + m[10] * u[2];
}

template <typename T> inline std::array<T, 3> mul_mat_vec_w0_transpose(const std::array<T, 16> &m, const std::array<T, 3> &u)
{
    std::array<T, 3> v;
    mul_mat_vec_w0_transpose(v, m, u);
    return v;
}

template <typename T> inline void mul_mat_transpose(std::array<T, 16> &m, const std::array<T, 16> &b, const std::array<T, 16> &a)
{
    m[ 0] = b[ 0] * a[0] + b[ 1] * a[4] + b[ 2] * a[ 8] + b[ 3] * a[12];
    m[ 4] = b[ 0] * a[1] + b[ 1] * a[5] + b[ 2] * a[ 9] + b[ 3] * a[13];
    m[ 8] = b[ 0] * a[2] + b[ 1] * a[6] + b[ 2] * a[10] + b[ 3] * a[14];
    m[12] = b[ 0] * a[3] + b[ 1] * a[7] + b[ 2] * a[11] + b[ 3] * a[15];
    m[ 1] = b[ 4] * a[0] + b[ 5] * a[4] + b[ 6] * a[ 8] + b[ 7] * a[12];
    m[ 5] = b[ 4] * a[1] + b[ 5] * a[5] + b[ 6] * a[ 9] + b[ 7] * a[13];
    m[ 9] = b[ 4] * a[2] + b[ 5] * a[6] + b[ 6] * a[10] + b[ 7] * a[14];
    m[13] = b[ 4] * a[3] + b[ 5] * a[7] + b[ 6] * a[11] + b[ 7] * a[15];
    m[ 2] = b[ 8] * a[0] + b[ 9] * a[4] + b[10] * a[ 8] + b[11] * a[12];
    m[ 6] = b[ 8] * a[1] + b[ 9] * a[5] + b[10] * a[ 9] + b[11] * a[13];
    m[10] = b[ 8] * a[2] + b[ 9] * a[6] + b[10] * a[10] + b[11] * a[14];
    m[14] = b[ 8] * a[3] + b[ 9] * a[7] + b[10] * a[11] + b[11] * a[15];
    m[ 3] = b[12] * a[0] + b[13] * a[4] + b[14] * a[ 8] + b[15] * a[12];
    m[ 7] = b[12] * a[1] + b[13] * a[5] + b[14] * a[ 9] + b[15] * a[13];
    m[11] = b[12] * a[2] + b[13] * a[6] + b[14] * a[10] + b[15] * a[14];
    m[15] = b[12] * a[3] + b[13] * a[7] + b[14] * a[11] + b[15] * a[15];
}

template <typename T> inline void mul_mat_transpose(const std::array<T, 16> &b, const std::array<T, 16> &a)
{
    std::array<T, 16> m;
    mul_mat_transpose(m, b, a);
    return m;
}

template <typename T> inline void mul_mat(std::array<T, 16> &m, const std::array<T, 16> &a, const std::array<T, 16> &b)
{
    m[ 0] = a[ 0] * b[0] + a[ 1] * b[4] + a[ 2] * b[ 8] + a[ 3] * b[12];
    m[ 1] = a[ 0] * b[1] + a[ 1] * b[5] + a[ 2] * b[ 9] + a[ 3] * b[13];
    m[ 2] = a[ 0] * b[2] + a[ 1] * b[6] + a[ 2] * b[10] + a[ 3] * b[14];
    m[ 3] = a[ 0] * b[3] + a[ 1] * b[7] + a[ 2] * b[11] + a[ 3] * b[15];
    m[ 4] = a[ 4] * b[0] + a[ 5] * b[4] + a[ 6] * b[ 8] + a[ 7] * b[12];
    m[ 5] = a[ 4] * b[1] + a[ 5] * b[5] + a[ 6] * b[ 9] + a[ 7] * b[13];
    m[ 6] = a[ 4] * b[2] + a[ 5] * b[6] + a[ 6] * b[10] + a[ 7] * b[14];
    m[ 7] = a[ 4] * b[3] + a[ 5] * b[7] + a[ 6] * b[11] + a[ 7] * b[15];
    m[ 8] = a[ 8] * b[0] + a[ 9] * b[4] + a[10] * b[ 8] + a[11] * b[12];
    m[ 9] = a[ 8] * b[1] + a[ 9] * b[5] + a[10] * b[ 9] + a[11] * b[13];
    m[10] = a[ 8] * b[2] + a[ 9] * b[6] + a[10] * b[10] + a[11] * b[14];
    m[11] = a[ 8] * b[3] + a[ 9] * b[7] + a[10] * b[11] + a[11] * b[15];
    m[12] = a[12] * b[0] + a[13] * b[4] + a[14] * b[ 8] + a[15] * b[12];
    m[13] = a[12] * b[1] + a[13] * b[5] + a[14] * b[ 9] + a[15] * b[13];
    m[14] = a[12] * b[2] + a[13] * b[6] + a[14] * b[10] + a[15] * b[14];
    m[15] = a[12] * b[3] + a[13] * b[7] + a[14] * b[11] + a[15] * b[15];
}

template <typename T> inline std::array<T, 16> mul_mat(const std::array<T, 16> &a, const std::array<T, 16> &b)
{
    std::array<T, 16> m;
    mul_mat(m, a, b);
    return m;
}

template <typename T> inline std::array<T, 16> mat(
    const std::array<T, 4> &a,
    const std::array<T, 4> &b,
    const std::array<T, 4> &c,
    const std::array<T, 4> &d
) {
    std::array<T, 16> m;
    m[ 0] = a[0]; m[ 1] = a[1]; m[ 2] = a[2]; m[ 3] = a[3];
    m[ 4] = b[0]; m[ 5] = b[1]; m[ 6] = b[2]; m[ 7] = b[3];
    m[ 8] = c[0]; m[ 9] = c[1]; m[10] = c[2]; m[11] = c[3];
    m[12] = d[0]; m[13] = d[1]; m[14] = d[2]; m[15] = d[3];
    return m;
}

template <typename T> inline std::array<T, 16> mat_transpose(
    const std::array<T, 4> &a,
    const std::array<T, 4> &b,
    const std::array<T, 4> &c,
    const std::array<T, 4> &d
) {
    std::array<T, 16> m;
    m[ 0] = a[0]; m[ 1] = b[0]; m[ 2] = c[0]; m[ 3] = d[0];
    m[ 4] = a[1]; m[ 5] = b[1]; m[ 6] = c[1]; m[ 7] = d[1];
    m[ 8] = a[2]; m[ 9] = b[2]; m[10] = c[2]; m[11] = d[2];
    m[12] = a[3]; m[13] = b[3]; m[14] = c[3]; m[15] = d[3];
    return m;
}

template <typename T> inline std::array<T, 9> mat(
    const std::array<T, 3> &a,
    const std::array<T, 3> &b,
    const std::array<T, 3> &c
) {
    std::array<T, 9> m;
    m[0] = a[0]; m[1] = a[1]; m[2] = a[2];
    m[3] = b[0]; m[4] = b[1]; m[5] = b[2];
    m[6] = c[0]; m[7] = c[1]; m[8] = c[2];
    return m;
}

template <typename T> inline std::array<T, 9> mat_transpose(
    const std::array<T, 3> &a,
    const std::array<T, 3> &b,
    const std::array<T, 3> &c
) {
    std::array<T, 9> m;
    m[0] = a[0]; m[1] = b[0]; m[2] = c[0];
    m[3] = a[1]; m[4] = b[1]; m[5] = c[1];
    m[6] = a[2]; m[7] = b[2]; m[8] = c[2];
    return m;
}

inline mat4_t mat_perspective(float w, float h, float znear, float zfar, bool left = true) // xna
{
    // d3d
    // left
    // 2*zn/w  0       0              0
    // 0       2*zn/h  0              0
    // 0       0       zf/(zn-zf)    -1
    // 0       0       zn*zf/(zn-zf)  0

    // right
    // 2*zn/w  0       0              0
    // 0       2*zn/h  0              0
    // 0       0       zf/(zf-zn)     1
    // 0       0       zn*zf/(zn-zf)  0

    float zvol = zfar / (znear - zfar), factor = left ? -1.f : 1.f;
    mat4_t m = {
        w,  0,   0,             0,
        0,  h,   0,             0,
        0,  0,   zvol * factor, factor,
        0,  0,  -znear * zvol,  0
    };
    return m;
}

inline mat4_t mat_perspective_fov(float fov, float aspect, float znear, float zfar, bool left = true) // xna
{
    float sine = (float)sinf(fov * 0.5f);
    if (aspect == 0.f || sine == 0.f || zfar - znear == 0.f) {
        printf("Invalid perspective fov specified!\n");
        mat4_t m;
        set_identity(m);
        return m;
    }
    float h = (float)cosf(fov * 0.5f) / sine;
    return mat_perspective(h / aspect, h, znear, zfar, left);
}

inline mat4_t mat_look_at(const vec3_t &eye, const vec3_t &at, const vec3_t &up, bool left = true)
{
    vec3_t zaxis = left ? normal(at - eye) : normal(eye - at);
    vec3_t xaxis = normal(cross(up, zaxis));
    vec3_t yaxis = cross(zaxis, xaxis);
    
    return mat_transpose(
        v4(xaxis, -dot(xaxis, eye)),
        v4(yaxis, -dot(yaxis, eye)),
        v4(zaxis, -dot(zaxis, eye)),
        v4(0.f, 0.f, 0.f, 1.f)
    );

    // xaxis.x           yaxis.x           zaxis.x           0
    // xaxis.y           yaxis.y           zaxis.y           0
    // xaxis.z           yaxis.z           zaxis.z           0
    // -dot(xaxis, eye)  -dot(yaxis, eye)  -dot(zaxis, eye)  1
}
/*

mat4_t mat_look_at_rh_fov(float fovy, float aspect, float zNear, float zFar) // gl
{
    float radians = fovy * 0.5f * Camera::PI / 180.f;
    float deltaZ = zFar - zNear, sine = sinf(radians);
    if (deltaZ == 0.f || sine == 0.f || aspect == 0.f) {
        printf("Invalid perpective fov specified!\n");
        mat4_t m;
        set_identity(m);
        return m;
    }

    float cotangent = cosf(radians) / sine;
    mat4_t m = {
        cotangent / aspect, 0.f,        0.f,                          0.f,
        0.f,                cotangent,  0.f,                          0.f,
        0.f,                0.f,       -(zFar + zNear) / deltaZ,     -1.f,
        0.f,                0.f,       -2.f * zNear * zFar / deltaZ,  0.f
    };
    return m;
}
*/
