#include "VertexFormat.h"

typedef std::string_view::const_iterator sv_iter_t;

int parseOneOf(sv_iter_t &b, sv_iter_t e, std::span<const char * const> options, bool caseInsensitive = true)
{
    int i = 0;
    decltype(strncmp) *fns[] = { strncmp, strncasecmp };
    auto compare = fns[caseInsensitive ? 1 : 0];
    for (auto p : options) {
        size_t n = strlen(p), sz(e - b);
        if (sz >= n && compare(b, p, std::min(sz, n)) == 0) {
            b += n;
            return i;
        }
        i++;
    }
    return -1;
}

const char *ValueType::name(Type t)
{
    if (t == ERROR) {
        return "ERROR";
    } else if (is_none(t)) {
        return "none";
    } else if (t == N2_10_10_10) {
        return "n2_10_10_10";
    } else if (is_float(t)) {
        switch (size(t)) {
        case _8:  return "f8";
        case _16: return "f16";
        case _32: return "f32";
        case _64: return "f64";
        default:  break;
        }
    } else if (is_sint(t)) {
        switch (size(t)) {
        case _8:  return "i8";
        case _16: return "i16";
        case _32: return "i32";
        case _64: return "i64";
        default:  break;
        }
    } else if (is_uint(t)) {
        switch (size(t)) {
        case _8:  return "u8";
        case _16: return "u16";
        case _32: return "u32";
        case _64: return "u64";
        default:  break;
        }
    }
    return nullptr;
}

uint8_t ValueType::parseSize_8_16_32_64(sv_iter_t &b, sv_iter_t e)
{
    sv_iter_t check = b;
    for (; b != e && *b == '0'; ++b) {} // drop all leading zeros
    switch (*b++) {
        case '8':                            return  _8;
        case '1': if (b != e && *b++ == '6') return _16; break;
        case '3': if (b != e && *b++ == '2') return _32; break;
        case '6': if (b != e && *b++ == '4') return _64; break;
        default:                                         break;
    }
    b = check;
    return 0;
}

ValueType::Type ValueType::parse(sv_iter_t &b, sv_iter_t e)
{
    sv_iter_t check = b;
    char ch = tolower(*b);
    Type type = ERROR;
    constexpr const char
        *signeds[]   = { "integer", "int", "i", "signed", "sint", "si", "s" },
        *unsigneds[] = { "unsigned", "uint", "ui", "u" },
        *floats[]    = { "float", "fl", "f" },
        *doubles[]   = { "double", "dbl", "d" };
    switch (ch) {
        case 'd': {
            if (parseOneOf(b, e, doubles) == -1) { b = check; return type; }
            uint8_t size = parseSize_8_16_32_64(b, e);
            if (b == e || isspace(*b) || size == _64) {
                return FLOAT64; // double requires 64 bits
            }
            break;
        }
        case 'f': {
            if (parseOneOf(b, e, floats) == -1) { b = check; return type; }
            uint8_t size = parseSize_8_16_32_64(b, e);
            if (b == e || isspace(*b) || size != 0) {
                return Type(uint8_t(_FLOAT) | (size ? size : _32));
            }
            break;
        }
        case 'u': {
            if (parseOneOf(b, e, unsigneds) == -1) { b = check; return type; }
            uint8_t size = parseSize_8_16_32_64(b, e);
            if (b == e || isspace(*b) || size != 0) {
                return Type(uint8_t(_UINT) | (size ? size : _32));
            }
            break;
        }
        case 'i':
        case 's':  {
            if (parseOneOf(b, e, signeds) == -1) { b = check; return type; }
            uint8_t size = parseSize_8_16_32_64(b, e);
            if (b == e || isspace(*b) || size != 0) {
                return Type(uint8_t(_SINT) | (size ? size : _32));
            }
            break;
        }
        default: break;
    }
    return type;
}

int test_value_type()
{
    const char *succeed[] = {
        "double",
        "double ",
        "float32",
        "float16",
        "f16 ",
        "i16",
        "i8 ",
        "u ",
        "unsigned"
    };
    const char *fail[] = {
        "doub",
        "doubleds ",
        "float17",
        "float19",
        "flo16 ",
        "iinteger",
        "i3 ",
        "un ",
        "unsigneds"
    };
    std::cout << "--- expecting success ---" << std::endl;
    for (auto s: std::span<const char *>(succeed)) {
        std::string_view sv(s);
        sv_iter_t b = sv.begin();
        auto type = ValueType::parse(b, sv.end());
        auto name = ValueType::name(type);
        name = name ? name : "error";
        std::cout << "parse [" << sv << "] returned " << size_t(type) << ":" << name << std::endl;
    }
    std::cout << "--- expecting failure ---" << std::endl;
    for (auto s: std::span<const char *>(fail)) {
        std::string_view sv(s);
        sv_iter_t b = sv.begin();
        auto type = ValueType::parse(b, sv.end());
        auto name = ValueType::name(type);
        name = name ? name : "error";
        std::cout << "parse [" << sv << "] returned " << size_t(type) << ":" << name << std::endl;
    }
    std::cout << "-----------" << std::endl;
    return 0;
}
