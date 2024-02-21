#ifndef _GRAPHICSDEVICE_H
#define _GRAPHICSDEVICE_H

#include "core/xna.h"

class GraphicsDevice
{
public:

    void EnableDepthTest(bool v);
    void EnableDepthWrite(bool v);
    void EnableLighting(bool v);

    void SetProjection(const Matrix &m);
};

#endif // _GRAPHICSDEVICE_H
