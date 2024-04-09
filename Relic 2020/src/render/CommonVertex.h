#pragma once

#include <vector>
#include <iostream>
#include <iomanip>
#include "tuple3f.h"
#include "tuple2f.h"
#include "VertexOptions.h"

struct CommonVertex
{
    Tuple3f position;
    Tuple2f texture;
    
    enum { MULTITEXTURE = 0 };
    enum { OFFSET_POSITION = 0, OFFSET_TEXTURE = sizeof(Tuple3f), OFFSET_NORMAL = -1 };
};

inline std::ostream &operator<<(std::ostream &os, const CommonVertex &v)
{
    return os << "[pos " << v.position << " tex " << v.texture << "]";
}

template <typename Vertex>
inline Vertex &set_position(Vertex &v, const Tuple3f &value)
{
    if constexpr (Vertex::OFFSET_POSITION != -1) {
        v.position = value;
    }
    return v;
}

template <typename Vertex, size_t Index = 0>
inline Vertex &set_texture_coors(Vertex &v, const Tuple2f &value)
{
    if constexpr (Vertex::OFFSET_TEXTURE != -1 && !Vertex::MULTITEXTURE && Index == 0) {
        v.texture = value;
    } else if constexpr (Vertex::MULTITEXTURE) {
        v.texture[Index] = value;
    }
    return v;
}

template <typename Index = uint16_t>
inline VertexOptions options(const CommonVertex &vertex, int triangle_mode)
{
    VertexOptions opts;
    opts.set_triangle_mode(triangle_mode);
    opts.set_stride(sizeof(vertex));
    opts.set_sizeof_index(sizeof(Index));
    const char *v = (const char *)&vertex;
    opts.set_offset(VertexOptions::POSITION,  uint8_t((const char *)&vertex.position - v));
    opts.set_offset(VertexOptions::TEXTURE_0, uint8_t((const char *)&vertex.texture  - v));
    return opts;
}
