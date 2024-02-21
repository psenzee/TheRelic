#ifndef _DRAWABLEUTILS_H
#define _DRAWABLEUTILS_H

class Drawable;

Drawable *CreateRing(const char *renderTag,
                     const char *textureName,                      
                     int segments, float innerRadius, float outerRadius);

#endif // _DRAWABLEUTILS_H