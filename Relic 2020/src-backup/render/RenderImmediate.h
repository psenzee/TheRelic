#ifndef _RENDERIMMEDIATE_H
#define _RENDERIMMEDIATE_H

#include "platform/GLIncludes.h"

class RenderContext;
class DeviceTexture;
class Texture;

struct ImmediateVertex
{
    float x, y, z, u, v;
};

void RenderImmediate(const RenderContext   &context,
                     const ImmediateVertex *vertices,
                     int                    count);

void RenderImmediate(const RenderContext   &context,
                     DeviceTexture         *texture,
                     const ImmediateVertex *vertices,
                     int                    count,
                     unsigned               blendsrc = GL_SRC_ALPHA,
                     unsigned               blenddst = GL_ONE_MINUS_SRC_ALPHA);

void RenderImmediate(const RenderContext   &context,
                     Texture               *texture,
                     const ImmediateVertex *vertices,
                     int                    count);

#endif // _RENDERIMMEDIATE_H