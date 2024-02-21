/*
 */

#ifndef _DEVICEMODEL_H
#define _DEVICEMODEL_H

#include "core/xna.h"
#include "core/aabox.h"

class GraphicsDevice;
class OverheadCamera;
class OpenGLESMesh;

class DeviceModel 
{
public:

    DeviceModel(const char *filename);
    ~DeviceModel();
    
    void          Render(GraphicsDevice &device, const Matrix &transform, const OverheadCamera &camera);
    bool          IsVisible(const Matrix &transform, const OverheadCamera &camera) const;
    const AABox  &GetBounds() const;
    
    bool          SetProperty(const char *key, const char *value);
    
private:
    
    bool          SetNormalAction(const char *action);    
    
    OpenGLESMesh *mesh;    
};

#endif // _DEVICEMODEL_H