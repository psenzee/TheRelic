#ifndef _GLINCLUDES_H
#define _GLINCLUDES_H

#ifdef WIN32
#define NOMINMAX 1
#include <windows.h>
#include <GL/gl.h>
#define GL_GLEXT_PROTOTYPES 1
#include "platform/Win32/glext.h"
#else
#include <OpenGLES/ES1/glext.h>
#endif

#endif // _GLINCLUDES_H