#include "VertexFormat.h"

typedef std::string_view::const_iterator sv_iter_t;

inline sv_iter_t        _consume_space(sv_iter_t b, sv_iter_t e) { for (; b != e && isspace(*b); ++b) {} return b; }
inline std::string_view  consume_space(sv_iter_t b, sv_iter_t e) { return std::string_view(_consume_space(b, e)); }
inline std::string_view  consume_space(std::string_view s)       { return consume_space(s.begin(), s.end()); }

const char *VertexField::name(Type type)
{
    static constexpr const char *types[] = { "position", "normal", "texture", "color", 0 };
    return types[type];
}

const char *VertexField::attribute(Type type, size_t index)
{
    if (index >= MAX_ITEMS_PER_FIELD) {
        return nullptr;
    }
    constexpr size_t COUNT = size_t(_TYPE_MAX) * MAX_ITEMS_PER_FIELD;
    constexpr size_t LENGTH = 32;
    typedef std::array<char, LENGTH * COUNT> buffer_table_t;
    static buffer_table_t table { 0 };
    char *buffer = &table[(size_t(type) * MAX_ITEMS_PER_FIELD + index) * LENGTH];
    if (!*buffer) {
        if (index) {
            snprintf(buffer, LENGTH - 1, "_%s_%lu", name(type), index);
        } else {
            snprintf(buffer, LENGTH - 1, "_%s", name(type));
        }
    }
    return buffer;
}

inline std::ostream &operator<<(std::ostream &os, const Element &e)
{
    return os << "offset " << int(e.offset) << " count " << int(e.count) << " component type " << ValueType::name(e.type)
              << " bytes " << sizeof(float) * e.count
              << " [" << VertexField::name(e.field) << "]";
}

std::ostream &VertexFormat::print(std::ostream &os) const
{
    os << "vertex format [" << format << "]";
    if (element_count) {
        os << " (" << size_t(element_count) << " element" << (element_count > 1 ? "s" : "") << ")";
    }
    if (error) {
        return os << " " << error_message << std::endl;
    }
    os << std::endl;
    if (layout == INTERLEAVED) {
        os << "  interleaved" << std::endl;
    } else if (layout == MULTIBUFFER) {
        os << "  multibuffer" << std::endl;
    }
    os << "  type " << _type_name(type) << std::endl;
    os << "  stride " << size_t(stride) << " bytes" << std::endl;
    const element_array_t *arrays[] = { &position, &normal, &textures, &colors };
    for (size_t i = 0; i < VertexField::_TYPE_MAX; ++i) {
        if (counts[i] > 0) {
            os << "  " << VertexField::name(i) << " count " << size_t(counts[i]) << ":" << std::endl;
            for (size_t j = 0; j < counts[i]; ++j) {
                os << "    " << (*arrays[i])[j] << std::endl;
            }
        }
    }
    return os;
}

void VertexFormat::enumerate(enumerated_elements_t &elements) const
{
    size_t count = 0;
    const element_array_t *arrays[] = { &position, &normal, &textures, &colors };
    for (auto arr: std::span<const element_array_t *>(arrays)) {
        for (auto elem: *arr) {
            if (elem.field != VertexField::_TYPE_MAX) {
                elements[count++] = elem;
            }
        }
    }
    std::sort(elements.begin(), elements.begin() + count, [](auto &a, auto &b) { return a.offset < b.offset; });
}

bool VertexFormat::parse(std::string_view s)
{
    format = s;
    parse_result_t pr = _parse(s);
    error = pr.first;
    std::string_view at(pr.second, s.end());
    if (pr.first == NO_ERROR) {
        error_message = "";
        return true;
    }
    _clear();
    char buffer[1024] = "";
    snprintf(buffer, sizeof(buffer) - 1, "%s at \'%s\'", _error_messages(error), at.data());
    error_message = buffer;
    return false;
}

const char *VertexFormat::_error_messages(Error e)
{
    // in the order of the Error enum
    static const char *text[] = {
        nullptr,
        "parse error: empty text",
        "parse error: unexpected end of text",
        "parse error: bad layout value, expected I for interleaved or M for multibuffer (not yet supported)",
        "parse error: bad layout value, M is valid but unsupported",
        "parse error: bad triangle type, expected L for list or S for strip",
        "parse error: unrecognized field name, expecting P, N, T or C",
        "parse error: unrecognized type name after :",
        "parse error: position (P...) error",
        "parse error: position with count should be P1, P2, P3 or P4",
        "parse error: too many positions given",
        "parse error: normal (N...) error",
        "parse error: normal with count should be N1, N2, N3 or N4",
        "parse error: too many normals given",
        "parse error: texture coords (T...) error",
        "parse error: texture coords with count should be T1, T2, T3 or T4",
        "parse error: too many texture coords given",
        "parse error: color (C...) error",
        "parse error: color with count should be C1, C2, C3 or C4",
        "parse error: too many colors given",
    };
    return text[e];
}

const char *VertexFormat::_type_name(Type t)
{
    static const char *text[] = {
        "none",
        "triangle list",
        "triangle strip",
    };
    return text[t];
}

void VertexFormat::_clear_error()
{
    error = NO_ERROR;
    if (!error_message.empty()) {
        error_message = "";
    }
}

void VertexFormat::_clear()
{
    type = TRI_NONE;
    stride = 0;
    element_count = 0;
    std::fill(counts.begin(), counts.end(), 0);
    element_array_t *arrays[] = { &position, &normal, &textures, &colors };
    for (auto arr: std::span<element_array_t *>(arrays)) {
        *arr = element_array_t();
    }
}

VertexFormat::parse_result_t VertexFormat::_parse(std::string_view s)
{
    using namespace std::string_view_literals;

    _clear();
    _clear_error();
    s = consume_space(s);
    if (s.empty()) { return _result(ERROR_PARSE_EMPTY_TEXT, s, s.begin()); }
    const_iter_t b = s.begin(), e = s.end();
    char ch = toupper(*b++);
    switch (ch) {
        case 'I': layout = INTERLEAVED; break;
        case 'M': layout = MULTIBUFFER; break;
        default: return _result(ERROR_PARSE_LAYOUT, s, b - 1);
    }
    if (layout == MULTIBUFFER) {
        return _result(ERROR_PARSE_LAYOUT_UNSUPPORTED, s, b - 1);
    }
    s = consume_space(b, e);
    if (s.empty()) { return _result(ERROR_PARSE_UNEXPECTED_END, s, b - 1); }
    ch = toupper(*b++);
    type = TRI_NONE;
    switch (ch) {
        case 'L': type = TRI_LIST;  break;
        case 'S': type = TRI_STRIP; break;
        default: return _result(ERROR_PARSE_TRIANGLE_TYPE, s, b - 1);
    }
    // for field lookup arrays below the order is always PNTC: 0:P 1:N 2:T 3:C
    // in the order of the VertexField enums
    while (b != e) {
        b = _consume_space(b, e);
        if (b == e) {
            break;
        }
        Element elem(stride, 0);
        auto checkpoint = b;
        char c = toupper(*b++);
        if (c == '#') { b = e; break; } // # is a comment, finish up
        size_t field = "PNTC"sv.find(c);
        if (field == std::string_view::npos) {
            return _result(ERROR_PARSE_UNRECOGNIZED_FIELD_NAME, s, checkpoint);
        }
        if (b == e) {
            return _result(ERROR_PARSE_UNEXPECTED_END, s, checkpoint);
        }
        elem.field = VertexField::from(field);
        size_t count = "1234"sv.find(*b++);
        if (count == std::string_view::npos) {
            return _result(_count_errors[field], s, checkpoint);
        }
        elem.count = count + 1;
        if (*b == ':') { // are we specifying a type?
            b++;
            elem.type = ValueType::parse(b, e);
            if (elem.type == ValueType::NONE || elem.type == ValueType::ERROR) {
                return _result(ERROR_PARSE_UNRECOGNIZED_TYPE_NAME, s, checkpoint);
            }
        }
        stride += elem.size();
        element_array_t *arrays[] = { &position, &normal, &textures, &colors };
        size_t cf = counts[field];
        if (cf >= _max_items[field]) {
            return _result(_too_many_errors[field], s, checkpoint);
        }
        elem.index = cf;
        (*arrays[field])[cf] = elem;
        counts[field]++;
        element_count++;
    }
    return _result(NO_ERROR, s, s.begin());
}

VertexFormat::parse_result_t VertexFormat::_result(Error e, std::string_view s, const_iter_t b)
{
    return parse_result_t(e, std::max(b, s.begin()));
}
