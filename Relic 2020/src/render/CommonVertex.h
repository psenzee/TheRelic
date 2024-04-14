#pragma once

#include <vector>
#include <iostream>
#include <iomanip>
#include "tuple3f.h"
#include "tuple2f.h"
#include "VertexOptions.h"

struct CommonVertex
{
    typedef Tuple3f position_type;
    typedef Tuple2f texture_coors_type;
    
    position_type      position;
    texture_coors_type texture_coors;
    
    enum { MULTITEXTURE = 0 };
    enum { OFFSET_POSITION = 0, OFFSET_TEXTURE = sizeof(Tuple3f), OFFSET_NORMAL = -1 };
};

inline std::ostream &operator<<(std::ostream &os, const CommonVertex &v)
{
    return os << "[pos " << v.position << " tex " << v.texture_coors << "]";
}

template <typename VertexType, typename PositionType = VertexType::position_type>
inline VertexType &set_position(VertexType &v, const PositionType &value)
{
    if constexpr (VertexType::OFFSET_POSITION != -1) {
        v.position = value;
    }
    return v;
}

template <typename VertexType, size_t Index = 0, typename UvType = VertexType::texture_coors_type>
inline VertexType &set_texture_coors(VertexType &v, const UvType &value)
{
    if constexpr (VertexType::OFFSET_TEXTURE != -1 && !VertexType::MULTITEXTURE && Index == 0) {
        v.texture_coors = value;
    } else if constexpr (VertexType::MULTITEXTURE) {
        v.texture_coors[Index] = value;
    }
    return v;
}

template <typename IndexType = uint16_t>
inline VertexOptions options(const CommonVertex &vertex, int triangle_mode)
{
    VertexOptions opts;
    opts.set_triangle_mode(triangle_mode);
    opts.set_stride(sizeof(vertex));
    opts.set_sizeof_index(sizeof(IndexType));
    const char *v = (const char *)&vertex;
    opts.set_offset(VertexOptions::POSITION,  uint8_t((const char *)&vertex.position - v));
    opts.set_offset(VertexOptions::TEXTURE_0, uint8_t((const char *)&vertex.texture_coors - v));
    return opts;
}
