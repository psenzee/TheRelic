#ifndef _GRAPHICSDEVICE_H
#define _GRAPHICSDEVICE_H

#include "core/core.h"
#include "Material.h"
#include "Light.h"

class GraphicsDevice
{
public:

    GraphicsDevice(const core::Size &frameSize) : frameSize(frameSize) {}

    inline core::Size GetFrameSize() const { return frameSize; }

    void Initialize();

    void EnableDepthTest(bool v);
    void EnableDepthWrite(bool v);
    void EnableCullFace(bool v);
    void EnableLighting(bool v);

    void SetProjection(const Matrix &m);
    void SetMaterial  (const Material &m);
    void SetLight     (const Light &light, bool enable = true);

    void EnableLight  (int id, bool enable = true);
    void EnableLight  (const Light &light, bool enable = true) { EnableLight(light.id, enable); }

private:

    core::Size frameSize;
};

#endif // _GRAPHICSDEVICE_H