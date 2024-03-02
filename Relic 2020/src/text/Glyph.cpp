#include "Glyph.h"

#include "core/random.h"
#include "core/simplexml.h"
#include "core/global.h"

#include "xml/XmlUtil.h"
#include "xml/XmlObjectReader.h"

#include "render/OverheadCamera.h"
#include "render/DeviceTexture.h"
#include "render/Material.h"

Glyph::Glyph(const GlyphExtent &extent, const Vector2 &position, const Vector2 &scale, uint32_t color)
    : extent(extent), position(position), scale(scale), color(color)
{
}

GlyphWriter::GlyphWriter(const char *filename) : glyphSize(0, 0), buffer(0), _hack_override_color(0)
{
    buffer = new uint32_t [BUFFER_BYTES / sizeof(uint32_t)]; // text render buffer
    memset(buffer, 0, BUFFER_BYTES);
    const char *text = globalReadTextFile(filename);
    XmlElement *xml = parse(text);
    Init(xml);
    delete xml;
}

GlyphWriter::~GlyphWriter()
{
    if (buffer)
        delete [] (uint32_t *)buffer;
    buffer = 0;
}

Vector2 GlyphWriter::GetSize(const char *s, const Vector2 &scale, bool stopAtNewline)
{
    Vector2 position(0.0f, 0.0f);
    if (!s || !*s)
        return position;
    float maxx = 0.0f;
    const char *ends = s + strlen(s);
    uint32_t color = 0;
    for (; s < ends; )
    {
        if (*s == '\n')
        {
            if (stopAtNewline)
                break;
            position.y += scale.y;
            if (fabsf(position.x) > maxx)
                maxx = fabsf(position.x);
            position.x = 0.0f;
            s++;
        }
        else if (ParseColor(&s, color))
        {
            // skip this
        }
        else
        {
            int32_t c = ((uint8_t)*s) - GLYPH_START;
            if (c < 0 || c >= MAX_GLYPHS)
                c = 0;
            GlyphExtent &ex = extents[c];
            position.x += ex.width * scale.x;
            s++;
        }
    }
    if (fabsf(position.x) > maxx)
        maxx = fabsf(position.x);
    return Vector2(maxx, position.y);
}

GlyphDrawList *GlyphWriter::Write(const char *s, const Vector2 &pos, const Vector2 &scale, const Vector4 &color)
{
    return Write(s, pos, pos, scale, color);
}

GlyphDrawList *GlyphWriter::Write(const char *s, const Vector2 &start, const Vector2 &pos, const Vector2 &scale, const Vector4 &color)
{
    if (!s) return 0;
  //const uint8_t *us = static_cast<const uint8_t *>(s);
    GlyphDrawList *list    = new GlyphDrawList(texture, Vector3(scale.x, scale.y, 1.0f), color);
    Vector3        initial = v3(start), position(v3(pos));
    list->SetInitialPosition(initial);
    list->SetCursor(position);
    for (uint32_t i = 0, sz = (uint32_t)strlen(s); i < sz; i++)
    {
        if (s[i] == '\n')
        {
            position.y += scale.y;
            position.x  = initial.x;
        }
        else
        {
            int32_t c = (uint8_t)s[i] - GLYPH_START;
            if (c < 0 || c >= MAX_GLYPHS)
                c = 0;
            GlyphExtent &ex = extents[c];
            list->Add(Glyph(ex, Vector2(position.x + ex.offset * scale.x, position.y), scale, 0xffffffff));
            position.x += ex.width * scale.x;
        }
        list->SetCursor(position);        
    }
    return list;
}

void GlyphWriter::Init(XmlElement *xml)
{
    Vector2 gsz     = XmlObjectReader::ReadSize2(xml);
    this->glyphSize = core::Size((int32_t)gsz.x, (int32_t)gsz.y);
    this->texture   = new DeviceTexture(XmlUtil::GetExpectedAttributeValue(xml, "texture"));
    ReadExtentsFile(xml, extents, texture->GetSize(), glyphSize);
}    
        
GlyphExtent GlyphWriter::ReadExtent(XmlElement *glyphs, int32_t character)
{
    char s[32] = " ";
    s[0] = character + GLYPH_START;
    switch (s[0])
    {
    case '\"': strcpy((char *)s, "&quot;"); break;
    case '&':  strcpy((char *)s, "&amp;");  break;
    case '>':  strcpy((char *)s, "&gt;");   break;
    case '<':  strcpy((char *)s, "&lt;");   break;
    // more later perhaps
    default:
        if ((uint8_t)(*s) >= 128) {
            snprintf(s, sizeof(s) - 1, "&#%u;", (uint8_t)*s);
        }
        break;
    }
    GlyphExtent ge;
    const float defaultWidth = 0.8f, defaultOffset = 0.0f;
    ge.width  = defaultWidth;
    ge.offset = defaultOffset;
    XmlElement *e = XmlUtil::GetFirstElementWithKeyValue(glyphs, "glyph", "character", (const char *)s);
    if (e)
    {
        ge.width  = XmlUtil::GetFloat(e, "width",  defaultWidth);
        ge.offset = XmlUtil::GetFloat(e, "offset", defaultOffset);
    }
    return ge;
}
        
void GlyphWriter::ReadExtentsFile(XmlElement *xml, GlyphExtent *extents, const core::Size &textureSize, const core::Size &glyphSize)
{
    int32_t perRow = textureSize.width  / glyphSize.width,
            perCol = textureSize.height / glyphSize.height;
    Vector2 inverse(1.0f / perRow, 1.0f / perCol);
    int32_t a = MAX_GLYPHS, b = perRow * perCol;
    int32_t count = a < b ? a : b;
    for (int32_t i = 0; i < std::min(count + GLYPH_START, 255); i++)
    {
        GlyphExtent ge = ReadExtent(xml, i);
        Vector2 at((float)(i % perRow), (float)(i / perRow));
        ge.uv0 = at * inverse;
        ge.uv1 = at * inverse + inverse;
// these two lines for image that's not flipped top-to-bottom
ge.uv0.y = 1.0f - ge.uv0.y;
ge.uv1.y = 1.0f - ge.uv1.y;
        extents[i] = ge;
    }
}

static int32_t hex(char c)
{
    c = toupper(c);
    if ((c < '0' || c > '9') && (c < 'A' || c > 'F'))
        return -1;
    return (int32_t)(c <= '9' ? (c - '0') : ((c - 'A') + 10));
}

static int32_t hex(const char **s, int32_t sz)
{
    if (!s) return -1;
    const char *p = *s;
    int32_t v = 0, u = 0;
    char c = 0;
    for (int32_t i = 0; i < sz; i++, p++)
    {
        if (!(c = *p) || (u = hex(c)) == -1)
            return -1;
        v *= 16;
        v += u;
    }
    *s = p;
    return v;
}

bool GlyphWriter::ParseColor(const char **s, uint32_t &color)
{
    const char *p = *s;
    bool alpha = false;
    if (*p++ != '\\')
        return false;
    char c = *p++;
    if (c != '#' && c != '%')
        return false;
    alpha = (c == '%');
    color = alpha ? 0 : 0x000000ff; // if no alpha, set it to full    
    int32_t u = 0;
    if ((u = hex(&p, 2)) == -1) return false; color |= uint32_t(u) << 24; // red
    if ((u = hex(&p, 2)) == -1) return false; color |= uint32_t(u) << 16; // green
    if ((u = hex(&p, 2)) == -1) return false; color |= uint32_t(u) <<  8; // blue
    if (alpha)
    {
        if ((u = hex(&p, 2)) == -1) return false; color |= uint32_t(u); // alpha
    }
    *s = p;
    return true;
}

bool GlyphWriter::ParseColor(const char **s, Vector4 &color)
{
    uint32_t uc = 0;
    if (!ParseColor(s, uc))
        return false;
    color.x = ((uc >> 24) & 0x0ff) / 255.0f;
    color.y = ((uc >> 16) & 0x0ff) / 255.0f;
    color.z = ((uc >>  8) & 0x0ff) / 255.0f;
    color.w = ((uc >>  0) & 0x0ff) / 255.0f;
    return true;
}
