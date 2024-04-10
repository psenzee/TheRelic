#include "parse_color.h"
#include "tuple4f.h"

static int64_t hex(char c)
{
    c = toupper(c);
    if ((c < '0' || c > '9') && (c < 'A' || c > 'F')) {
        return -1;
    }
    return (int64_t)(c <= '9' ? (c - '0') : ((c - 'A') + 10));
}

static int64_t parse_hex(const char **s, size_t sz)
{
    if (!s) return -1;
    const char *p = *s;
    int64_t v = 0, u = 0;
    char c = 0;
    for (size_t i = 0; i < sz; i++, p++) {
        if (!(c = *p) || (u = hex(c)) == -1) {
            return -1;
        }
        v *= 16;
        v += u;
    }
    *s = p;
    return v;
}

bool parse_color(const char **s, uint32_t &color)
{
    const char *p = *s;
    bool alpha = false;
    if (*p++ != '\\') {
        return false;
    }
    char c = *p++;
    if (c != '#' && c != '%') {
        return false;
    }
    alpha = (c == '%');
    color = alpha ? 0 : 0x000000ff; // if no alpha, set it to full
    int32_t u = 0;
    if ((u = parse_hex(&p, 2)) == -1) { return false; }
    color |= uint32_t(u) << 24; // red
    if ((u = parse_hex(&p, 2)) == -1) { return false; }
    color |= uint32_t(u) << 16; // green
    if ((u = parse_hex(&p, 2)) == -1) { return false; }
    color |= uint32_t(u) <<  8; // blue
    if (alpha) {
        if ((u = parse_hex(&p, 2)) == -1) {
            return false;
        }
        color |= uint32_t(u); // alpha
    }
    *s = p;
    return true;
}

bool parse_color(const char **s, std::array<float, 4> &color)
{
    uint32_t uc = 0;
    if (!parse_color(s, uc)) {
        return false;
    }
    color[0] = ((uc >> 24) & 0x0ff) / 255.0f;
    color[1] = ((uc >> 16) & 0x0ff) / 255.0f;
    color[2] = ((uc >>  8) & 0x0ff) / 255.0f;
    color[3] = ((uc >>  0) & 0x0ff) / 255.0f;
    return true;
}

bool parse_color(const char **s, Tuple4f &color)
{
    std::array<float, 4> into;
    bool result = parse_color(s, into);
    color.x = into[0];
    color.y = into[1];
    color.z = into[2];
    color.w = into[3];
    return result;
}
