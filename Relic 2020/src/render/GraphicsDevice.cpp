#include "GraphicsDevice.h"

#include "core/core.h"
#include "GLUtils.h"
#include "platform/GLIncludes.h"
#include "render/GLStates.h"

GraphicsDevice *GraphicsDevice::sInstance = 0;

void GraphicsDevice::Initialize()
{
    glViewport(0, 0, mFrameSize.width, mFrameSize.height);
    glShadeModel(GL_SMOOTH);
    SetColor(Vector4(1.f, 1.f, 1.f, 1.f));
}

void GraphicsDevice::ClearZBuffer()
{
    ::ClearZBuffer();
}

void GraphicsDevice::ClearAll()
{
    ::ClearAll();
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
    if (memcmp(&mUvTransform, &m, sizeof(Matrix)) == 0)
        return;
    mUvTransform = m;
    glMatrixMode(GL_TEXTURE);
    Matrix mm(mBaseUvTransform * mUvTransform);
    glLoadMatrixf((float *)&mm);
    glMatrixMode(GL_MODELVIEW);
}

void GraphicsDevice::SetBaseUvTransform(const Matrix &m)
{
    if (memcmp(&mUvTransform, &m, sizeof(Matrix)) == 0)
        return;
    mBaseUvTransform = m;
    glMatrixMode(GL_TEXTURE);
    Matrix mm(mBaseUvTransform * mUvTransform);
    glLoadMatrixf((float *)&mm);
    glMatrixMode(GL_MODELVIEW);
}
        
void GraphicsDevice::SetProjection(const Matrix &m)
{
    //Set the OpenGL projection matrix
    glMatrixMode(GL_PROJECTION);
    mProjection = m;
    glLoadMatrixf((float *)&m);
    glMatrixMode(GL_MODELVIEW); //Make the OpenGL modelview matrix the default
}

Matrix GraphicsDevice::GetProjection() const
{
    return mProjection;
}

void GraphicsDevice::SetColor(const Vector4 &color)
{
    if (mColor.x != color.x || mColor.y != color.y || mColor.z != color.z || mColor.w != color.w)
    {
        mColor = color;
        glColor4f(mColor.x, mColor.y, mColor.z, mColor.w);
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
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT,   (const float *)&m.ambient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE,   (const float *)&m.diffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION,  (const float *)&m.emissive);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR,  (const float *)&m.specular);
    glMaterialf (GL_FRONT_AND_BACK, GL_SHININESS,                 m.shininess);
}

void GraphicsDevice::SetFog(const Vector4 &color, float density)
{
	/*
//    EnableDepthTest(true);
    glEnable(GL_FOG); //enable the fog
    glFogi  (GL_FOG_MODE, GL_LINEAR); //set the fog mode to GL_EXP2
    glFogfv (GL_FOG_COLOR, (const float *)&color); //set the fog color to our color chosen above
    glFogf  (GL_FOG_DENSITY, density); //set the density to the value above
    glHint  (GL_FOG_HINT, GL_NICEST); // set the fog to look the nicest, may slow down on older cards
    glFogf  (GL_FOG_START, 550.0f);
    glFogf  (GL_FOG_END,   750.0f);
	 */
}

void GraphicsDevice::EnableFog(const TriState &value)
{
    if (!value.IsUnknown())
    {
        GLStates::fog.Set(value.ToBool());
        mEnableFog = value;
    }
}

void GraphicsDevice::EnableColorMaterial(bool v)
{
   if (v) glEnable(GL_COLOR_MATERIAL); else glDisable(GL_COLOR_MATERIAL);
}

void GraphicsDevice::SetLight(const Light &light, bool enable)
{
    glLoadIdentity();

    int id = GL_LIGHT0 + light.id;

    glLightfv(id, GL_AMBIENT,  (const float *)&light.ambient);
    glLightfv(id, GL_DIFFUSE,  (const float *)&light.diffuse);
    glLightfv(id, GL_SPECULAR, (const float *)&light.specular);

    float position[] = { light.position.x, light.position.y, light.position.z, light.position.w };
    glLightfv(id, GL_POSITION, position);

    if (enable)
        EnableLight(light.id, true);
}

void GraphicsDevice::EnableLight(int id, bool enable)
{
    GLStates::light[id].enable.Set(enable);
}