#include "DeviceMesh.h"
#include "render/GLUtils.h"
#include "platform/GLIncludes.h"
#include "render/OpenGLESMesh.h"
#include "OverheadCamera.h"
#include "RenderContext.h"
#include "Visibility.h"
#include "GraphicsDevice.h"

DeviceMesh::DeviceMesh(const char *filename, bool compact) : mMesh(0)
{
    char name[1024] = "";
    mMesh = new OpenGLESMesh;
    snprintf(name, sizeof(name) - 1, "%s.ips", filename);
    if (!mMesh->Read(name, compact)) {
        delete mMesh;
        mMesh = new OpenGLESMesh;
        snprintf(name, sizeof(name) - 1, "%s.ipi", filename);
        if (!mMesh->Read(name, compact)) {
            printf("Unable to read file '%s'!\n", filename);
            delete mMesh;
            mMesh = 0;
        }
    }
}

DeviceMesh::DeviceMesh(const void *data, int size, bool compact) : mMesh(0)
{
    mMesh = new OpenGLESMesh;
    if (!mMesh->ReadFromData((const char *)data, size, compact)) {
        printf("Unable to read mesh from data!\n");
        delete mMesh;
        mMesh = 0;
    }
}

bool DeviceMesh::SetNormalAction(const char *action)
{
    if (!mMesh || !action) return false;
    else if (strcmp(action, "NORMALIZE") == 0) { mMesh->SetNormalAction(OpenGLESMesh::NORMALIZE); return true; }
    else if (strcmp(action, "true"     ) == 0) { mMesh->SetNormalAction(OpenGLESMesh::NORMALIZE); return true; }
    else if (strcmp(action, "RESCALE"  ) == 0) { mMesh->SetNormalAction(OpenGLESMesh::RESCALE);   return true; }
    else if (strcmp(action, "NONE"     ) == 0) { mMesh->SetNormalAction(OpenGLESMesh::NONE);      return true; }
    else if (strcmp(action, "false"    ) == 0) { mMesh->SetNormalAction(OpenGLESMesh::NONE);      return true; }
    return false;
}

bool DeviceMesh::SetProperty(const char *key, const char *value)
{
    if (!key) return false;
    else if (strcmp(key, "normalize") == 0) return SetNormalAction(value);
    return false;
}

const AABox &DeviceMesh::GetBounds() const
{
    const static AABox empty;
    return !mMesh ? empty : mMesh->GetBounds();
}

DeviceMesh::~DeviceMesh()
{
    if (mMesh)
        delete mMesh;
    mMesh = 0;
}

void DeviceMesh::RenderImmediate(RenderContext &context)
{
    if (mMesh) {
        Matrix4f m(context.camera.GetView());
        m *= context.transform;
        auto program = context.device.SetShaderProgram("Shader");
        program->SetActive();
        program->SetUniform("u_transform", m);
        //glLoadMatrixf((float *)context.camera.GetView().data);
        //glMultMatrixf((float *)context.transform.data);
        mMesh->Render();
    }
}

bool DeviceMesh::IsVisible(const RenderContext &context) const
{
    return !mMesh ? false : Visibility::IsVisible(context.camera.GetViewProjection(), context.transform, mMesh->GetBounds());
}

void DeviceMesh::Retain()
{
    mRef++;
}

void DeviceMesh::Release() 
{
    if (!--mRef)
        delete this;
}
