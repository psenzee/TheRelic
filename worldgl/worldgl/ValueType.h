#pragma once

#include <string>
#include <string_view>
#include <algorithm>
#include <utility>
#include <array>
#include <iostream>
#include <span>

struct ValueType
{
    enum { _8 = 1, _16 = 2, _32 = 4, _64 = 8, _SIZE_MASK = 15, _FLOAT = 1 << 4,
           _SINT = 2 << 4, _UINT = 4 << 4, _PACKED_NORM = 8 << 4 };

    enum Type : uint8_t {
        NONE    = 0,
        FLOAT64 = _FLOAT | _64, FLOAT32 = _FLOAT | _32, FLOAT16 = _FLOAT | _16, FLOAT8 = _FLOAT | _8,
        SINT64  = _SINT  | _64, SINT32  = _SINT  | _32, SINT16  = _SINT  | _16, SINT8  = _SINT  | _8,
        UINT64  = _UINT  | _64, UINT32  = _UINT  | _32, UINT16  = _UINT  | _16, UINT8  = _UINT  | _8,
        N2_10_10_10 = _PACKED_NORM | _32,
        ERROR = 0xff
    };
    
    inline static size_t size(Type t)      { return (size_t(t) &  _SIZE_MASK); }
    inline static size_t is_none(Type t)   { return (size_t(t) &  _SIZE_MASK) == 0; }
    inline static size_t is_float(Type t)  { return (size_t(t) &  _FLOAT) != 0; }
    inline static size_t is_int(Type t)    { return (size_t(t) & (_SINT | _UINT)) != 0; }
    inline static size_t is_uint(Type t)   { return (size_t(t) &  _UINT) != 0; }
    inline static size_t is_sint(Type t)   { return (size_t(t) &  _SINT) != 0; }
    inline static size_t is_packed(Type t) { return (size_t(t) &  _PACKED_NORM) != 0; }
    
    typedef typename std::string_view::const_iterator sv_iter_t;
    
    static Type        parse(sv_iter_t &b, sv_iter_t e);
    static const char *name(Type t);

private:

    static uint8_t     parseSize_8_16_32_64(sv_iter_t &b, sv_iter_t e);
};

int test_value_type();
