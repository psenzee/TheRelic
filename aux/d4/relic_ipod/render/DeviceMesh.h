#ifndef _DEVICEMESH_H
#define _DEVICEMESH_H

#include "core/core.h"
#include "core/aabox.h"
#include "core/global.h"

class RenderContext;

#include "IDrawable.h"

class DeviceMesh : public IDrawable 
{
public:

    DeviceMesh(const char *filename);
    ~DeviceMesh();
    
    void          RenderImmediate(RenderContext &context);
    bool          IsVisible(const RenderContext &context) const;
    const AABox  &GetBounds() const;
    
    bool          SetProperty(const char *key, const char *value);

    void          Retain();
    void          Release();
    
private:
    
    bool          SetNormalAction(const char *action);
    
    OpenGLMeshType *mMesh;
    int             mRef;
};

#endif // _DEVICEMESH_H