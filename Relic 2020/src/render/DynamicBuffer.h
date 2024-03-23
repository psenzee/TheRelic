#pragma once

#include <vector>
#include "GLAbstract.h"

struct CommonVertex
{
    Tuple3f position;
    Tuple2f texture;
    
    enum { OFFSET_POSITION = 0, OFFSET_TEXTURE = sizeof(Tuple3f), OFFSET_NORMAL = -1 };
};

template <typename T>
inline void overwrite_append(std::span<T> s, std::vector<T> &v, size_t at = 0)
{
    v.resize(std::max(v.size(), s.size() + at));
    std::copy(s.begin(), s.end(), v.begin() + at);
}

template <typename T>
inline void append(std::span<T> s, std::vector<T> &v)
{
    overwrite_append(s, v, v.size());
}

template <typename Vertex, typename Index = uint16_t>
struct DynamicBuffer // not index
{
    typedef std::array<uint32_t, 2> array_u32_t;

    inline DynamicBuffer() : _reserves({ 0 }), _ids({ 0 }) {}

    ~DynamicBuffer() { _destroy(); }

    inline Vertex       *vertex_data()                          { return _vertices.data(); }
    inline const Vertex *vertex_data()                    const { return _vertices.data(); }
    inline size_t        vertex_count()                   const { return _vertices.size(); }
    inline size_t        vertex_bytes_size()              const { return _vertices.size() * sizeof(Vertex); }
    
    inline void          reserve(array_u32_t reserves)          { _reserves = reserves; _reserve(); }

    inline Index        *index_data()                           { return _indices.data(); }
    inline const Index  *index_data()                     const { return _indices.data(); }
    inline size_t        index_count()                    const { return _indices.size(); }
    inline size_t        index_bytes_size()               const { return _indices.size() * sizeof(Index); }
    
    inline void          add_vertex(const Vertex &v)            { _reserve(); _vertices.push_back(v); }
    inline void          add_vertices(std::span<Vertex> s)      { _reserve(); append(s, _vertices); }
    inline void          add_index(uint16_t i)                  { _reserve(); _indices.push_back(i); }
    inline void          add_indices(std::span<uint16_t> s)     { _reserve(); append(s, _indices); }
    inline void          add_last_index()                       { if (!_vertices.empty()) add_index(_vertices.size() - 1); }

    void render(RenderContext &context)
    {
        if (!_vertices.empty()) {
            _validate_buffers();
            GLSetEnabled(GL_CULL_FACE, true);
            GLCopyIntoBuffer(_ids[0], GL_ARRAY_BUFFER, vertex_data(), vertex_bytes_size());
            GLCopyIntoBuffer(_ids[1], GL_ELEMENT_ARRAY_BUFFER, index_data(), index_bytes_size());
            GLBindBufferForElements(_ids[0], _ids[1]);
            GLSetVertexSetupInterleaved<Vertex>();
            GLDrawElements(GL_TRIANGLES, _indices.size());
        }
    }

private:

    std::vector<Vertex>     _vertices;
    std::vector<uint16_t>   _indices;
    array_u32_t             _ids;
    array_u32_t             _reserves;
    
    inline void _reserve()
    {
        if (_reserves[0]) {
            _vertices.reserve(_reserves[0]);
        }
        if (_reserves[1]) {
            _indices.reserve(_reserves[1]);
        }
    }

    inline void _validate_buffers()
    {
        if (!_ids[0]) {
            _reserve();
            _ids = GLGenerateBuffers(true);
        }
    }
    
    inline void _destroy()
    {
        if (_ids[0]) {
            GLDestroyBuffers(_ids);
        }
        _ids = { 0 };
    }
};
