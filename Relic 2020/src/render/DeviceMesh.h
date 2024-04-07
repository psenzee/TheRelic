#ifndef _DEVICEMESH_H
#define _DEVICEMESH_H

#include "core/core.h"
#include "core/aabox.h"
#include "core/global.h"
#include "fast/Allocator.h"

class RenderContext;
class OpenGLESMesh;

#include "IDrawable.h"

class DeviceMesh : public IDrawable 
{
public:
    
    CLASS_NEW_DELETE()

    DeviceMesh(const char *filename);
    DeviceMesh(const void *data, int size);
    ~DeviceMesh();
    
    void          RenderImmediate(RenderContext &context);
    bool          IsVisible(const RenderContext &context) const;
    const AABox  &GetBounds() const;
    
    bool          SetProperty(const char *key, const char *value);

    void          Retain();
    void          Release();

    bool          IsLoaded() const { return mMesh != 0; }
    
private:
    
    bool          SetNormalAction(const char *action);
    
    OpenGLESMesh   *mMesh;
    int             mRef;
};

#endif // _DEVICEMESH_H
