#include "gl_utils.h"
#include "VertexFormatGL.h"
#include "VertexFormat.h"
#include "ValueType.h"
#include "gl_utils.h"

void VertexFormatGL::_layOutArrayOneElement(unsigned index, const Element &elem) const
{
    bool normalized = false;
    
    uint32_t locationId = VertexField::locationIdForTypeAndIndex(elem.field, elem.index);
    std::cerr << "location id " << locationId << " for type TEXTURE and index " << index << std::endl;
    _GL(glVertexAttribPointer(
        /*index*/ locationId,
        unsigned(elem.count), _typeValue(elem.type), normalized,
        unsigned(_format.stride), (const void *)uint64_t(elem.offset)
    ));
    _GL(glEnableVertexAttribArray(/*index*/locationId));
}

void VertexFormatGL::_bindAttribLocationOneElement(int programId, unsigned index, const Element &elem) const
{
    std::cerr << "attribute " << VertexField::attribute(elem.field, elem.index) << " " << std::endl;
    //_GL(glBindAttribLocation(programId, index, VertexField::attribute(elem.field, elem.index)));
}

void VertexFormatGL::_setProgramOneElement(int programId, unsigned index, const Element &elem) const
{
    std::cerr << "attribute " << VertexField::attribute(elem.field, elem.index) << " ";
    /*
    int location_id = _GL(glGetAttribLocation(programId, VertexField::attribute(elem.field, elem.index)));
    if (location_id != -1) {
      //  _GL(glEnableVertexAttribArray(location_id));
      //  std::cerr << "index " << index << " location_id " << location_id << " elem index " << int(elem.index) << std::endl;
    } else {
        std::cerr << " error glGetAttribLocation returned -1" << std::endl;
    }
    */
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
    unsigned index = 0;
    for (const auto &elem: elems) {
        _layOutArrayOneElement(index++, elem);
        if (index >= _format.element_count) {
            break;
        }
    }
}

void VertexFormatGL::setProgram(int programId) const
{
    typename VertexFormat::enumerated_elements_t elems;
    _format.enumerate(elems);
    unsigned index = 0;
    for (const auto &elem: elems) {
        _setProgramOneElement(programId, index++, elem);
        if (index >= _format.element_count) {
            break;
        }
    }
}

void VertexFormatGL::bindAttribLocations(int programId) const
{
    typename VertexFormat::enumerated_elements_t elems;
    _format.enumerate(elems);
    unsigned index = 0;
    for (const auto &elem: elems) {
        _bindAttribLocationOneElement(programId, index++, elem);
        if (index >= _format.element_count) {
            break;
        }
    }
}
