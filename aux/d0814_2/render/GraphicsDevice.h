#ifndef _GRAPHICSDEVICE_H
#define _GRAPHICSDEVICE_H

#include "core/core.h"
#include "Material.h"
#include "Light.h"

class GraphicsDevice
{
public:

    static void            CreateInstance(const core::Size &frameSize) { if (!sInstance) sInstance = new GraphicsDevice(frameSize); }
    static GraphicsDevice *GetInstance()                               { return sInstance; }
    static void            DestroyInstance()                           { if (sInstance) delete sInstance; sInstance = 0; }

    inline core::Size GetFrameSize() const { return mFrameSize; }

    void              Initialize();

    void              SetColor(const Vector4 &color);
    const Vector4    &GetColor() const;
    bool              HasGlobalAlpha() const;
    void              EnableColorMaterial(bool v);

    void              EnableDepthTest(bool v);
    void              EnableDepthWrite(bool v);
    void              EnableCullFace(bool v);
    void              EnableLighting(bool v);
    void              EnableFog(const TriState &v);
    TriState          GetEnableFog() const { return mEnableFog; }

    void              SetProjection(const Matrix &m);
    void              SetMaterial(const Material &m);
    void              SetLight(const Light &light, bool enable = true);
    void              SetFog(const Vector4 &color, float density = 0.3f);
    void              SetUvTransform(const Matrix &m);

    void              SetBaseUvTransform(const Matrix &m);

    void              EnableLight(int id, bool enable = true);
    void              EnableLight(const Light &light, bool enable = true) { EnableLight(light.id, enable); }

    void              StartFrame() {}
    void              EndFrame() { mFrame++; }

    int               GetFrameCount() const { return mFrame; }

    static void       ClearAll();
    static void       ClearZBuffer();

private:

    static GraphicsDevice *sInstance;

    GraphicsDevice(const core::Size &frameSize) : mFrameSize(frameSize), mColor(1.f, 1.f, 1.f, 1.f), mUvTransformInvalid(true), mFrame(0) {}

    core::Size mFrameSize;
    TriState   mEnableFog;
    Vector4    mColor;
    Matrix     mUvTransform;
    Matrix     mBaseUvTransform;
    bool       mUvTransformInvalid;
    int        mFrame;
};

#endif // _GRAPHICSDEVICE_H