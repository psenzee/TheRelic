#pragma once

#include <string_view>
#include <algorithm>
#include <iostream>

#include "ValueType.h"

class VertexFormat;
class Element;

class VertexFormatGL
{
    const VertexFormat &_format;
    
    std::ostream &_codeLayOutArrayOneElement(std::ostream &os, size_t index, const Element &elem) const;
    std::ostream &_codeBindProgramOneElement(std::ostream &os, std::string_view programIdOrVar, size_t index, const Element &elem) const;
    
    void          _layOutArrayOneElement(size_t index, const Element &elem) const;
    void          _bindProgramOneElement(int programId, size_t index, const Element &elem) const;
    const char   *_typeName(ValueType::Type type)  const;
    int           _typeValue(ValueType::Type type) const;

public:

    inline        VertexFormatGL(const VertexFormat &format) : _format(format) {}
    
    std::ostream &codeLayOutArray(std::ostream &os) const;
    std::ostream &codeBindProgram(std::ostream &os, std::string_view programIdOrVar) const;
    
    void          layOutArray() const;
    void          bindProgram(int programId) const;
};

int test_vertex_format_gl();
