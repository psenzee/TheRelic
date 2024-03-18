#include "GLAbstract.h"
#include "render/GLIncludes.h"
#include "render/glError.h"

#include "matrix4f.h"
#include "tuple4f.h"

void GLLoadMatrix(const Matrix4f &matrix)
{
#ifndef _OPENGLES_2
    _GLv(glLoadMatrixf((const float *)matrix.data));
#else
    
#endif
}

void GLMultMatrix(const Matrix4f &matrix)
{
#ifndef _OPENGLES_2
    _GLv(glMultMatrixf((const float *)matrix.data));
#else
    
#endif
}

void GLLoadTextureMatrix(const Matrix4f &matrix)
{
#ifndef _OPENGLES_2
    _GLv(glMatrixMode(GL_TEXTURE));
    _GLv(glLoadMatrixf((const float *)matrix.data));
    _GLv(glMatrixMode(GL_MODELVIEW)); // reset default matrix
#else
    
#endif
}

void GLLoadProjectionMatrix(const Matrix4f &matrix)
{
#ifndef _OPENGLES_2
    _GLv(glMatrixMode(GL_PROJECTION));
    _GLv(glLoadMatrixf((const float *)matrix.data));
    _GLv(glMatrixMode(GL_MODELVIEW)); // reset default matrix
#else
    
#endif
}

void GLLoadModelViewMatrix(const Matrix4f &matrix)
{
#ifndef _OPENGLES_2
    _GLv(glMatrixMode(GL_MODELVIEW));
    _GLv(glLoadMatrixf((const float *)matrix.data));
#else
    
#endif
}

void GLLoadModelViewProjectionMatrix(const Matrix4f &projection, const Matrix4f &modelView)
{
#ifndef _OPENGLES_2
    _GLv(glMatrixMode(GL_PROJECTION));
    _GLv(glLoadMatrixf((const float *)projection.data));
    _GLv(glMatrixMode(GL_MODELVIEW));
    _GLv(glLoadMatrixf((const float *)modelView.data));
#else
    
#endif
}

void GLLoadProjectionIdentityMatrix()
{
#ifndef _OPENGLES_2
    _GLv(glMatrixMode(GL_PROJECTION));
    _GLv(glLoadIdentity());
    _GLv(glMatrixMode(GL_MODELVIEW)); // reset default matrix
#else
    
#endif
}

void GLLoadModelViewIdentityMatrix()
{
#ifndef _OPENGLES_2
    _GLv(glMatrixMode(GL_MODELVIEW));
    _GLv(glLoadIdentity());
#else
    
#endif
}

void GLLoadIdentityMatrix()
{
#ifndef _OPENGLES_2
    _GLv(glLoadIdentity());
#else
    
#endif
}

void GLLoadBaseTextureMatrix()
{
#ifndef _OPENGLES_2
    _GLv(glMatrixMode(GL_TEXTURE));
    _GLv(glLoadIdentity());
    _GLv(glScalef(1.f, -1.f, 1.f));
    _GLv(glTranslatef(0.f, 1.f, 0.f));
#else
    
#endif
}

void GLSetActiveTexture(int value)
{
#ifndef _OPENGLES_2
    _GLv(glActiveTexture(value));
#else
    
#endif
}

void GLSetColorCombineMode(int combine) // combine GL_ADD/GL_MODULATE/GL_INTERPOLATE
{
#ifndef _OPENGLES_2
   /*
   glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB,  combine);
   glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB,  GL_PREVIOUS);
   glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_RGB,  GL_TEXTURE);
   glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_RGB, GL_SRC_COLOR);
   glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_RGB, GL_SRC_COLOR);
   */
#else
    
#endif
}

void GLSetAlphaCombineMode(int combine) // combine GL_ADD/GL_MODULATE/GL_INTERPOLATE
{
#ifndef _OPENGLES_2
   /*
   glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_ALPHA,  combine);
   glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_ALPHA,  GL_PREVIOUS);
   glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_ALPHA,  GL_TEXTURE);
   glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_ALPHA, GL_SRC_ALPHA);
   glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_ALPHA, GL_SRC_ALPHA);
   */
#else
    
#endif
}

void GLSetEnabled(int id, bool enabled)
{
    auto fn = (enabled ? glEnable : glDisable);
    _GLv(fn(id));
}

void GLSetEnabledClientState(int id, bool enabled)
{
#ifndef _OPENGLES_2
    auto fn = (enabled ? glEnableClientState : glDisableClientState);
    _GLv(fn(id));
#else
    
#endif
}

void GLSetNormalAction(bool use_normals, NormalAction action, bool enable)
{
#ifndef _OPENGLES_2
    if (!use_normals) {
        return;
    }
    // we want to cache these values somewhere..
    switch (action)
    {
    case NORMALIZE_ACTION_NONE: // FAST   if we're not scaling, do nothing here
        GLSetEnabled(GL_RESCALE_NORMAL, false);
        GLSetEnabled(GL_NORMALIZE,      false);
        break;
    case NORMALIZE_ACTION_RESCALE:   GLSetEnabled(GL_RESCALE_NORMAL, enable); break; // MEDIUM if we're scaling uniformly, use this
    case NORMALIZE_ACTION_NORMALIZE: GLSetEnabled(GL_NORMALIZE,      enable); break; // SLOW   if we're scaling non-uniformly, use this
    }
#else
    
#endif
}

void *GLMapBuffer()
{
    return _GL(glMapBufferOES(GL_ARRAY_BUFFER, GL_WRITE_ONLY_OES));
}

void GLUnmapBuffer()
{
    _GLv(glUnmapBufferOES(GL_ARRAY_BUFFER));
}

void GLClearAll()
{
    _GLv(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
}

void GLClearZBuffer()
{
    _GLv(glClear(GL_DEPTH_BUFFER_BIT));
}

std::array<unsigned, 2> GLCreateBuffers(const void *interleaved_data, const void *indices_data, int interleaved_size, int indices_size)
{
    // BEGIN CREATE BUFFERS
    // http://playcontrol.net/ewing/jibberjabber/opengl_vertex_buffer_object.html
    
    unsigned vb_id = 0, ib_id = 0;
    
    // allocate a new buffer
    _GLv(glGenBuffers(1, &vb_id));
    
    // bind the buffer object to use
    _GLv(glBindBuffer(GL_ARRAY_BUFFER, vb_id));
    
    // allocate enough space for the VBO
    _GLv(glBufferData(GL_ARRAY_BUFFER, interleaved_size, 0, GL_STATIC_DRAW));

    void *vbuffer = GLMapBuffer();
    // transfer the vertex data to the VBO
    memcpy(vbuffer, interleaved_data, interleaved_size);
    GLUnmapBuffer();

    _GLv(glBindBuffer(GL_ARRAY_BUFFER, 0));

    if (indices_data && indices_size) {
        // create index buffer
        _GLv(glGenBuffers(1, &ib_id));
        _GLv(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ib_id));
        // instead of glBufferSubData and glMapBuffer, we can directly supply the data in one-shot
        _GLv(glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_size, indices_data, GL_STATIC_DRAW));
        _GLv(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
    }

    return std::array<unsigned, 2> { vb_id, ib_id };
}

void GLSetLight(int id, int state, float value)
{
    _GLv(glLightf(id, state, value));
}

void GLSetLight4(int id, int state, const float *value)
{
    _GLv(glLightfv(id, state, value));
}

void GLSetMaterial(int state, float value)
{
    _GLv(glMaterialf(GL_FRONT_AND_BACK, state, value));
}

void GLSetMaterial4(int state, const float *value)
{
    _GLv(glMaterialfv(GL_FRONT_AND_BACK, state, value));
}

void GLSetMaterial4(int state, const Tuple4f &value)
{
    GLSetMaterial4(state, static_cast<const float *>(value.data()));
}

void GLSetDepthWrite(bool value)
{
    _GLv(glDepthMask(value ? GL_TRUE : GL_FALSE));
}

bool GLIsBlendAlphaType(int blend)
{
    switch (blend)
    {
    case GL_SRC_ALPHA:
    case GL_ONE_MINUS_SRC_ALPHA:
    case GL_DST_ALPHA:
    case GL_ONE_MINUS_DST_ALPHA:
    case GL_SRC_ALPHA_SATURATE:
#ifdef GL_CONSTANT_COLOR
    case GL_CONSTANT_ALPHA:
    case GL_ONE_MINUS_CONSTANT_ALPHA:
#endif
        return true;
    default: break;
    }
    return false;
}

void GLDestroyBuffer(unsigned id)
{
    if (id) {
        _GLv(glDeleteBuffers(1, &id));
    }
}

void GLDestroyBuffers(std::span<unsigned> ids)
{
    _GLv(glDeleteBuffers(ids.size(), ids.data()));
}

void GLDestroyTexture(unsigned id)
{
    if (id) {
        _GLv(glDeleteTextures(1, &id));
    }
}

void GLDestroyTextures(std::span<unsigned> ids)
{
    _GLv(glDeleteTextures(ids.size(), ids.data()));
}

void GLBlendFunc(int blend_src, int blend_dst)
{
    // Set a blending function to use
    _GLv(glBlendFunc(blend_src, blend_dst));
}

void GLBindTexture2d(int texid)
{
    _GLv(glBindTexture(GL_TEXTURE_2D, texid));
}

void GLSetAsTextureN(int id, int n)
{
    GLSetActiveTexture(GL_TEXTURE0 + n);
    GLBindTexture2d(id);
}

void GLDrawElements(int listStripOrFan, size_t indexCount)
{
    _GLv(glDrawElements(listStripOrFan, int(indexCount), GL_UNSIGNED_SHORT, 0));
}

void GLBindBufferForElements(unsigned vb, unsigned ib)
{
    // Activate the VBOs to draw
    _GLv(glBindBuffer(GL_ARRAY_BUFFER,         vb));
    _GLv(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ib));
}

void GLShadeModelSmooth()
{
    _GLv(glShadeModel(GL_SMOOTH));
}

void GLTextureSetMipMapFilters(int texid, bool value)
{
    GLBindTexture2d(texid);
    _GLv(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, value ? GL_LINEAR_MIPMAP_NEAREST : GL_LINEAR));
}

void GLSetVertexPointersInterleaved(bool has_normals)
{
    int size = 20 + (has_normals ? 12 : 0);
    // Describe to OpenGL where the vertex data is in the buffer
    _GLv(glVertexPointer  (3, GL_FLOAT, size, ((char*)NULL + 0)));
    // Describe to OpenGL where the uv data is in the buffer
    _GLv(glTexCoordPointer(2, GL_FLOAT, size, ((char*)NULL + 12)));
    // Describe to OpenGL where the normal data is in the buffer
    if (has_normals) {
        _GLv(glNormalPointer(GL_FLOAT, size, ((char*)NULL + 20)));
    }
}

void GLSetColor(const float *color)
{
    _GLv(glColor4f(color[0], color[1], color[2], color[2]));
}

void GLSetColor(const Tuple4f &color)
{
    GLSetColor(color.data());
}

void GLSetTextureEnvMode(int mode)
{
    _GLv(glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, mode));
}
