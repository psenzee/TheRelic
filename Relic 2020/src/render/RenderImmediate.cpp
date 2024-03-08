#include "RenderImmediate.h"
#include "OverheadCamera.h"
#include "RenderContext.h"
#include "DeviceTexture.h"
#include "Texture.h"
#include "platform/GLIncludes.h"

extern void ClearCachedPointers();

void RenderImmediate(const RenderContext   &context,
                     const ImmediateVertex *vertices,
                     int                    count)
{
    //glLoadMatrixf((GLfloat *)context.camera.GetView().data);
    //glMultMatrixf((GLfloat *)context.transform.data);
    auto program = context.device.SetShaderProgram("Shader");
    program->SetActive();
    program->SetUniform("u_transform", m);

    ClearCachedPointers();

    //glDisableClientState(GL_NORMAL_ARRAY);
    //glEnableClientState(GL_VERTEX_ARRAY);
    //glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    // Describe to OpenGL where the vertex and uv data is in the buffer
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 20, reinterpret_cast<const void *>(vertices));
    glEnableVertexAttribArray(0);
    //glTexCoordPointer(2, GL_FLOAT, 20, reinterpret_cast<const char *>(vertices) + sizeof(float) * 3);

    glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(count));
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
