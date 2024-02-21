/*
 *  GLUtils.h
 *  GLGravity
 *
 *  Created by Paul Senzee on 12/26/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _GLUTILS_H
#define _GLUTILS_H

#include "core/xna.h"
#include <OpenGLES/ES1/glext.h>

void   ClearCachedPointers();

void   SetVertexPointer(void *data);
void   SetNormalPointer(void *data);
void   SetUvPointer(void *data);
void   SetBuffers(unsigned vb, unsigned ib, unsigned vertices);
void   SetBuffersInterleaved(unsigned vb, unsigned ib, unsigned vertices);

GLuint LoadTexture(const char *filename, core::Size *size = 0);

void   Print(const Vector3 &v);
void   Print(const char *label, const Vector3 &v);
void   Print(const char *label, const Matrix &v);

void   PrintGLError();

#endif // _GLUTILS_H