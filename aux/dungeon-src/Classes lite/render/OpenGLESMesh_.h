/*
 *  OpenGLESMesh.h
 *  GLGravity
 *
 *  Created by Paul Senzee on 12/24/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _OPENGLESMESH_H
#define _OPENGLESMESH_H

#include "core/xna.h"
#include "core/aabox.h"

#include <OpenGLES/ES1/glext.h>

class OpenGLESMesh
{
public:
    
    enum NormalAction { NONE, RESCALE, NORMALIZE };
    
    void           *buffer;
    float          *vertices,
                   *normals,
                   *uvs;
    unsigned short *indices;
    size_t          verticesCount,
                    normalsCount,
                    uvsCount,
                    indicesCount;
    AABox           bounds;
    NormalAction    normalAction;
        
    OpenGLESMesh();
    ~OpenGLESMesh();
        
    bool read(const char *filename);
    void clear();
    void render();
    
private:
    
    GLuint buffers[4];
};

#endif  // _OPENGLESMESH_H