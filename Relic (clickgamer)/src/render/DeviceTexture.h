#ifndef _DEVICETEXTURE_H
#define _DEVICETEXTURE_H

#include "core/core.h"
#include "fast/Allocator.h"

class GraphicsDevice;
class OverheadCamera;

class DeviceTexture 
{
public:
    
    CLASS_NEW_DELETE()
    
    DeviceTexture(const char *filename);
    ~DeviceTexture();
    
    inline const core::Size &GetSize() const { return size; }
    
    bool Loaded() const;
    void SetMipMapped(bool value);
    void Set(GraphicsDevice &device, unsigned int blendsrc, unsigned int blenddst);

    unsigned GetId() const { return texid; }
    
private:
    
    volatile unsigned texid;
    core::Size        size;
};

#endif // _DEVICETEXTURE_H