#pragma once

#include <array>
#include <span>
#include "GLIncludes.h"
#include "glError.h"

//#define _OPENGLES_2
#define GLES_SILENCE_DEPRECATION 1

class Matrix4f;
class Tuple4f;
class VertexOptions;

enum NormalAction { NORMALIZE_ACTION_NONE = 0, NORMALIZE_ACTION_RESCALE, NORMALIZE_ACTION_NORMALIZE };

void                     GLLoadMatrix(const Matrix4f &matrix);
void                     GLMultMatrix(const Matrix4f &matrix);
void                     GLLoadModelViewMatrix(const Matrix4f &matrix);
void                     GLLoadIdentityMatrix();
void                     GLLoadMatrixStack(const Matrix4f &projection, const Matrix4f &view, const Matrix4f &model);
void                     GLLoadTextureMatrix(const Matrix4f &matrix);
void                     GLLoadProjectionMatrix(const Matrix4f &matrix);
void                     GLLoadProjectionIdentityMatrix();
void                     GLLoadModelViewIdentityMatrix();
void                     GLLoadModelViewProjectionMatrix(const Matrix4f &projection, const Matrix4f &modelView);
void                     GLLoadBaseTextureMatrix();
void                     GLSetActiveTexture(int value);
void                     GLSetAsTextureN(int id, int n);
void                     GLSetColorCombineMode(int combine);
void                     GLSetAlphaCombineMode(int combine);
void                     GLSetEnabled(int id, bool enabled);
void                     GLSetEnabledClientState(int id, bool enabled);
std::array<unsigned, 2>  GLGenerateBuffers(bool has_indices);
std::array<unsigned, 2>  GLCreateBuffers(const void *interleaved_data, const void *indices_data, size_t interleaved_size, size_t indices_size);
void                    *GLMapBuffer();
void                     GLUnmapBuffer();
void                     GLSetNormalAction(bool use_normals, NormalAction action, bool enable);
void                     GLClearAll();
void                     GLClearZBuffer();
void                     GLSetLight(int id, int state, float value);
void                     GLSetLight4(int id, int state, const float *value);
void                     GLSetMaterial(int state, float value);
void                     GLSetMaterial4(int state, const float *value);
void                     GLSetMaterial4(int state, const Tuple4f &value);
bool                     GLIsBlendAlphaType(int blend);
void                     GLDestroyBuffer(unsigned id);
void                     GLDestroyBuffers(std::span<unsigned> ids);
void                     GLDestroyTexture(unsigned id);
void                     GLDestroyTextures(std::span<unsigned> ids);
void                     GLSetDepthWrite(bool value);
void                     GLBlendFunc(int blend_src, int blend_dst);
void                     GLBindTexture2d(int texid);
void                     GLDrawElements(int listStripOrFan, size_t indexCount, int sizeOfIndexType = sizeof(uint16_t));
void                     GLBindBufferForElements(unsigned vb, unsigned ib);
void                     GLShadeModelSmooth();
void                     GLTextureSetMipMapFilters(int texid, bool value);
void                     GLSetVertexPointersInterleaved(bool has_normals);
void                     GLSetColor(const float *color);
void                     GLSetColor(const Tuple4f &color);
void                     GLSetTextureEnvMode(int mode);
void                     GLDrawArrays(int type, size_t count);
void                     GLSetVertexPointer(const float *vertices, size_t stride = 0);
void                     GLSetTexCoordPointer(const float *uvs, size_t stride = 0);
bool                     GLIsTexture(int texid);
void                     GLCopyIntoBuffer(int buffer_id, int type, const void *data, size_t bytes_count, int usage = GL_STATIC_DRAW);
void                     GLRenderQuads(const float *vertices, const float *uvs, size_t count, const Tuple4f &color);
void                     GLConfigureVertexInterleavedOptions(const VertexOptions &options);

template <typename Vertex>
void GLConfigureVertexInterleaved()
{
    const char *base     = (const char *)nullptr,
               *position = base + Vertex::OFFSET_POSITION,
               *texture  = base + Vertex::OFFSET_TEXTURE,
               *normal   = base + Vertex::OFFSET_NORMAL;
    size_t stride = sizeof(Vertex);
#ifndef _OPENGLES_2

    // This could actually be moved into the setup since we never disable it
    GLSetEnabledClientState(GL_VERTEX_ARRAY, true);
    GLSetEnabledClientState(GL_TEXTURE_COORD_ARRAY, true);
    
    // Describe to OpenGL where the vertex data is in the buffer
    _GLv(glVertexPointer(3, GL_FLOAT, stride, position));
    // Describe to OpenGL where the uv data is in the buffer
    _GLv(glTexCoordPointer(2, GL_FLOAT, stride, texture));
    // Describe to OpenGL where the normal data is in the buffer
    if (Vertex::OFFSET_NORMAL > 0) {
        GLSetEnabledClientState(GL_NORMAL_ARRAY, true);
        _GLv(glNormalPointer(GL_FLOAT, stride, normal));
    } else {
        GLSetEnabledClientState(GL_NORMAL_ARRAY, false);
    }
#else
    int count = (Vertex::OFFSET_NORMAL > 0) ? 3 : 2;
    for (int i = 0; i < count; i++) {
        _GLv(glEnableVertexAttribArray(i));
    }
    // Describe to OpenGL where the vertex data is in the buffer
    _GLv(glVertexAttribPointer(0, 3, GL_FLOAT, stride, GL_FALSE, position));
    // Describe to OpenGL where the uv data is in the buffer
    _GLv(glVertexAttribPointer(1, 2, GL_FLOAT, stride, GL_FALSE, texture));
    // Describe to OpenGL where the normal data is in the buffer
    if (Vertex::OFFSET_NORMAL > 0) {
        _GLv(glVertexAttribPointer(2, 3, GL_FLOAT, stride, GL_FALSE, normal));
    }
#endif
}
