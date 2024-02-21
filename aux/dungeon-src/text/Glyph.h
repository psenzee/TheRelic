/*
 *  Glyph.h
 *  GLGravity
 *
 *  Created by Paul Senzee on 12/31/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _GLYPH_H
#define _GLYPH_H

#include "core/xna.h"
#include "render/Animator.h"

class LocalGameServices;
class DeviceTexture;
class OverheadCamera;
class XmlElement;

class GlyphExtent
{
public:
    
    float   width, offset;
    Vector2 uv0, uv1;
    
    GlyphExtent() : width(1.0f), offset(0.0f) {}
};

class Glyph
{
public:
    
    GlyphExtent extent;
    Vector2     position;
    Vector2     scale;
    unsigned    color;
        
    // the prototypical quad in worldspace will be -0.5 to 0.5
        
    Glyph(const GlyphExtent &extent, const Vector2 &position, const Vector2 &scale, unsigned color);
};

class GlyphDrawList
{
public:
        
    LocalGameServices *services;
    Vector3            scale;
    Vector4            color;
        
    GlyphDrawList(LocalGameServices *services, DeviceTexture *texture, const Vector3 &scale, const Vector4 &color);
        
    void           Add(const Glyph &glyph);
    
    inline Vector3 GetMin()                              const { return minimum; }
    inline Vector3 GetMax()                              const { return maximum; }
    inline Vector3 GetSize()                             const { return maximum - minimum; }
    inline Vector3 GetCursor()                           const { return cursor; }
    void           SetCursor(const Vector3 &p);
    inline Vector3 GetInitialPosition()                  const { return initial; }
    inline void    SetInitialPosition(const Vector3 &p)        { initial = p; }
        
    void           Render(const Matrix &transform, const OverheadCamera &camera, const Vector4 &color);
        
private:
        
    std::vector<Vector3>   vertices;
    std::vector<Vector2>   uvs;
    std::vector<unsigned>  colors;
    DeviceTexture         *texture;
    Vector3                minimum,
                           maximum,
                           initial,
                           cursor;
};

class GlyphWriter
{
    enum { MAX_GLYPHS = 256, GLYPH_START = 32, BUFFER_BYTES = 1024 * 1024 };    
    
public:
        
    core::Size         glyphSize;
    DeviceTexture     *texture;
    GlyphExtent        extents[MAX_GLYPHS];
    LocalGameServices *services;
        
    GlyphWriter(const char *filename, LocalGameServices *services);
    ~GlyphWriter();
    
    Vector2            GetSize(const char *s, const Vector2 &scale);
    GlyphDrawList     *Write(const char *s, const Vector2 &pos, const Vector2 &scale, const Vector4 &color);
    GlyphDrawList     *Write(const char *s, const Vector2 &start, const Vector2 &pos, const Vector2 &scale, const Vector4 &color);
    
    void               Update();    
    
    void               DrawString(const char *s, const Vector2 &lookAt, const Vector2 &at, float alpha, float scaler, const Matrix &view);

    static bool        ParseColor(const char **s, unsigned &color);
    static bool        ParseColor(const char **s, Vector4 &color);
      
private:
    
    void              *buffer;    
    
    void               DrawString(const char *s, const Vector2 &start, const Vector2 &pos, const Vector4 &color, const Matrix &transform, const Matrix &view, void *buffer);
    
    void               Init(XmlElement *xml, LocalGameServices *services);
    static GlyphExtent ReadExtent(XmlElement *glyphs, int character);
    static void        ReadExtentsFile(XmlElement *xml, GlyphExtent *extents,
                                       const core::Size &textureSize, const core::Size &glyphSize);
};

#endif // _GLYPH_H