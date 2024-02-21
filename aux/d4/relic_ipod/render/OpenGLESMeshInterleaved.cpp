/*
 *  OpenGLESMeshInterleaved.cpp
 *  GLGravity
 *
 *  Created by Paul Senzee on 12/24/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include "OpenGLESMeshInterleaved.h"
#include "core/global.h"
#include "platform/GLUtils.h"
#include "platform/GLIncludes.h"

#include <stdio.h>
#include <string.h>

#ifdef WIN32
const OpenGLESMeshInterleaved::Usage OpenGLESMeshInterleaved::usage = OpenGLESMeshInterleaved::USE_NO_BUFFERS;
#else
const OpenGLESMeshInterleaved::Usage OpenGLESMeshInterleaved::usage = OpenGLESMeshInterleaved::USE_BUFFERS_INTERLEAVED;
#endif

static void *MapBuffer()
{
#ifndef WIN32
    return glMapBufferOES(GL_ARRAY_BUFFER, GL_WRITE_ONLY_OES);
#else
    return glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
#endif
}

static void UnmapBuffer()
{
#ifndef WIN32
    glUnmapBufferOES(GL_ARRAY_BUFFER);
#else
    glUnmapBuffer(GL_ARRAY_BUFFER);
#endif
}

OpenGLESMeshInterleaved::OpenGLESMeshInterleaved() : buffer(0), normalAction(NONE), vb(0), ib(0), interleaved(0)
{
    clear();
}

OpenGLESMeshInterleaved::~OpenGLESMeshInterleaved()
{
    clear();
}

void OpenGLESMeshInterleaved::clear()
{
    if (ib || vb)
    {
#ifndef WIN32
        glDeleteBuffers(1, &ib);
        glDeleteBuffers(1, &vb);
#endif
    }
    if (interleaved)
        delete [] (float *)interleaved;
    if (buffer)
        delete [] (char *)buffer;
    buffer = 0;
    indices = 0;
    verticesCount = indicesCount = 0;
}

inline char CastClamp(float x)
{
    if (x < -128.0f)
        x = -128;
    else if (x > 127)
        x = 127;
    return static_cast<char>(x);
}

bool OpenGLESMeshInterleaved::read(const char *filename)
{
    clear();
    FILE *file = fopen(globalTranslatePath(filename), "rb");
    if (!file)
    {
        globalErrorHandler("Unable to open file '%s'.", filename);
        return false;
    }    
    size_t sz = 0, totalsz = 0;
    if (!fread(&sz, sizeof(sz), 1, file) || sz == 0)
        return false;
    totalsz = sz;
    buffer = new char [sz];
    if (!fread(buffer, sz, 1, file))
    {
        fclose(file);
        delete [] (char *)buffer;
        buffer = 0;
        return false;
    }
    fclose(file);
    char *p = (char *)buffer;
    type = *(Type *)p;
    p += sizeof(Type);
    sz = *(size_t *)p;
    p += sizeof(size_t);    
    interleaved = (GLVertex *)p;
    p += sz;
    verticesCount = sz / sizeof(GLVertex);

retyped = new GLVertexSmall [verticesCount];
for (int i = 0; i < verticesCount; i++)
{
    retyped[i].x = static_cast<short>(interleaved[i].position.x);
    retyped[i].y = static_cast<short>(interleaved[i].position.y);
    retyped[i].z = static_cast<short>(interleaved[i].position.z);
    /*
    retyped[i].x = CastClamp(interleaved[i].position.x);
    retyped[i].y = CastClamp(interleaved[i].position.y);
    retyped[i].z = CastClamp(interleaved[i].position.z);
    */
    retyped[i].uv = interleaved[i].uv;
}

    for (int i = 0, cnt = (int)(sz / sizeof(GLVertex)); i < cnt; i++)
    {
        Vector3 v(interleaved[i].position);
        bounds.minimum = v.minimum(bounds.minimum);
        bounds.maximum = v.maximum(bounds.maximum);
    }
    size_t dataSize = sz;
    verticesCount = sz / sizeof(GLVertex);
    sz = *(size_t *)p;
    p += sizeof(size_t);
    indices = (unsigned short *)p;
    p += sz;
    size_t indicesSize = sz;
    indicesCount = sz / sizeof(unsigned short);
    if ((size_t)(p - (char *)buffer) != totalsz)
        return false;

#ifndef WIN32    
    if (usage == USE_BUFFERS_INTERLEAVED)
    {
        // BEGIN CREATE BUFFERS
        // http://playcontrol.net/ewing/jibberjabber/opengl_vertex_buffer_object.html
        
        // allocate a new buffer
        glGenBuffers(1, &vb);
        
        // bind the buffer object to use
        glBindBuffer(GL_ARRAY_BUFFER, vb);
        
        // allocate enough space for the VBO
        glBufferData(GL_ARRAY_BUFFER, dataSize, 0, GL_STATIC_DRAW);

        void *vbuffer = MapBuffer();

        // transfer the vertex data to the VBO
        memcpy(vbuffer, interleaved, dataSize);
        UnmapBuffer();
        
        // create index buffer
        glGenBuffers(1, &ib);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ib);
        // For constrast, instead of glBufferSubData and glMapBuffer, we can directly supply the data in one-shot
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesSize, indices, GL_STATIC_DRAW);
        
        // END CREATE BUFFERS 
        
        glBindBuffer(GL_ARRAY_BUFFER, 0); 
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        
        PrintGLError();        
    }
#endif

    return true;
}

void OpenGLESMeshInterleaved::render()
{
    switch (normalAction)
    {
    default:                                     break; // FAST   if we're not scaling, do nothing here
    case RESCALE:   glEnable(GL_RESCALE_NORMAL); break; // MEDIUM if we're scaling uniformly, use this
    case NORMALIZE: glEnable(GL_NORMALIZE);      break; // SLOW   if we're scaling non-uniformly, use this
    }
//    if (type == INTERLEAVED_STRIP)
//      glDisable(GL_CULL_FACE);

    glEnable(GL_CULL_FACE);
    if (usage == USE_NO_BUFFERS)
    {
        if (retyped && USE_SHORTS_IF_POSSIBLE)
            SetPointersInterleavedShortPos(retyped, static_cast<unsigned>(verticesCount));
        else
            SetPointersInterleaved(interleaved, static_cast<unsigned>(verticesCount));
        glDrawElements(type == INTERLEAVED_STRIP ? GL_TRIANGLE_STRIP : GL_TRIANGLES, static_cast<GLsizei>(indicesCount), GL_UNSIGNED_SHORT, indices);
    }
#ifndef WIN32
    else if (usage == USE_BUFFERS_INTERLEAVED)
    {
        SetBuffersInterleaved(vb, ib, verticesCount);       
        // This is the actual draw command
        glDrawElements(type == INTERLEAVED_STRIP ? GL_TRIANGLE_STRIP : GL_TRIANGLES, indicesCount, GL_UNSIGNED_SHORT, 0);
    }
#endif
//    if (type == INTERLEAVED_STRIP)
//        glEnable(GL_CULL_FACE);
    switch (normalAction)
    {
    default:                                      break; // FAST   if we're not scaling, do nothing here            
    case RESCALE:   glDisable(GL_RESCALE_NORMAL); break; // MEDIUM if we're scaling uniformly, use this        
    case NORMALIZE: glDisable(GL_NORMALIZE);      break; // SLOW   if we're scaling non-uniformly, use this 
    }
}