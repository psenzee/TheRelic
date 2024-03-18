#include "RenderImmediate.h"
#include "OverheadCamera.h"
#include "RenderContext.h"
#include "DeviceTexture.h"
#include "Texture.h"
#include "render/GLIncludes.h"
#include "GLAbstract.h"
#include "glError.h"

extern void ClearCachedPointers();

void RenderImmediate(const RenderContext   &context,
                     const ImmediateVertex *vertices,
                     int                    count)
{
    GLLoadMatrix(context.camera.GetView());
    GLMultMatrix(context.transform);

    ClearCachedPointers();

    GLSetEnabledClientState(GL_NORMAL_ARRAY, false);
    GLSetEnabledClientState(GL_VERTEX_ARRAY, true);
    GLSetEnabledClientState(GL_TEXTURE_COORD_ARRAY, true);

    // Describe to OpenGL where the vertex and uv data is in the buffer
    _GLv(glVertexPointer  (3, GL_FLOAT, 20, reinterpret_cast<const char *>(vertices) + 0));
    _GLv(glTexCoordPointer(2, GL_FLOAT, 20, reinterpret_cast<const char *>(vertices) + sizeof(float) * 3));

    _GLv(glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(count)));
}

void RenderImmediate(const RenderContext   &context,
                     DeviceTexture         *texture,
                     const ImmediateVertex *vertices,
                     int                    count,
                     unsigned               blendsrc,
                     unsigned               blenddst)
{
    if (texture) texture->Set(context.device, blendsrc, blenddst);
    RenderImmediate(context, vertices, count);
}

void RenderImmediate(const RenderContext   &context,
                     Texture               *texture,
                     const ImmediateVertex *vertices,
                     int                    count)
{
    if (texture) texture->Set(context.device);
    RenderImmediate(context, vertices, count);
}
