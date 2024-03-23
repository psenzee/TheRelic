#include "DeviceMesh.h"
#include "render/GLUtils.h"
#include "render/GLIncludes.h"
#include "render/OpenGLESMesh.h"
#include "OverheadCamera.h"
#include "RenderContext.h"
#include "Visibility.h"
#include "render/glError.h"
#include "GLAbstract.h"

DeviceMesh::DeviceMesh(const char *filename, bool compact) : mMesh(0)
{
    char name[1024];
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
    if (!mMesh) return false;

    if      (!action) return false;
    else if (strcmp(action, "NORMALIZE") == 0) { mMesh->SetNormalAction(NORMALIZE_ACTION_NORMALIZE); return true; }
    else if (strcmp(action, "true"     ) == 0) { mMesh->SetNormalAction(NORMALIZE_ACTION_NORMALIZE); return true; }
    else if (strcmp(action, "RESCALE"  ) == 0) { mMesh->SetNormalAction(NORMALIZE_ACTION_RESCALE);   return true; }
    else if (strcmp(action, "NONE"     ) == 0) { mMesh->SetNormalAction(NORMALIZE_ACTION_NONE);      return true; }
    else if (strcmp(action, "false"    ) == 0) { mMesh->SetNormalAction(NORMALIZE_ACTION_NONE);      return true; }
    return false;
}

bool DeviceMesh::SetProperty(const char *key, const char *value)
{
    if (!key)                               return false;
    else if (strcmp(key, "normalize") == 0) return SetNormalAction(value);
    return false;
}

const AABox &DeviceMesh::GetBounds() const
{
    const static AABox empty;
    if (!mMesh) {
        return empty;
    }
    return mMesh->GetBounds();
}

DeviceMesh::~DeviceMesh()
{
    if (mMesh) {
        delete mMesh;
    }
    mMesh = 0;
}

void DeviceMesh::RenderImmediate(RenderContext &context)
{
    if (mMesh) {
        GLLoadMatrixStack(
            context.camera.GetProjection(),
            context.camera.GetView(),
            context.transform
        );
        mMesh->Render();
    }
}

bool DeviceMesh::IsVisible(const RenderContext &context) const
{
    if (!mMesh) {
        return false;
    }
    return Visibility::IsVisible(context.camera.GetViewProjection(), context.transform, mMesh->GetBounds());
}

void DeviceMesh::Retain()
{
    mRef++;
}

void DeviceMesh::Release() 
{
    if (!--mRef) {
        delete this;
    }
}
