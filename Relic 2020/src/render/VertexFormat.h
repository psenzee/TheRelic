#pragma once

/* VertexFormat
 
 format
 
 non case sensitve
 current:
 starts with
 
 I      - structure: Interleaved - currently the only option, required
 
 then
 
 L or S - triangle set types: List (3 vertices per) or Strips (2 initial then 1 per subsequent)
 
 the following can occur in any order:
 
 Pn     - Position - where n is 1-4.  Only 1
 Nn     - Normal - where n is 1-4.  Only 1?
 Tn     - Texture - coor where n is 1-4.  Repeatable up to the max textures
 Cn     - Color - where n is 1-4.  Repeatable number up to the max textures
 
 */

#include <string>
#include <string_view>
#include <algorithm>
#include <utility>
#include <array>
#include <iostream>
#include <span>

#include "ValueType.h"

struct VertexField
{
    enum Type : uint8_t { POSITION = 0, NORMAL, TEXTURE, COLOR, _TYPE_MAX };
    enum { MAX_ITEMS_PER_FIELD = 16 };

    inline static constexpr Type        from(size_t v)   { return Type(v >= _TYPE_MAX ? _TYPE_MAX : v); }
    inline static constexpr size_t      value(Type type) { return size_t(type); }
    inline static           const char *name(size_t v)   { return name(from(v)); }
    static                  const char *name(Type type);
    static                  const char *attribute(Type type, size_t index = 0);
};

struct Element
{
    VertexField::Type field;
    ValueType::Type   type;
    uint8_t           offset,
                      count,
                      index;
    inline Element(uint8_t offset = 0, uint8_t count = 0, VertexField::Type field = VertexField::_TYPE_MAX, uint8_t index = 0, ValueType::Type type = ValueType::FLOAT32) :
        field(field), offset(offset), count(count), index(index), type(type) {}
    inline size_t size() const { return count * ValueType::size(type); }
};

struct VertexFormat
{
    enum            { MAX_ITEMS = VertexField::MAX_ITEMS_PER_FIELD, MAX_ELEMENTS = MAX_ITEMS * size_t(VertexField::_TYPE_MAX) };

    enum Layout     { INTERLEAVED = 0, MULTIBUFFER };
    
    enum Type       { TRI_NONE = 0, TRI_LIST, TRI_STRIP };

    enum Error      {
        NO_ERROR = 0,
        ERROR_PARSE_EMPTY_TEXT,
        ERROR_PARSE_UNEXPECTED_END,
        ERROR_PARSE_LAYOUT,
        ERROR_PARSE_LAYOUT_UNSUPPORTED,
        ERROR_PARSE_TRIANGLE_TYPE,
        ERROR_PARSE_UNRECOGNIZED_FIELD_NAME,
        ERROR_PARSE_UNRECOGNIZED_TYPE_NAME,
        ERROR_PARSE_POSITION,
        ERROR_PARSE_POSITION_COUNT,
        ERROR_PARSE_POSITION_TOO_MANY,
        ERROR_PARSE_NORMAL,
        ERROR_PARSE_NORMAL_COUNT,
        ERROR_PARSE_NORMAL_TOO_MANY,
        ERROR_PARSE_COLOR,
        ERROR_PARSE_COLOR_COUNT,
        ERROR_PARSE_COLOR_TOO_MANY,
        ERROR_PARSE_TEXTURE,
        ERROR_PARSE_TEXTURE_COUNT,
        ERROR_PARSE_TEXTURE_TOO_MANY
    };

    typedef std::array<Element, MAX_ITEMS>               element_array_t;
    typedef std::string_view::const_iterator             const_iter_t;
    typedef std::array<uint8_t, VertexField::_TYPE_MAX>  field_array_t;
    typedef std::array<Element, MAX_ELEMENTS>            enumerated_elements_t;

    Layout           layout;
    Type             type;
    element_array_t  position,
                     normal,
                     colors,
                     textures;
    uint8_t          stride,
                     element_count;
    field_array_t    counts;

    Error            error;
    std::string      format, error_message;
    
    std::ostream &print(std::ostream &os) const;

    inline VertexFormat() : type(TRI_NONE), stride(0), counts{ 0 }, element_count(0), error(NO_ERROR), layout(INTERLEAVED) { _clear(); }

    bool parse(std::string_view s);
    
    void enumerate(enumerated_elements_t &elements) const;

private:
    
    typedef std::pair<Error, const_iter_t> parse_result_t;
    
    static constexpr Error _count_errors[] = {
        ERROR_PARSE_POSITION_COUNT, ERROR_PARSE_NORMAL_COUNT,
        ERROR_PARSE_TEXTURE_COUNT,  ERROR_PARSE_COLOR_COUNT
    };
    static constexpr Error _too_many_errors[] = {
        ERROR_PARSE_POSITION_TOO_MANY, ERROR_PARSE_NORMAL_TOO_MANY,
        ERROR_PARSE_TEXTURE_TOO_MANY,  ERROR_PARSE_COLOR_TOO_MANY
    };
    static constexpr uint8_t _max_items[] = { 1, 1, MAX_ITEMS, MAX_ITEMS };
    
    static const char *_error_messages(Error e);
    
    static const char *_type_name(Type t);
    
    void _clear_error();
    
    void _clear();
    
    parse_result_t _parse(std::string_view s);
    
    parse_result_t _result(Error e, std::string_view s, const_iter_t b);
};

inline std::ostream &operator<<(std::ostream &os, const VertexFormat &vf) { return vf.print(os); }
