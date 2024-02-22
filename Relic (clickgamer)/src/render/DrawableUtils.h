#ifndef _DRAWABLEUTILS_H
#define _DRAWABLEUTILS_H

#include "core/core.h"

class Drawable;

Drawable *CreateRing    (const char *renderTag,
                         const char *textureName,                      
                         int segments, float innerRadius, float outerRadius, float z);
Drawable *CreateRingTransform(const char *renderTag,
                         const char *textureName,                      
                         int segments, float innerRadius, float outerRadius, const Matrix &transform);
Drawable *CreateCylinder(const char *renderTag,
                         const char *textureName,                      
                         int segments, float radius, float height, float heightUv);

#endif // _DRAWABLEUTILS_H