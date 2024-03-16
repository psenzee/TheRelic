#pragma once

#include <string_view>
#include <algorithm>
#include <iostream>

#include "ValueType.h"

class VertexFormat;
class Element;

class VertexFormatGL
{
public:

    inline        VertexFormatGL(const VertexFormat &format) : _format(format) {}
    
    void          layOutArray() const;
    void          setProgram(int programId) const;
    void          bindAttribLocations(int programId) const;
    
    static void   bindAllAttribLocations(int programId);
    
private:

    const VertexFormat &_format;

    void          _layOutArrayOneElement(unsigned index, const Element &elem) const;
    void          _setProgramOneElement(int programId, unsigned index, const Element &elem) const;
    void          _bindAttribLocationOneElement(int programId, unsigned index, const Element &elem) const;
    const char   *_typeName(ValueType::Type type)  const;
    int           _typeValue(ValueType::Type type) const;
};

int test_vertex_format_gl();
