/*
 *  GlyphDrawList.cpp
 *  GLGravity
 *
 *  Created by Paul Senzee on 12/31/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include "Glyph.h"
#include "GLUtils.h"
#include "OverheadCamera.h"
#include "DeviceTexture.h"
#include "LocalGameServices.h"
#include "XmlUtil.h"
#include "XmlObjectReader.h"
#include "Material.h"

#include "core/random.h"
#include "core/simplexml.h"
#include "core/global.h"

Glyph::Glyph(const GlyphExtent &extent, const Vector2 &position, const Vector2 &scale, unsigned color, bool indices) 
    : extent(extent), position(position), scale(scale), color(color), indices(indices)
{
}

GlyphDrawList::GlyphDrawList(LocalGameServices *services, DeviceTexture *texture, const Vector3 &scale, const Vector4 &color) 
    : services(services), scale(scale), color(color), texture(texture), maximum(FLT_MIN, FLT_MIN, FLT_MIN), minimum(FLT_MAX, FLT_MAX, FLT_MAX)
{
}

void GlyphDrawList::Reserve(size_t glyphs)
{
    size_t vertices = glyphs * 6; // for now
    vertices.reserve(vertices);
    
    
}

void GlyphDrawList::Add(const Glyph &glyph)
{
    Vector3 p(glyph.position.x, glyph.position.y, 0.0f),
            s(glyph.scale.x,    glyph.scale.y,    1.0f);
    
    static const Vector3 UL(-0.5f, -0.5f, 0.0f), UR( 0.5f, -0.5f, 0.0f),
                         LL(-0.5f,  0.5f, 0.0f), LR( 0.5f,  0.5f, 0.0f);
    
    float u0 = glyph.extent.uv0.x, v0 = glyph.extent.uv0.y,
          u1 = glyph.extent.uv1.x, v1 = glyph.extent.uv1.y;

    size_t index = vertices.size();

    vertices.push_back(p + UL * s); uvs.push_back(Vector2(u0, v0)); colors.push_back(glyph.color);
    vertices.push_back(p + LL * s); uvs.push_back(Vector2(u0, v1)); colors.push_back(glyph.color);
    vertices.push_back(p + LR * s); uvs.push_back(Vector2(u1, v1)); colors.push_back(glyph.color);
    vertices.push_back(p + UR * s); uvs.push_back(Vector2(u1, v0)); colors.push_back(glyph.color);

    indices.push_back(index + 0);
    indices.push_back(index + 1);
    indices.push_back(index + 2);
    
    indices.push_back(index + 0);
    indices.push_back(index + 2);
    indices.push_back(index + 3);
}

void GlyphDrawList::SetCursor(const Vector3 &p)
{
    cursor = p;
    if (minimum.x > maximum.x)
        initial = cursor;    
    minimum = minimum.minimum(p);
    maximum = maximum.maximum(p);
}
    
void GlyphDrawList::Render(const Matrix &transform, const OverheadCamera &camera, const Vector4 &color)
{
    ClearCachedPointers();
    
    texture->Set(*(services->device), GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    ClearCachedPointers();

    glDisable(GL_CULL_FACE);

    Vector4 clr(color * this->color);

    static Vector4  zeros(0.0f, 0.0f, 0.0f, 1.0f);
    static Material m(zeros, zeros, zeros, zeros, 0.0f);
    m.ambient = clr * Vector4(2.0f, 2.0f, 2.0f, 0.0f);
    m.diffuse = Vector4(1.0f, 1.0f, 1.0f, clr.w);        
    
    m.Set(*(services->device));

    glDisableClientState(GL_NORMAL_ARRAY);    
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    glDisable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);
    
    glVertexPointer  (3, GL_FLOAT,         0, (GLfloat *)&vertices[0]);
    glTexCoordPointer(2, GL_FLOAT,         0, (GLfloat *)&uvs[0]);
    glColorPointer   (4, GL_UNSIGNED_BYTE, 0, (GLfloat *)&colors[0]);    

    glLoadMatrixf((GLfloat *)(camera.GetView().data));
    glMultMatrixf((GLfloat *)transform.data);        
    
    glDrawElements(GL_TRIANGLES, 0, vertices.size(), .., &indices[0], ..);
    
    glDisableClientState(GL_COLOR_ARRAY);
    
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
}

GlyphWriter::GlyphWriter(const char *filename, LocalGameServices *services) : glyphSize(0, 0)
{
    const char *text = globalReadTextFile(filename);
    XmlElement *xml = parse(text);
    Init(xml, services);
}

Vector2 GlyphWriter::GetSize(const char *s, const Vector2 &scale)
{
    Vector2 position(0.0f, 0.0f);
    if (!s || !*s)
        return position;
    float maxx = 0.0f;
    for (size_t i = 0, sz = strlen(s); i < sz; i++)
    {
        if (s[i] == '\n')
        {
            position.y += scale.y;
            if (fabsf(position.x) > maxx)
                maxx = fabsf(position.x);
            position.x  = 0.0f;
        }
        else
        {
            int c = s[i] - GLYPH_START;
            if (c < 0 || c >= MAX_GLYPHS)
                c = 0;
            GlyphExtent &ex = extents[c];
            position.x += ex.width * scale.x;
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

static int hex(char c)
{
    c = toupper(c);
    if ((c < '0' || c > '9') && (c < 'A' || c > 'F'))
        return -1;
    return (int)(c <= '9' ? (c - '0') : ((c - 'A') + 10));
}

static int hex(const char **s, int sz)
{
    if (!s) return -1;
    const char *p = *s;
    int v = 0, u = 0;
    char c = 0;
    for (int i = 0; i < sz; i++, p++)
    {
        if (!(c = *p) || (u = hex(c)) == -1)
            return -1;
        v *= 16;
        v += u;
    }
    *s = p;
    return v;
}

static bool parsecolor(const char **s, unsigned &color)
{
    color = 0x000000ff; // set alpha to full
    const char *p = *s;
    if (*p++ != '\\' || *p++ != '#')
        return false;
    unsigned int u = 0;
    if ((u = hex(&p, 2)) == -1) return false; color |= unsigned(u) << 24; // red
    if ((u = hex(&p, 2)) == -1) return false; color |= unsigned(u) << 16; // green
    if ((u = hex(&p, 2)) == -1) return false; color |= unsigned(u) <<  8; // blue
    *s = p;
    return true;
}

GlyphDrawList *GlyphWriter::Write(const char *s, const Vector2 &start, const Vector2 &pos, const Vector2 &scale, const Vector4 &color)
{
    if (!s) return 0;
    GlyphDrawList *list    = new GlyphDrawList(services, texture, Vector3(scale.x, scale.y, 1.0f), color);
    Vector3        initial = v3(start), position(v3(pos));
    unsigned int   clr     = 0xffffffff;
    list->SetInitialPosition(initial);
    list->SetCursor(position);
    while (*s)
    {    
        if (*s == '\n')
        {
            position.y += scale.y;
            position.x  = initial.x;
            s++;
        }
        else if (!parsecolor(&s, clr))
        {
            int c = *s - GLYPH_START;
            if (c < 0 || c >= MAX_GLYPHS)
                c = 0;
            GlyphExtent &ex = extents[c];
            list->Add(Glyph(ex, Vector2(position.x + ex.offset * scale.x, position.y), scale, clr));
            position.x += ex.width * scale.x;
            s++;
        }
        list->SetCursor(position);
    }
    return list;
}

void GlyphWriter::Init(XmlElement *xml, LocalGameServices *services)
{
    Vector2 gsz     = XmlObjectReader::ReadSize2(xml);
    this->services  = services;
    this->glyphSize = core::Size((int)gsz.x, (int)gsz.y);
    this->texture   = new DeviceTexture(XmlUtil::GetExpectedAttributeValue(xml, "texture"));
    ReadExtentsFile(xml, extents, texture->GetSize(), glyphSize);
}    
        
GlyphExtent GlyphWriter::ReadExtent(XmlElement *glyphs, int character)
{
    char s[32] = " ";
    s[0] = character + GLYPH_START;
    switch (s[0])
    {
    case '\"': strcpy(s, "&quot;"); break;
    case '&':  strcpy(s, "&amp;");  break;
    case '>':  strcpy(s, "&gt;");   break;
    case '<':  strcpy(s, "&lt;");   break;
    // more later perhaps
    }
    GlyphExtent ge;
    const float defaultWidth = 0.8f, defaultOffset = 0.0f;
    ge.width  = defaultWidth;
    ge.offset = defaultOffset;
    XmlElement *e = XmlUtil::GetFirstElementWithKeyValue(glyphs, "glyph", "character", s);
    if (e)
    {
        ge.width  = XmlUtil::GetFloat(e, "width",  defaultWidth);
        ge.offset = XmlUtil::GetFloat(e, "offset", defaultOffset);
    }
    return ge;
}
        
void GlyphWriter::ReadExtentsFile(XmlElement *xml, GlyphExtent *extents, const core::Size &textureSize, const core::Size &glyphSize)
{
    int perRow = textureSize.width  / glyphSize.width,
           perCol = textureSize.height / glyphSize.height;
    Vector2 inverse(1.0f / perRow, 1.0f / perCol);
    int a = MAX_GLYPHS, b = perRow * perCol;
    int count = a < b ? a : b;
    for (int i = 0; i < count; i++)
    {
        GlyphExtent ge = ReadExtent(xml, i);
        Vector2 at(i % perRow, i / perRow);
        ge.uv0 = at * inverse;
        ge.uv1 = at * inverse + inverse;
        extents[i] = ge;
    }
}