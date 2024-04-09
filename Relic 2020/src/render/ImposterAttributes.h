#pragma once

#include "core/core.h"
#include "tuple2f.h"
#include "tuple3f.h"
#include "tuple4f.h"
#include <span>

struct ImposterAttributes
{
    enum BlendType { BLEND_LIGHT = 0, BLEND_DARK = 1 };

    int       texture_id;
    int       program_id;
    Tuple4f   color;
    Tuple4f   emissive;
    float     average_distance;
    BlendType blend_type;
    
    inline size_t hash() const
    {
        static std::hash<float> hf;
        static std::hash<int> hi;
        size_t h = 0, values[] = { hi(program_id), hi(texture_id), color.hash(), emissive.hash(), hi(int(blend_type)), hf(average_distance) };
        for (size_t v : std::span<size_t>(values)) { h <<= 2; h ^= v; }
        return h;
    }
    
    inline bool renderable() const
    {
        return color.w > 0.01f && texture_id /* && program_id [TODO] */;
    }
    
    inline bool operator==(const ImposterAttributes &a) const
    {
        return texture_id == a.texture_id && program_id == a.program_id && color == a.color && emissive == a.emissive &&
               blend_type == a.blend_type && average_distance == a.average_distance;
    }
    
    inline bool operator<(const ImposterAttributes &a) const
    {
        if (program_id != a.program_id) return program_id < a.program_id;
        if (texture_id != a.texture_id) return texture_id < a.texture_id;
        if (color      != a.color)      return color < a.color;
        if (emissive   != a.emissive)   return emissive < a.emissive;
        if (blend_type != a.blend_type) return blend_type < a.blend_type;
        return average_distance < a.average_distance;
    }
};

inline std::ostream &operator<<(std::ostream &os, const ImposterAttributes &ia)
{
    return os << "tid " << ia.texture_id << " pid " << ia.program_id << " color " <<
           ia.color << " emissive " << ia.emissive << " av dist " <<
           ia.average_distance << " blend_type " << ia.blend_type << " hash " << ia.hash();
}

template <typename ValueHashMember>
struct hasher_t
{
  inline size_t operator()(const ValueHashMember &value) const { return value.hash(); }
};
