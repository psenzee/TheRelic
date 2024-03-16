#pragma once

#include "std_array_mat.h"
#include "math_typedefs.h"

/*

 const GLfloat vertices [] = {
      1.f, -1.f, 0.0f,  1.f, 0.f,
      1.f,  1.f, 0.0f,  1.f, 1.f,
     -1.f, -1.f, 0.0f,  0.f, 0.f,
     -1.f,  1.f, 0.0f,  0.f, 1.f,
 };

 const uint16_t indices [] = {
     0, 1, 2, 2, 1, 3
 };
 
 */

struct MeshVertexPT
{
    vec3_t position;
    vec2_t texture;
};

inline vec3_t       &position(MeshVertexPT &v)       { return v.position; }
inline const vec3_t &position(const MeshVertexPT &v) { return v.position; }
inline vec2_t       &texture0(MeshVertexPT &v)       { return v.texture; }
inline const vec2_t &texture0(const MeshVertexPT &v) { return v.texture; }

template <typename Vertex>
struct Triangle
{
    Vertex a, b, c;
};
