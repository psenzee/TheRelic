#pragma once

#include "core/core.h"
#include "render/Animator.h"
#include "fast/Allocator.h"
#include "Glyph.h"
#include "GlyphDrawList.h"
#include "DynamicBuffer.h"
#include "CommonVertex.h"

class DeviceTexture;
class OverheadCamera;
class RenderContext;
class XmlElement;

class GlyphWriter
{
    enum { MAX_GLYPHS = 256, GLYPH_START = 32, BUFFER_BYTES = 1024 * 1024 };
    
public:
    
    enum TextJustify { TEXT_JUSTIFY_LEFT, TEXT_JUSTIFY_CENTER, TEXT_JUSTIFY_RIGHT };  

    CLASS_NEW_DELETE()

    typedef DynamicBuffer<CommonVertex> buffer_t;
        
    core::Size         glyphSize;
    DeviceTexture     *texture;
    GlyphExtent        extents[MAX_GLYPHS];
    const Vector4     *_hack_override_color;
        
    GlyphWriter(const char *filename);
    ~GlyphWriter();
    
    Vector2            GetSize(const char *s, const Vector2 &scale, bool stopAtNewline = false);
    GlyphDrawList     *Write(const char *s, const Vector2 &pos, const Vector2 &scale, const Vector4 &color);
    GlyphDrawList     *Write(const char *s, const Vector2 &start, const Vector2 &pos, const Vector2 &scale, const Vector4 &color);

    void               Update();    
    
    void               DrawString(RenderContext &context, const char *s, const Vector2 &lookAt, const Vector2 &at, float alpha, float scaler, const Matrix &view, bool useInlineColor, TextJustify justify = TEXT_JUSTIFY_LEFT);
    void               DrawString(RenderContext &context, const char *s, const Vector2 &lookAt, const Vector2 &at, const Vector4 &color, float scaler, const Matrix &view, bool useInlineColor, TextJustify justify = TEXT_JUSTIFY_LEFT);
      
private:
    
    void              *buffer;
    buffer_t          _buffer;

    void               DrawString(RenderContext &context, const char *s, const Vector2 &start, const Vector2 &pos, const Vector4 &color, const Matrix &transform, const Matrix &view, void *buffer, bool useInlineColor, TextJustify justify = TEXT_JUSTIFY_LEFT);
    
    void               Init(XmlElement *xml);
    static GlyphExtent ReadExtent(XmlElement *glyphs, int character);
    static void        ReadExtentsFile(XmlElement *xml, GlyphExtent *extents,
                                       const core::Size &textureSize, const core::Size &glyphSize);
};
