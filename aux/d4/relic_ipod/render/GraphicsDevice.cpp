#include "GraphicsDevice.h"

#include "core/core.h"
#include "platform/GLUtils.h"
#include "platform/GLIncludes.h"
#include "render/GLStates.h"

void GraphicsDevice::Initialize()
{
    glViewport(0, 0, frameSize.width, frameSize.height);
    glShadeModel(GL_SMOOTH);
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
        
void GraphicsDevice::SetProjection(const Matrix &m)
{
     //Set the OpenGL projection matrix
     glMatrixMode(GL_PROJECTION);
     glLoadMatrixf((float *)&m);
     glMatrixMode(GL_MODELVIEW); //Make the OpenGL modelview matrix the default
}

void GraphicsDevice::SetMaterial(const Material &m)
{                                     
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT,   (const float *)&m.ambient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE,   (const float *)&m.diffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION,  (const float *)&m.emissive);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR,  (const float *)&m.specular);
    glMaterialf (GL_FRONT_AND_BACK, GL_SHININESS,                 m.shininess);
}

void GraphicsDevice::SetLight(const Light &light, bool enable)
{
    glLoadIdentity();

    int id = GL_LIGHT0 + light.id;

    glLightfv(id, GL_AMBIENT,  (const float *)&light.ambient);
    glLightfv(id, GL_DIFFUSE,  (const float *)&light.diffuse);
    glLightfv(id, GL_SPECULAR, (const float *)&light.specular);

    float position[] = { light.position.x, light.position.y, light.position.z, 0.0f /* here depends on the type.. */ };
    glLightfv(id, GL_POSITION, position);

    if (enable)
        EnableLight(light.id, true);
}

void GraphicsDevice::EnableLight(int id, bool enable)
{
    GLStates::light[id].enable.Set(enable);
}