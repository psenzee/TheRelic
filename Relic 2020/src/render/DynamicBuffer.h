#pragma once

#include <vector>
#include <iostream>
#include <iomanip>
#include "GLAbstract.h"
#include "std_utils.h"
#include "Timer.h"
#include "GraphicsDevice.h"
#include "Buffer.h"
#include "CommonVertex.h"

class RenderContext;

template <typename Vertex, typename Index = uint16_t>
struct DynamicBuffer
{
    typedef std::array<uint32_t, 2> array_u32_t;

    inline DynamicBuffer() : _reserves({ 0 }), _buffer(options<Index>(Vertex(), GL_TRIANGLES)) {}

    inline Vertex       *vertex_data()                            { return _vertices.data(); }
    inline const Vertex *vertex_data()                      const { return _vertices.data(); }
    inline size_t        vertex_count()                     const { return _vertices.size(); }
    inline size_t        vertex_bytes_size()                const { return _vertices.size() * sizeof(Vertex); }

    inline Index        *index_data()                             { return _indices.data(); }
    inline const Index  *index_data()                       const { return _indices.data(); }
    inline size_t        index_count()                      const { return _indices.size(); }
    inline size_t        index_bytes_size()                 const { return _indices.size() * sizeof(Index); }
    
    inline void          clear()                                  { _vertices.clear(); _indices.clear(); }
    inline void          reserve(size_t vcount, size_t icount)    { _reserves = array_u32_t { uint32_t(vcount), uint32_t(icount) }; _invalidate(); }
    
    inline void          add_vertex(const Vertex &v)              { _invalidate(); _vertices.push_back(v); }
    inline void          add_vertices(std::span<Vertex> s)        { _invalidate(); append(s, _vertices); }
    inline void          add_vertices(std::span<const Vertex> s)  { _invalidate(); append(s, _vertices); }
    inline void          add_index(uint16_t i)                    { _invalidate(); _indices.push_back(i); }
    inline void          add_indices(std::span<uint16_t> s)       { _invalidate(); append(s, _indices); }
    inline void          add_indices(std::span<const uint16_t> s) { _invalidate(); append(s, _indices); }
    inline void          add_last_index()                         { if (!_vertices.empty()) add_index(_vertices.size() - 1); }
    
    void render(RenderContext &context)
    {
        if (!_vertices.empty() && !_indices.empty()) {
            _buffer.set(vertex_data(), vertex_bytes_size(), index_data(), index_bytes_size());
            _buffer.render(context);
        }
    }
    
    std::ostream &print(std::ostream &os, size_t count = 0)
    {
        for (size_t i = 0; i < count; i++) {
            os << "vertex " << _vertices[i] << std::endl;
        }
        for (size_t i = 0; i < count; i++) {
            os << "index " << _indices[i] << std::endl;
        }
        return os;
    }

private:

    Buffer                  _buffer;
    std::vector<Vertex>     _vertices;
    std::vector<uint16_t>   _indices;
    array_u32_t             _reserves;
    
    inline void _invalidate()
    {
        _buffer.invalidate();
        if (_reserves[0] > _vertices.size()) {
            _vertices.reserve(_reserves[0]);
        }
        if (_reserves[1] > _indices.size()) {
            _indices.reserve(_reserves[1]);
        }
    }
};
