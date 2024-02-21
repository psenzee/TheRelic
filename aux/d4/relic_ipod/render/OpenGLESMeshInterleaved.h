/*
 *  OpenGLESMeshInterleaved.h
 *  GLGravity
 *
 *  Created by Paul Senzee on 12/24/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _OPENGLESMESHINTERLEAVED_H
#define _OPENGLESMESHINTERLEAVED_H

#include "core/core.h"
#include "core/aabox.h"

/*
class GLVertex
{
    // corresponds to
    // GL_T2F_N3F_V3F format
public:
    Vector2 uv;
    Vector3 normal,
            position;
};
*/
class GLVertex
{
public:
    Vector3 position;
//  Vector3 normal;           
    Vector2 uv;
};

class GLVertexSmall
{
public:
    //char   x, y, z, w;
    short   x, y, z, w;
    Vector2 uv;
};

class OpenGLESMeshInterleaved
{
public:

    enum Type  { NONINTERLEAVED_LIST = 0, INTERLEAVED_LIST = 1, INTERLEAVED_STRIP = 2, _ = 0x7fffffff };

    static const bool USE_SHORTS_IF_POSSIBLE = false;

    enum NormalAction { NONE, RESCALE, NORMALIZE };
    
    void           *buffer;
    Type            type;
    GLVertex       *interleaved;
GLVertexSmall *retyped;
    unsigned short *indices;
    size_t          verticesCount,
                    indicesCount;
    AABox           bounds;
    NormalAction    normalAction;
        
    OpenGLESMeshInterleaved();
    ~OpenGLESMeshInterleaved();
        
    bool read(const char *filename);
    void clear();
    void render();

private:
    
    enum Usage { USE_NO_BUFFERS = 0, USE_BUFFERS, USE_BUFFERS_INTERLEAVED };
    
    static const Usage usage;        
    
    unsigned  vb, ib;
};

#endif  // _OPENGLESMESHINTERLEAVED_H