#ifndef _GLUTILS_H
#define _GLUTILS_H

#include "core/core.h"

void     SetDefaultLightingType(int type);
int      GetDefaultLightingType();
void     SetDefaultLighting();
void     PushLightingType();
void     PopLightingType();

// these are deprecated, currently used only for UI and ParticleEffects
// which need to be migrated to SetPointersInterleaved*
void     ClearCachedPointers();
//void     SetVertexPointer(void *data);
//void     SetNormalPointer(void *data);
//void     SetUvPointer(void *data);

// these are our main rendering functions
void     SetBuffersInterleaved(unsigned vb, unsigned ib, unsigned vertices, bool normals);

void     Print(const Vector3 &v);
void     Print(const char *label, const Vector3 &v);
void     Print(const char *label, const Matrix &v);

#endif // _GLUTILS_H
