#pragma once

#include "core/core.h"
#include "tuple2f.h"
#include "tuple3f.h"
#include "tuple4f.h"
#include <span>

class QuadListAttributes
{
public:

    typedef QuadListAttributes self_t;
    
    enum BlendType { BLEND_LIGHT = 0, BLEND_DARK = 1 };
    
    inline QuadListAttributes() : _hash(~0ull), _texture_id(0), _program_id(1), _color(1.f, 1.f, 1.f, 1.f), _emissive(0.f, 0.f, 0.f, 0.f), _average_distance(0.f), _blend_type(BLEND_DARK), _order(0) {}
    
   
    inline int       texture_id()                       const { return _texture_id; }
    inline int       program_id()                       const { return _program_id; }
    inline Tuple4f   color()                            const { return _color; }
    inline Tuple4f   emissive()                         const { return _emissive; }
    inline float     average_distance()                 const { return _average_distance; }
    inline BlendType blend_type()                       const { return _blend_type; }
    inline int       order()                            const { return _order; }

    inline float     alpha()                            const { return _color.w; }
    inline bool      renderable()                       const { return _color.w > 0.01f && _texture_id && _program_id; }
    
    inline size_t    hash()                             const { return _hash == ~0ull ? _rehash() : _hash; }
    
    inline self_t   &set_texture_id(int value)                { _hash = ~0ull; _texture_id = value; return *this; }
    inline self_t   &set_program_id(int value)                { _hash = ~0ull; _program_id = value; return *this; }
    inline self_t   &set_color(const Tuple4f &value)          { _hash = ~0ull; _color = value; return *this; }
    inline self_t   &set_emissive(const Tuple4f &value)       { _hash = ~0ull; _emissive = value; return *this; }
    inline self_t   &set_average_distance(float value)        { _hash = ~0ull; _average_distance = value; return *this; }
    inline self_t   &set_blend_type(BlendType value)          { _hash = ~0ull; _blend_type = value; return *this; }
    inline self_t   &set_order(int value)                     { _hash = ~0ull; _order = value; return *this; }
   
    inline bool operator==(const QuadListAttributes &a) const
    {
        return _texture_id == a._texture_id && _program_id == a._program_id && _color == a._color && _emissive == a._emissive &&
               _blend_type == a._blend_type && _average_distance == a._average_distance && _order == a._order;
    }
    
    inline bool operator<(const QuadListAttributes &a) const
    {
        if (_program_id != a._program_id) return _program_id < a._program_id;
        if (_texture_id != a._texture_id) return _texture_id < a._texture_id;
        if (_color      != a._color)      return _color < a._color;
        if (_emissive   != a._emissive)   return _emissive < a._emissive;
        if (_blend_type != a._blend_type) return _blend_type < a._blend_type;
        if (_order      != a._order)      return _order < a._order;
        return _average_distance < a._average_distance;
    }
    
    inline std::ostream &print(std::ostream &os) const
    {
        return os << "attr tid " << _texture_id << " pid " << _program_id << " color " <<
               _color << " emissive " << _emissive << " av dist " <<
               _average_distance << " blend_type " << _blend_type << " order " << _order << " hash " << hash();
    }

private:

    int       _texture_id;
    int       _program_id;
    Tuple4f   _color;
    Tuple4f   _emissive;
    float     _average_distance;
    BlendType _blend_type;
    int       _order;

    mutable size_t _hash = ~0ull;
    
    inline size_t _rehash() const
    {
        static std::hash<float> hf;
        static std::hash<uint64_t> hu;
        uint64_t value = (_order << 24) | (_program_id << 16) | (_texture_id << 8) | int(_blend_type);
        size_t h = 0, values[] = { hu(value), _color.hash(), _emissive.hash(), hf(_average_distance) };
        for (size_t v : std::span<size_t>(values)) { h <<= 2; h ^= v; }
        return _hash = h;
    }
    
};

inline std::ostream &operator<<(std::ostream &os, const QuadListAttributes &ia) { return ia.print(os); }

template <typename ValueHashMember>
struct hasher_t
{
  inline size_t operator()(const ValueHashMember &value) const { return value.hash(); }
};
