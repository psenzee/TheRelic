#ifndef _RENDERSTATES_H
#define _RENDERSTATES_H

#include "core/core.h"
#include "fast/Allocator.h"

class GraphicsDevice;

class RenderStates
{
public:    
    
    CLASS_NEW_DELETE() 
    
    Matrix4f UvTransform;
    Vector4  Color;
    int      RenderOrder;
    float    DepthBias;
    TriState Fog;
    bool     Transparent;
    bool     DepthTest;
    bool     DepthWrite;
    bool     CullFace;
    bool     TestVisible;
    bool     Blurrable;
    
    inline RenderStates() : UvTransform(), DepthBias(0.0f), Transparent(false), DepthTest(true), DepthWrite(true), CullFace(true), TestVisible(true), Blurrable(false), Fog(), RenderOrder(INT_MAX), Color(1.f, 1.f, 1.f, 1.f) {}

    void Apply(GraphicsDevice &device) const;
    void Unapply(GraphicsDevice &device) const;
};

#endif // _RENDERSTATES_H