#ifndef _LOCALGAMESERVICES_H
#define _LOCALGAMESERVICES_H

#include "core/core.h"

class ContentLoader;
class GraphicsDevice;

class LocalGameServices
{
public:    

    core::Size      frame;
    GraphicsDevice *device;
    ContentLoader  *loader;

    inline LocalGameServices(const core::Size &frame, GraphicsDevice *device, ContentLoader *loader)
        : frame(frame), device(device), loader(loader) {}
};

#endif // _LOCALGAMESERVICES_H
