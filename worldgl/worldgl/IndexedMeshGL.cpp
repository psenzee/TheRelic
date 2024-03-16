#include <iostream>
#include <thread>
#include <cstdio>

#include "gl_utils.h"
#include "IndexedMeshGL.h"
#include "VertexFormat.h"
#include "VertexFormatGL.h"

IndexedMeshGL::IndexedMeshGL(
    std::string_view format,
    const void *vertices, size_t vertex_count,
    const uint16_t *indexes, size_t index_count
) {
    VertexFormat *vf = new VertexFormat;
    vf->parse(format);
    _format = vf;
    
    _GL(glGenVertexArrays(1, &_vertex_array_object));
    _GL(glBindVertexArray(_vertex_array_object));

    _GL(glGenBuffers(1, &_vertex_buffer_object));
    _GL(glBindBuffer(GL_ARRAY_BUFFER, _vertex_buffer_object));
    _GL(glBufferData(GL_ARRAY_BUFFER, vertex_count * sizeof(float), vertices, GL_STATIC_DRAW));
    VertexFormatGL vfgl(*_format);
    vfgl.layOutArray();
    _GL(glBindBuffer(GL_ARRAY_BUFFER, 0));

    _GL(glGenBuffers(1, &_index_buffer_object));
    _GL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _index_buffer_object));
    _GL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_count * sizeof(uint16_t), indexes, GL_STATIC_DRAW));
}

void IndexedMeshGL::enable(unsigned shader_id)
{
    //_GL(glBindBuffer(GL_ARRAY_BUFFER, _vertex_buffer_object));
    //_GL(glBindBuffer(GL_ARRAY_BUFFER, 0));
}

IndexedMeshGL::~IndexedMeshGL()
{
    _GL(glDeleteBuffers(1, &_index_buffer_object));
    _GL(glDeleteBuffers(1, &_vertex_array_object));
    _GL(glDeleteVertexArrays(1, &_vertex_buffer_object));
    _index_buffer_object = _vertex_array_object = _vertex_buffer_object = 0;
    delete _format;
    _format = nullptr;
}
