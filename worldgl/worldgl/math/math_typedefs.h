#pragma once

#include <array>
#include <iostream>
#include <iomanip>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

typedef std::array<float, 2>     vec2_t;
typedef std::array<float, 3>     vec3_t;
typedef std::array<float, 4>     vec4_t;
typedef std::array<float, 2 * 2> mat2_t;
typedef std::array<float, 3 * 3> mat3_t;
typedef std::array<float, 4 * 4> mat4_t;

typedef std::array<int64_t,  2>  size2i_t;
typedef std::array<uint64_t, 2>  size2u_t;
typedef std::array<int64_t,  2>  point2i_t;
/*
template <typename T> constexpr A2     v2(T a, T b)                    { return A2 { a, b }; }
template <typename T> constexpr A3     v3(T a, T b, T c)               { return A3 { a, b, c }; }
template <typename T> constexpr A3     v3(const A2 &a, T b)            { return A3 { a[0], a[1], b }; }
template <typename T> constexpr A3     v3(T a, const A2 &b)            { return A3 { a, b[0], b[1] }; }
template <typename T> constexpr A4     v4(T a, T b, T c, T d)          { return A4 { a, b, c, d }; }
template <typename T> constexpr A4     v4(T a, const A3 &b)            { return A4 { a, b[0], b[1], b[2] }; }
template <typename T> constexpr A4     v4(T a, T b, const A2 &c)       { return A4 { a, b, c[0], c[1] }; }
template <typename T> constexpr A4     v4(const A2 &a, T b, T c)       { return A4 { a[0], a[1], b, c }; }
template <typename T> constexpr A4     v4(const A2 &a, const A2 &b)    { return A4 { a[0], a[1], b[0], b[1] }; }
template <typename T> constexpr A4     v4(const A3 &a, T b)            { return A4 { a[0], a[1], a[2], b }; }
*/
inline                          vec3_t v3(const glm::vec3 &v)          { return vec3_t { v[0], v[1], v[2] }; }
inline                          vec4_t v4(const glm::vec4 &v)          { return vec4_t { v[0], v[1], v[2], v[3] }; }



inline                          mat4_t m4(const glm::mat4 &gm)
{
    return *reinterpret_cast<const mat4_t *>(&gm);
    /*
    mat4_t m;
    const float *p = glm::value_ptr(gm), *e = p + 16;
    for (size_t i = 0; p != e; ++p) m[i++] = *p;
    return m;
    /*
    return mat4_t {
        m[0][0], m[0][1], m[0][2], m[0][3],
        m[1][0], m[1][1], m[1][2], m[1][3],
        m[2][0], m[2][1], m[2][2], m[2][3],
        m[3][0], m[3][1], m[3][2], m[3][3],
    };*/
}

inline                          glm::mat4 glm_m4(const mat4_t &m)
{
    
    return *reinterpret_cast<const glm::mat4 *>(&m);/*
    glm::mat4 gm;
    float *p = glm::value_ptr(gm);
    const float *e = p + 16;
    for (size_t i = 0; p != e; ++p) *p = m[i++];
    return gm;
    /*
    gm[0][0] = m[ 0]; gm[0][1] = m[ 1]; gm[0][2] = m[ 2]; gm[0][3] = m[ 3];
    gm[1][0] = m[ 4]; gm[1][1] = m[ 5]; gm[1][2] = m[ 6]; gm[1][3] = m[ 7];
    gm[2][0] = m[ 8]; gm[2][1] = m[ 9]; gm[2][2] = m[10]; gm[2][3] = m[11];
    gm[3][0] = m[12]; gm[3][1] = m[13]; gm[3][2] = m[14]; gm[3][3] = m[15];
    return gm;
     */
}

inline std::ostream &mat_print(std::ostream &os, const mat4_t &m)
{
    for (size_t i = 0; i < m.size(); i += 4) {
        os << "| ";
        for (size_t j = 0; j < 4; j++) {
            os << std::setprecision(5) << std::setw(5) << m[i + j] << " ";
        }
    }
    return os;
}

inline std::ostream &mat_print(std::ostream &os, const glm::mat4 &m)
{
    for (size_t i = 0; i < 4; i++) {
        os << "| ";
        for (size_t j = 0; j < 4; j++) {
            os << std::setprecision(5) << std::setw(5) << m[i][j] << " ";
        }
    }
    return os;
}

inline                          mat4_t mat_invert(const mat4_t &m)
{
    glm::mat4 gm = glm_m4(m);
    gm = glm::inverse(gm);
    return m4(gm);
}
