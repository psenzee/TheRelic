#include "Buffer.h"
#include <vector>
#include <iostream>
#include <iomanip>
#include "GLAbstract.h"
#include "std_utils.h"
#include "Timer.h"
#include "GraphicsDevice.h"

class RenderContext;

void Buffer::set(const void *vertices_data, size_t vertices_bytes, const void *indices_data, size_t indices_bytes)
{
    _data_changed |= (vertices_data != _vertices_data || indices_data != _indices_data || indices_bytes != _indices_bytes);
    if (_data_changed) {
        _vertices_data = vertices_data;
        _vertices_bytes = vertices_bytes;
        _indices_data = indices_data;
        _indices_bytes = indices_bytes;
        if (!_vertices_data || !_vertices_bytes || !_indices_data || !_indices_bytes) {
            _destroy();
        } else {
            _validate_buffers();
            GLCopyIntoBuffer(_ids[0], GL_ARRAY_BUFFER, _vertices_data, _vertices_bytes, GL_DYNAMIC_DRAW);
            GLCopyIntoBuffer(_ids[1], GL_ELEMENT_ARRAY_BUFFER, _indices_data, _indices_bytes, GL_DYNAMIC_DRAW);
        }
        _data_changed = false;
    }
}

bool Buffer::is_valid() const
{
    return (_ids[0] && _ids[1] && _vertices_data && _vertices_bytes && _indices_data && _indices_bytes);
}

void Buffer::render(RenderContext &context)
{
    typedef std::pair<int, VertexOptions::Index> pair_t;
    if (is_valid()) {
        GLBindBufferForElements(_ids[0], _ids[1]);
        std::array<pair_t, 3> elems = {
            pair_t { GL_VERTEX_ARRAY,        VertexOptions::POSITION },
            pair_t { GL_TEXTURE_COORD_ARRAY, VertexOptions::TEXTURE_0 },
            pair_t { GL_NORMAL_ARRAY,        VertexOptions::NORMAL }
        };
        for (auto p : elems) {
            GLSetEnabledClientState(p.first, _options.has_offset_pointer(p.second));
        }
        GLConfigureVertexInterleavedOptions(_options);
        GLDrawElements(_options.triangle_mode, _indices_bytes / _options.sizeof_index, _options.sizeof_index);
        GLBindBufferForElements(0, 0);
    }
}

void Buffer::_validate_buffers()
{
    if (!_ids[0]) {
        _ids = GLGenerateBuffers(true);
    }
}

void Buffer::_destroy()
{
    _vertices_data = nullptr;
    _vertices_bytes = 0;
    _indices_data = nullptr;
    _indices_bytes = 0;
    _indices_count = 0;
    if (_ids[0]) {
        GLDestroyBuffers(_ids);
    }
    _ids = { 0 };
}
