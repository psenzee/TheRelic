#ifndef _GLYPH_H
#define _GLYPH_H

#include "core/core.h"
#include "render/Animator.h"
#include "fast/Allocator.h"

class DeviceTexture;
class OverheadCamera;
class RenderContext;
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
    
    CLASS_NEW_DELETE()
        
    Vector3            scale;
    Vector4            color;
        
    GlyphDrawList(DeviceTexture *texture, const Vector3 &scale, const Vector4 &color);
        
    void           Add(const Glyph &glyph);
    
    inline Vector3 GetMin()                              const { return minimum; }
    inline Vector3 GetMax()                              const { return maximum; }
    inline Vector3 GetSize()                             const { return maximum - minimum; }
    inline Vector3 GetCursor()                           const { return cursor; }
    void           SetCursor(const Vector3 &p);
    inline Vector3 GetInitialPosition()                  const { return initial; }
    inline void    SetInitialPosition(const Vector3 &p)        { initial = p; }
        
    void           Render(RenderContext &context, const Matrix &transform, const Vector4 &color);
        
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
    
    enum TextJustify { TEXT_JUSTIFY_LEFT, TEXT_JUSTIFY_CENTER, TEXT_JUSTIFY_RIGHT };  

    CLASS_NEW_DELETE()    
        
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

    static bool        ParseColor(const char **s, unsigned &color);
    static bool        ParseColor(const char **s, Vector4 &color);
      
private:
    
    void              *buffer;    
    
    void               DrawString(RenderContext &context, const char *s, const Vector2 &start, const Vector2 &pos, const Vector4 &color, const Matrix &transform, const Matrix &view, void *buffer, bool useInlineColor, TextJustify justify = TEXT_JUSTIFY_LEFT);
    
    void               Init(XmlElement *xml);
    static GlyphExtent ReadExtent(XmlElement *glyphs, int character);
    static void        ReadExtentsFile(XmlElement *xml, GlyphExtent *extents,
                                       const core::Size &textureSize, const core::Size &glyphSize);
};

#endif // _GLYPH_H