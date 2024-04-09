#pragma once

#include "core/core.h"
#include "tuple2f.h"
#include "tuple3f.h"
#include "tuple4f.h"
#include <array>

template <typename Vertex>
struct Imposter
{
    typedef std::array<Vertex,  4> vertices_t;

    vertices_t             vertices;
    float                  distance;
    Tuple3f                center;
    
    void set(const Tuple3f &center_point, const std::array<Tuple2f, 2> &texture_coors, const Tuple3f &size)
    {
        static const Tuple3f SCALE(-1.0f, -1.0f, 1.0f), // why not bake this in?
                             UL(Tuple3f(-0.5f, -0.5f, 0.0f) * SCALE), UR(Tuple3f( 0.5f, -0.5f, 0.0f) * SCALE),
                             LL(Tuple3f(-0.5f,  0.5f, 0.0f) * SCALE), LR(Tuple3f( 0.5f,  0.5f, 0.0f) * SCALE);
        center = center_point;
        const Tuple2f &uv0  = texture_coors[0],
                      &uv1  = texture_coors[1];
        std::array<Tuple3f, 4> pos { center + UL * size, center + LL * size, center + LR * size, center + UR * size };
        std::array<Tuple2f, 4> uvs { uv0, Tuple2f(uv0.x, uv1.y), uv1, Tuple2f(uv1.x, uv0.y) };
        for (size_t i = 0; i < 4; i++) {
            set_position(vertices[i], pos[i]);
            set_texture_coors(vertices[i], uvs[i]);
        }
    }
    
    inline void set(const Tuple3f &center, const std::array<Tuple2f, 2> &texture_coors, const Tuple2f &size)
    {
        set(center, texture_coors, Tuple3f(size.x, size.y, 1.f));
    }

    inline void set(const Tuple3f &center, const std::array<Tuple2f, 2> &texture_coors, float size)
    {
        set(center, texture_coors, Tuple3f(size, size, 1.f));
    }
    
    void set(const vertices_t &v)
    {
        vertices = v;
        center = _center();
    }

private:

    inline Tuple3f _center() const
    {
        Tuple3f c(0.f, 0.f, 0.f);
        for (const auto &v : vertices) {
            c += v.position;
        }
        return c * 0.25;
    }
};
