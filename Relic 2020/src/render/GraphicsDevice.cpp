#include "GraphicsDevice.h"

#include "core/core.h"
#include "GLUtils.h"
#include "render/GLIncludes.h"
#include "render/GLStates.h"
#include "OverheadCamera.h"
#include "GLAbstract.h"
#include "glError.h"

GraphicsDevice *GraphicsDevice::sInstance = 0;

void GraphicsDevice::Initialize()
{
    _GLv(glViewport(0, 0, mFrameSize.width, mFrameSize.height));
    GLShadeModelSmooth();
    SetColor(Vector4(1.f, 1.f, 1.f, 1.f));
}

void GraphicsDevice::ClearZBuffer()
{
    GLClearZBuffer();
}

void GraphicsDevice::ClearAll()
{
    GLClearAll();
}

void GraphicsDevice::EnableDepthTest(bool v)
{
    GLStates::depthTest.Set(v);
}

void GraphicsDevice::EnableCullFace(bool v)
{
    GLStates::cullFace.Set(v);
}

void GraphicsDevice::EnableDepthWrite(bool v)
{
    GLStates::depthWrite.Set(v);
}

void GraphicsDevice::EnableLighting(bool v)
{
    GLStates::lighting.Set(v);
}

void GraphicsDevice::SetUvTransform(const Matrix &m)
{
    if (memcmp(&mUvTransform, &m, sizeof(Matrix)) == 0) {
        return;
    }
    mUvTransform = m;
    Matrix mm(mBaseUvTransform * mUvTransform);
    GLLoadTextureMatrix(mm);
}

void GraphicsDevice::SetBaseUvTransform(const Matrix &m)
{
    if (memcmp(&mUvTransform, &m, sizeof(Matrix)) == 0) {
        return;
    }
    mBaseUvTransform = m;
    Matrix mm(mBaseUvTransform * mUvTransform);
    GLLoadTextureMatrix(mm);
}
        
void GraphicsDevice::SetProjection(const Matrix &m)
{
    //Set the OpenGL projection matrix
    mProjection = m;
    GLLoadProjectionMatrix(m);
}

Matrix GraphicsDevice::GetProjection() const
{
    return mProjection;
}

void GraphicsDevice::SetColor(const Vector4 &color)
{
    if (mColor.x != color.x || mColor.y != color.y || mColor.z != color.z || mColor.w != color.w) {
        mColor = color;
        GLSetColor(mColor);
    }
}

const Vector4 &GraphicsDevice::GetColor() const
{
    return mColor;
}

bool GraphicsDevice::HasGlobalAlpha() const
{
    return (mColor.w != 1.0f);
}

void GraphicsDevice::SetMaterial(const Material &m)
{
    GLSetMaterial4(GL_DIFFUSE,   m.diffuse);
    GLSetMaterial4(GL_EMISSION,  m.emissive);
    GLSetMaterial4(GL_SPECULAR,  m.specular);
    GLSetMaterial (GL_SHININESS, m.shininess);
}

void GraphicsDevice::EnableColorMaterial(bool v)
{
    GLSetEnabled(GL_COLOR_MATERIAL, v);
}

bool GraphicsDevice::IsLightingEnabled() const
{
    return GLStates::lighting.Get();
}

void GraphicsDevice::SetLight(const OverheadCamera &camera, size_t slot, const Light &light, bool enable)
{
    GLLoadIdentityMatrix();

    int id = GL_LIGHT0 + slot;

    Vector4 world = light.vectors[Light::V4_WORLD_POSITION];
    Vector3 pos = camera.GetLightPositionFromWorld(world.xyz());
    float eye[4] = { pos.x, pos.y, pos.z, world.w };

    GLSetLight4(id, GL_POSITION,              (const float *)eye);

    GLSetLight4(id, GL_AMBIENT,               (const float *)&light.vectors[Light::V4_AMBIENT]);
    GLSetLight4(id, GL_DIFFUSE,               (const float *)&light.vectors[Light::V4_DIFFUSE]);
    GLSetLight4(id, GL_SPECULAR,              (const float *)&light.vectors[Light::V4_SPECULAR]);
    GLSetLight4(id, GL_SPOT_DIRECTION,        (const float *)&light.vectors[Light::V4_SPOT_DIRECTION]);

    GLSetLight (id, GL_SHININESS,             light.floats[Light::F1_SHININESS]);
    GLSetLight (id, GL_SPOT_EXPONENT,         light.floats[Light::F1_SPOT_EXPONENT]);
    GLSetLight (id, GL_SPOT_CUTOFF,           light.floats[Light::F1_SPOT_CUTOFF]);
    GLSetLight (id, GL_CONSTANT_ATTENUATION,  light.floats[Light::F1_CONSTANT_ATTENUATION]);
    GLSetLight (id, GL_LINEAR_ATTENUATION,    light.floats[Light::F1_LINEAR_ATTENUATION]);
    GLSetLight (id, GL_QUADRATIC_ATTENUATION, light.floats[Light::F1_QUADRATIC_ATTENUATION]);

    if (enable) {
        EnableLight(slot, true);
    }
}

void GraphicsDevice::EnableLight(size_t slot, bool enable)
{
    GLStates::light[slot].enable.Set(enable);
}
