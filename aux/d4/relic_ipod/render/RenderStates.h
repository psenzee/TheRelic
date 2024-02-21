#ifndef _RENDERSTATES_H
#define _RENDERSTATES_H

class GraphicsDevice;

class RenderStates
{
public:    
    
    float DepthBias;
    bool  Transparent;
    bool  DepthTest;
    bool  DepthWrite;
    bool  CullFace;
    
    inline RenderStates() : DepthBias(0.0f), Transparent(false), DepthTest(true), DepthWrite(true), CullFace(true) {}

    void Apply(GraphicsDevice &device) const;
    void Unapply(GraphicsDevice &device) const;
};

#endif // _RENDERSTATES_H