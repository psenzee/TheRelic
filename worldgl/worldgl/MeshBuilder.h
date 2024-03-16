#include <iostream>
#include <thread>
#include <cstdio>
#include <unordered_map>
#include <span>

#include "std_array_mat.h"
#include "math_typedefs.h"
#incldue "MeshVertex.h"

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

struct VertexPT
{
    vec3_t position;
    vec2_t texture;
};

inline vec3_t       &position(VertexPT &v)       { return v.position; }
inline const vec3_t &position(const VertexPT &v) { return v.position; }
inline vec2_t       &texture0(VertexPT &v)       { return v.texture; }
inline const vec2_t &texture0(const VertexPT &v) { return v.texture; }

template <typename Vertex>
struct Triangle
{
    Vertex a, b, c;
};

template <typename Vertex>
struct MeshBuilder
{
    typedef MeshBuilder<Vertex> self_t;

    std::vector<Vertex>                  vertices;
    std::vector<uint16_t>                indices;
    std::unordered_map<Vertex, uint16_t> index_map;

    inline self_t &add(const Vertex &a)
    {
        uint16_t index = vertices.size();
        if (index_map.contains(a)) {
            index = index_map[a];
        } else {
            vertices.push_back(a);
        }
        indices.push_back(index);
        return *this;
    }

    inline self_t &add(std::span<Vertex> vertices)
    {
        for (const auto &v : vertices) {
            add(v);
        }
        return *this;
    }

    inline self_t &add(const Triangle<Vertex> &t)
    {
        return add(t.a).add(t.b).add(t.c);
    }

    inline self_t &add(const Vertex &a, const Vertex &b, const Vertex &c)
    {
        return add(a).add(b).add(c);
    }
};
