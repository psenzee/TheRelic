#ifndef _RENDERCONTEXT_H
#define _RENDERCONTEXT_H

#include "core/core.h"
#include "core/matrix4f.h"

class GraphicsDevice;
class OverheadCamera;

class RenderContext
{
public:

    GraphicsDevice       &device;
    const OverheadCamera &camera;
    Matrix4f              transform;    
    
    inline RenderContext(GraphicsDevice &device, const OverheadCamera &camera) : device(device), camera(camera) {}
};

#endif // _RENDERCONTEXT_H
