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

//template <typename VertexType, typename PositionType = >
//inline void add_vertex(const Tuple3f &p, const Tuple2f &uv) { Vertex v; set_position(v, p); set_texture_coors(v, uv); add_vertex(v); }

template <typename VertexType, typename IndexType = uint16_t>
struct DynamicBuffer
{
    typedef std::array<uint32_t, 2>   array_u32_t;
    typedef std::array<VertexType, 4> quad_vertices_t;

    inline DynamicBuffer() : _reserves({ 0 }), _buffer(options<IndexType>(VertexType(), GL_TRIANGLES)) {}

    inline VertexType       *vertex_data()                                    { return _vertices.data(); }
    inline const VertexType *vertex_data()                              const { return _vertices.data(); }
    inline size_t            vertex_count()                             const { return _vertices.size(); }
    inline size_t            vertex_bytes_size()                        const { return _vertices.size() * sizeof(VertexType); }

    inline IndexType       *index_data()                                      { return _indices.data(); }
    inline const IndexType *index_data()                                const { return _indices.data(); }
    inline size_t           index_count()                               const { return _indices.size(); }
    inline size_t           index_bytes_size()                          const { return _indices.size() * sizeof(IndexType); }

    inline void             clear()                                           { _vertices.clear(); _indices.clear(); }
    inline void             reserve(size_t vcount, size_t icount)             { _reserves = array_u32_t { uint32_t(vcount), uint32_t(icount) }; _invalidate(); }
    
    inline void             add_vertex(const VertexType &v)                   { _invalidate(); _vertices.push_back(v); }
    inline void             add_vertices(std::span<VertexType> s)             { _invalidate(); append(s, _vertices); }
    inline void             add_vertices(std::span<const VertexType> s)       { _invalidate(); append(s, _vertices); }
    inline void             add_index(IndexType i)                            { _invalidate(); _indices.push_back(i); }
    inline void             add_indices(std::span<IndexType> s)               { _invalidate(); append(s, _indices); }
    inline void             add_indices(std::span<const IndexType> s)         { _invalidate(); append(s, _indices); }
    inline void             add_last_index()                                  { if (!_vertices.empty()) add_index(_vertices.size() - 1); }

    inline void             add_indexed_quad(const quad_vertices_t &positions)
    {
        size_t last = _vertices.size();
        static constexpr std::array<IndexType, 6> indices = { 0, 1, 2, 2, 1, 3 };
        for (const auto &p : positions) { _vertices.push_back(p); }
        for (auto i : indices) { _indices.push_back(i + last); }
        _invalidate();
    }
    
    void render(RenderContext &context)
    {
        if (!_vertices.empty() && !_indices.empty()) {
            _buffer.set(vertex_data(), vertex_bytes_size(), index_data(), index_bytes_size());
            _buffer.render(context);
        }
    }
    
    std::ostream &print(std::ostream &os, size_t count = 0)
    {
        os << "vertices ";
        for (size_t i = 0; i < count; i++) { os << " [" << _vertices[i] << "]" << std::endl; }
        os << "indices ";
        for (size_t i = 0; i < count; i++) { os << " " << _indices[i] << std::endl; }
        return os;
    }

private:

    Buffer                  _buffer;
    std::vector<VertexType> _vertices;
    std::vector<IndexType>  _indices;
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
