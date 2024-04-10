#pragma once

#include "core/core.h"
#include "render/Animator.h"
#include "fast/Allocator.h"
#include "Glyph.h"

class DeviceTexture;
class RenderContext;

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
