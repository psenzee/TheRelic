#ifndef _GLUTILS_H
#define _GLUTILS_H

#include "core/core.h"

void _oglError(const char *function, const char *file, int line);

template <typename T>
inline T gl_error(T return_value, const char *function, const char *file, int line)
{
    _oglError(function, file, line);
    return return_value;
}

#define _GL(x) gl_error((x), #x, __FILE__, __LINE__) // (x); _oglError(#x, __FILE__, __LINE__);

void     SetDefaultLightingType(int type);
int      GetDefaultLightingType();
void     SetDefaultLighting();
void     PushLightingType();
void     PopLightingType();

void     ClearZBuffer();
void     ClearAll();

// these are deprecated, currently used only for UI and ParticleEffects
// which need to be migrated to SetPointersInterleaved*
void     ClearCachedPointers();
void     SetVertexPointer(void *data);
void     SetNormalPointer(void *data);
void     SetUvPointer(void *data);

void     SetActiveTexture(int value);
void     SetAsTexture0(int texid);
void     SetAsTexture1(int texid);
void     SetColorCombineMode(int combine); // combine GL_ADD/GL_MODULATE/GL_INTERPOLATE
void     SetAlphaCombineMode(int combine); // combine GL_ADD/GL_MODULATE/GL_INTERPOLATE

// not used?
void     SetBuffers(unsigned vb, unsigned ib, unsigned vertices);

// these are our main rendering functions
void     SetBuffersInterleaved(unsigned vb, unsigned ib, unsigned vertices, bool normals);
void     SetBuffersInterleavedShortPos(unsigned vb, unsigned ib, unsigned vertices, bool normals);
void     SetBuffersInterleavedCharPos(unsigned vb, unsigned ib, unsigned vertices, bool normals);
void     SetPointersInterleaved(void *data, unsigned vertices, bool normals);
void     SetPointersInterleavedShortPos(void *data, unsigned vertices, bool normals);
void     SetPointersInterleavedCharPos(void *data, unsigned vertices, bool normals);

void     Print(const Vector3 &v);
void     Print(const char *label, const Vector3 &v);
void     Print(const char *label, const Matrix &v);

void     PrintGLError();

#endif // _GLUTILS_H
