#pragma once

#include <vector>
#include <iostream>
#include <iomanip>
#include "VertexOptions.h"

class RenderContext;

struct Buffer
{
    typedef std::array<uint32_t, 2> array_u32_t;

    inline Buffer(const VertexOptions &options) : _ids({ 0 }), _options(options), _data_changed(false) {}

    inline ~Buffer() { _destroy(); }
    
    void set(const void *vertices_data, size_t vertices_bytes, const void *indices_data, size_t indices_bytes);
    
    VertexOptions &options() { return _options; }
    
    inline void invalidate() { _data_changed = true; }
    
    bool is_valid() const;
    
    inline const array_u32_t &ids() const { return _ids; }

    void render(RenderContext &context);

private:

    VertexOptions _options;
    array_u32_t   _ids;
    const void   *_vertices_data;
    size_t        _vertices_bytes;
    const void   *_indices_data;
    size_t        _indices_bytes;
    size_t        _indices_count;
    bool          _data_changed;

    void _validate_buffers();
    void _destroy();
};
