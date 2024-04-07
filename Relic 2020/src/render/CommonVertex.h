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
    
    enum { OFFSET_POSITION = 0, OFFSET_TEXTURE = sizeof(Tuple3f), OFFSET_NORMAL = -1 };
};

inline std::ostream &operator<<(std::ostream &os, const CommonVertex &v)
{
    return os << "[pos " << v.position << " tex " << v.texture << "]";
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
