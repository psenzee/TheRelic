/*
 */

#include "DeviceModel.h"
#include "GLUtils.h"
#include "OpenGLESMesh.h"
#include "OverheadCamera.h"
#include "LocalGameServices.h"
#include "Visibility.h"

DeviceModel::DeviceModel(const char *filename)
{
    char newname[1024];
    sprintf(newname, "%s.ipm", filename);
    mesh = new OpenGLESMesh;
    if (!mesh->read(newname))
    {
        printf("Unable to read file '%s'!\n", newname);
        delete mesh;
        mesh = 0;
    }
    else
    {
        printf("Loaded '%s' vertices %d normals %d uvs %d indices %d\n", 
               newname, mesh->verticesCount, mesh->normalsCount, mesh->uvsCount, mesh->indicesCount);
    }
}

bool DeviceModel::SetNormalAction(const char *action)
{
    if (!mesh) return false;

    if      (!action) return false;
    else if (strcmp(action, "NORMALIZE") == 0) { mesh->normalAction = OpenGLESMesh::NORMALIZE; return true; }
    else if (strcmp(action, "true")      == 0) { mesh->normalAction = OpenGLESMesh::NORMALIZE; return true; }
    else if (strcmp(action, "RESCALE"  ) == 0) { mesh->normalAction = OpenGLESMesh::RESCALE;   return true; }
    else if (strcmp(action, "NONE"     ) == 0) { mesh->normalAction = OpenGLESMesh::NONE;      return true; }
    else if (strcmp(action, "false"    ) == 0) { mesh->normalAction = OpenGLESMesh::NONE;      return true; }
    return false;
}

bool DeviceModel::SetProperty(const char *key, const char *value)
{
    if (!key) return false;
    else if (strcmp(key, "normalize") == 0) return SetNormalAction(value);
    return false;
}

const AABox &DeviceModel::GetBounds() const
{
    if (!mesh)
        return AABox();
    return mesh->bounds;
}

DeviceModel::~DeviceModel()
{
    if (mesh)
        delete mesh;
    mesh = 0;
}

void DeviceModel::Render(GraphicsDevice &device, const Matrix &transform, const OverheadCamera &camera)
{
    if (mesh)
    {
        glLoadMatrixf((GLfloat *)camera.GetView().data);
        glMultMatrixf((GLfloat *)transform.data);
        mesh->render();
    }
}

bool DeviceModel::IsVisible(const Matrix &transform, const OverheadCamera &camera) const
{
    if (!mesh)
        return false;
    return Visibility::IsVisible(camera.GetViewProjection(), transform, mesh->bounds);
}