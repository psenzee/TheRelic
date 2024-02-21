#ifndef _LOCALGAMESERVICES_H
#define _LOCALGAMESERVICES_H

#include "core/xna.h"

class ContentCache;
class GraphicsDevice;

class LocalGameServices
{
public:    

    core::Size      frame;
    GraphicsDevice *device;
    ContentCache   *cache;

    LocalGameServices(const core::Size &frame, GraphicsDevice *device, ContentCache *cache)
        : frame(frame), device(device), cache(cache)
    {
    }
};

#endif // _LOCALGAMESERVICES_H