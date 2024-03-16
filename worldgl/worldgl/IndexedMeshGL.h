#pragma once

#include <cstddef>
#include <string_view>

struct VertexFormat;

struct IndexedMeshGL
{
    unsigned _vertex_array_object,
             _vertex_buffer_object,
             _index_buffer_object;
    
    const VertexFormat *_format;
    
    IndexedMeshGL(std::string_view format, const void *vertices, size_t vertex_count, const uint16_t *indexes, size_t index_count);
    
    void enable(unsigned shader_id);

    ~IndexedMeshGL();
};
