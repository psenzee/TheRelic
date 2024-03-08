#include "VertexFormatGL.h"
#include "VertexFormat.h"
#include "ValueType.h"
#include "GLIncludes.h"

std::ostream &VertexFormatGL::_codeLayOutArrayOneElement(std::ostream &os, size_t index, const Element &elem) const
{
    bool normalized = false;
    char buffer[1024] = "";
    snprintf(buffer, sizeof(buffer) - 1, "glVertexAttribPointer(%lu, %lu, %s, %s, %lu, %lu); // %s: %lu * %s (%lu) = %lu bytes\n",
       index, size_t(elem.count), _typeName(elem.type),
       normalized ? "GL_TRUE" : "GL_FALSE", size_t(_format.stride), size_t(elem.offset),
       VertexField::attribute(elem.field, elem.index), size_t(elem.count), ValueType::name(elem.type), ValueType::size(elem.type),
       ValueType::size(elem.type) * size_t(elem.count)
    );
    return os << buffer;
}

std::ostream &VertexFormatGL::_codeBindProgramOneElement(std::ostream &os, std::string_view programIdOrVar, size_t index, const Element &elem) const
{
    char buffer[1024] = "";
    snprintf(
        buffer, sizeof(buffer) - 1, "glBindAttribLocation(%s, %lu, \"%s\");\n",
        programIdOrVar.data(), index, VertexField::attribute(elem.field, elem.index)
    );
    return os << buffer;
}

std::ostream &VertexFormatGL::codeLayOutArray(std::ostream &os) const
{
    /*
     
     Generates code like this from VertexFormat:
    
     glVertexAttribPointer(0, 3, GL_FLOAT, GLfalse, 20, 0);
     glVertexAttribPointer(1, 3, GL_UNSIGNED_BYTE, GL_TRUE, 20, 12);
     glVertexAttribPointer(3, 3, GL_UNSIGNED_SHORT, GL_TRUE, 20, 16);

    */
    typename VertexFormat::enumerated_elements_t elems;
    _format.enumerate(elems);
    size_t index = 0;
    for (const auto &elem: elems) {
        _codeLayOutArrayOneElement(os, index++, elem);
        if (index >= _format.element_count) {
            break;
        }
    }
    return os;
}

std::ostream &VertexFormatGL::codeBindProgram(std::ostream &os, std::string_view programIdOrVar) const
{
    /*

     Generates code like this from VertexFormat:
     
     glBindAttribLocation(1010, 0, "_position");
     glBindAttribLocation(1010, 1, "_texture");
     glBindAttribLocation(1010, 2, "_color");
     
    */
    typename VertexFormat::enumerated_elements_t elems;
    _format.enumerate(elems);
    size_t index = 0;
    for (const auto &elem: elems) {
        _codeBindProgramOneElement(os, programIdOrVar, index++, elem);
        if (index >= _format.element_count) {
            break;
        }
    }
    return os;
}

void VertexFormatGL::_layOutArrayOneElement(size_t index, const Element &elem) const
{
    bool normalized = false;
    glVertexAttribPointer(
        int(index), unsigned(elem.count), _typeValue(elem.type), normalized,
        unsigned(_format.stride), (const void *)uint64_t(elem.offset)
    );
}

void VertexFormatGL::_bindProgramOneElement(int programId, size_t index, const Element &elem) const
{
    glBindAttribLocation(programId, index, VertexField::attribute(elem.field, elem.index));
}

const char *VertexFormatGL::_typeName(ValueType::Type type) const
{
    switch (type) {
    case ValueType::FLOAT64:     return "GL_DOUBLE";
    case ValueType::FLOAT32:     return "GL_FLOAT";
    case ValueType::FLOAT16:     return "GL_HALF_FLOAT";
    case ValueType::UINT64:      return "GL_UNSIGNED_LONG";
    case ValueType::SINT64:      return "GL_LONG";
    case ValueType::UINT32:      return "GL_UNSIGNED_INT";
    case ValueType::SINT32:      return "GL_INT";
    case ValueType::UINT16:      return "GL_UNSIGNED_SHORT";
    case ValueType::SINT16:      return "GL_SHORT";
    case ValueType::UINT8:       return "GL_UNSIGNED_BYTE";
    case ValueType::SINT8:       return "GL_BYTE";
    case ValueType::N2_10_10_10: return "GL_2_10_10_10";
    case ValueType::NONE:        return "<none>";
    case ValueType::ERROR:       return "<error>";
    default:                     return "<unknown>";
    }
}

int VertexFormatGL::_typeValue(ValueType::Type type) const
{
    switch (type) {
    case ValueType::FLOAT64:     return -1;
    case ValueType::FLOAT32:     return GL_FLOAT;
    case ValueType::UINT64:      return -1;
    case ValueType::SINT64:      return -1;
    case ValueType::UINT32:      return GL_UNSIGNED_INT;
    case ValueType::SINT32:      return GL_INT;
    case ValueType::UINT16:      return GL_UNSIGNED_SHORT;
    case ValueType::SINT16:      return GL_SHORT;
    case ValueType::UINT8:       return GL_UNSIGNED_BYTE;
    case ValueType::SINT8:       return GL_BYTE;
    case ValueType::N2_10_10_10: return -1; //GL_INT_2_10_10_10_REV​;
    default:                     return -1;
    }
}

void VertexFormatGL::layOutArray() const
{
    typename VertexFormat::enumerated_elements_t elems;
    _format.enumerate(elems);
    size_t index = 0;
    for (const auto &elem: elems) {
        _layOutArrayOneElement(index++, elem);
        if (index >= _format.element_count) {
            break;
        }
    }
}

void VertexFormatGL::bindProgram(int programId) const
{
    typename VertexFormat::enumerated_elements_t elems;
    _format.enumerate(elems);
    size_t index = 0;
    for (const auto &elem: elems) {
        _bindProgramOneElement(programId, index++, elem);
        if (index >= _format.element_count) {
            break;
        }
    }
}

int test_vertex_format_gl()
{
    const char *tests[] = { "IL P3 N3:f16 T2 C3 C3 T2", "ILP3N3T2C3C3T2", "seveh", "ISP4T2 C3", "ISP4T2 C3", "MSP4T2C3", 0 };
    for (const char **t = tests; *t; t++) {
        VertexFormat vf;
        bool failed = !vf.parse(*t);
        std::cout << vf << std::endl;
        if (failed) {
            std::cout << "failed" << std::endl;
        } else {
            VertexFormatGL vfgl(vf);
            vfgl.codeLayOutArray(std::cout);
            vfgl.codeBindProgram(std::cout, "mProgramId");
        }
        std::cout << "--------" << std::endl << std::endl;
    }
    return 0;
}
