#ifndef _RENDERSTATES_H
#define _RENDERSTATES_H

class GraphicsDevice;

class RenderStates
{
public:    
    
    float DepthBias;
    bool  Transparent;
    bool  DepthEnable;
    
    inline RenderStates() : DepthBias(0.0f), Transparent(false), DepthEnable(true) {}

    void Apply(GraphicsDevice &device);
    void Unapply(GraphicsDevice &device);
};

#endif // _RENDERSTATES_H