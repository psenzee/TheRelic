#pragma once

#include <array>
#include <span>
#include <cstddef>

struct VertexOptions
{
    enum Index { POSITION = 0, NORMAL = 1, COLOR = 2, TEXTURE_0 = 3, TEXTURE_1 = 4, TEXTURE_2 = 5, TEXTURE_3 = 6 /* ... */, INDEX_COUNT };

    static constexpr const uint8_t INVALID = 255;

    uint8_t                          stride;
    uint8_t                          sizeof_index;
    std::array<uint8_t, INDEX_COUNT> offsets;
    int                              triangle_mode;
    
    inline VertexOptions(int triangle_mode = 0, uint8_t sizeof_index = 0): stride(0), sizeof_index(sizeof_index), offsets({ INVALID }), triangle_mode(triangle_mode) {}
    
    inline bool        has_offset_pointer(Index index) const { return offsets[index] != INVALID; }
    inline const void *offset_pointer(Index index)     const { return (const void *)(((const char *)nullptr) + offsets[index]); }
    
    inline VertexOptions &set_triangle_mode(int mode)             { triangle_mode = mode; return *this; }
    inline VertexOptions &set_stride(uint8_t size)                { stride = size; return *this; }
    inline VertexOptions &set_sizeof_index(uint8_t size)          { sizeof_index = size; return *this; }
    inline VertexOptions &set_offset(Index index, uint8_t offset) { offsets[index] = offset; return *this; }
};
