#ifndef _TEXT_H
#define _TEXT_H

#include "core/core.h"
#include "render/Animator.h"

#include <vector>

class XmlElement;
class GlyphWriter;
class GlyphDrawList;
class RenderContext;

class Text
{
public:    
    
    static void    MakePartyText(const char *s, char *buffer, int seed = 0);
    static void    StripColor(const char *s, char *buffer);
        
    Text(GlyphWriter *writer, const char *s, const Vector2 &pos, const Vector2 &scale, const Vector3 &defaultColor, IAnimator *animator);
    ~Text();
        
    void    Update(const GameTime &time);
    void    ForceUpdate(const GameTime &time);
        
    Vector3 GetMin()               const;
    Vector3 GetMax()               const;
    Vector3 GetSize()              const;
    
    const Vector3 &GetRenderAnchorPosition() const;
    
    void Render(RenderContext &context, const Matrix &transform, const Vector4 &color);
    void Reset();
        
private:
        
    std::vector<GlyphDrawList *>  lists;
    GlyphWriter                  *writer;
    IAnimator                    *animator;
    Vector3                       anchor;

    void Add(const char *s, const Vector3 &pos, const Vector2 &scale, const Vector4 &color);
};

#endif // _TEXT_H