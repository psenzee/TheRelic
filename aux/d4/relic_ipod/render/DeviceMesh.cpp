#include "DeviceMesh.h"
#include "platform/GLUtils.h"
#include "platform/GLIncludes.h"
#include OPENGLMESH_INCLUDE
#include "OverheadCamera.h"
#include "RenderContext.h"
#include "Visibility.h"

DeviceMesh::DeviceMesh(const char *filename) : mMesh(0)
{
    char newname[1024];
    mMesh = new OpenGLMeshType;
    sprintf(newname, "%s.ips", filename);
    if (!mMesh->read(newname))
    {
        delete mMesh;
        mMesh = 0;

        mMesh = new OpenGLMeshType;
        sprintf(newname, "%s.ipi", filename);
        if (!mMesh->read(newname))
        {
            printf("Unable to read file '%s'!\n", filename);
            delete mMesh;
            mMesh = 0;
        }
    }
}

bool DeviceMesh::SetNormalAction(const char *action)
{
    if (!mMesh) return false;

    if      (!action) return false;
    else if (strcmp(action, "NORMALIZE") == 0) { mMesh->normalAction = OpenGLMeshType::NORMALIZE; return true; }
    else if (strcmp(action, "true")      == 0) { mMesh->normalAction = OpenGLMeshType::NORMALIZE; return true; }
    else if (strcmp(action, "RESCALE"  ) == 0) { mMesh->normalAction = OpenGLMeshType::RESCALE;   return true; }
    else if (strcmp(action, "NONE"     ) == 0) { mMesh->normalAction = OpenGLMeshType::NONE;      return true; }
    else if (strcmp(action, "false"    ) == 0) { mMesh->normalAction = OpenGLMeshType::NONE;      return true; }
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
    if (!mMesh)
        return empty;
    return mMesh->bounds;
}

DeviceMesh::~DeviceMesh()
{
    if (mMesh)
        delete mMesh;
    mMesh = 0;
}

void DeviceMesh::RenderImmediate(RenderContext &context)
{
    if (mMesh)
    {
        glLoadMatrixf((float *)context.camera.GetView().data);
        glMultMatrixf((float *)context.transform.data);
        mMesh->render();
    }
}

bool DeviceMesh::IsVisible(const RenderContext &context) const
{
    if (!mMesh)
        return false;
    return Visibility::IsVisible(context.camera.GetViewProjection(), context.transform, mMesh->bounds);
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