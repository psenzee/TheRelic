#ifndef _RENDERCONTEXT_H
#define _RENDERCONTEXT_H

#include "core/core.h"
#include "core/matrix4f.h"
#include "fast/Allocator.h"

class GraphicsDevice;
class OverheadCamera;

typedef void (*breakfunction_t)();

class RenderContext
{
public:
    
    CLASS_NEW_DELETE()

    GraphicsDevice       &device;
    const OverheadCamera &camera;
    Matrix4f              transform;
    Vector4               color;
    TriState              depthTest;
    TriState              depthWrite;
    bool                  blur;
    breakfunction_t       breakFunction;
    
    inline RenderContext(GraphicsDevice &device, const OverheadCamera &camera) : device(device), camera(camera), color(1.f, 1.f, 1.f, 1.f), blur(false), breakFunction(0) {}
};

#endif // _RENDERCONTEXT_H
