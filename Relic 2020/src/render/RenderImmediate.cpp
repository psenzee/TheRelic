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
    GLLoadMatrixStack(
        context.camera.GetProjection(),
        context.camera.GetView(),
        context.transform
    );

    ClearCachedPointers();

    GLSetEnabledClientState(GL_NORMAL_ARRAY, false);
    GLSetEnabledClientState(GL_VERTEX_ARRAY, true);
    GLSetEnabledClientState(GL_TEXTURE_COORD_ARRAY, true);

    // Describe to OpenGL where the vertex and uv data is in the buffer
    GLSetVertexPointer((const float *)vertices, 20);
    GLSetTexCoordPointer((const float *)vertices + 3, 20);

    GLDrawArrays(GL_TRIANGLES, count);
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
