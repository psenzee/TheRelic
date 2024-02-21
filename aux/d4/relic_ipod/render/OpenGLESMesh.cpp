/*
 *  OpenGLESMesh.cpp
 *  GLGravity
 *
 *  Created by Paul Senzee on 12/24/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include "OpenGLESMesh.h"
#include "core/global.h"
#include "platform/GLUtils.h"
#include "platform/GLIncludes.h"

#include <stdio.h>
#include <string.h>

#ifdef WIN32
const OpenGLESMesh::Usage OpenGLESMesh::usage = OpenGLESMesh::USE_NO_BUFFERS;
#else
const OpenGLESMesh::Usage OpenGLESMesh::usage = OpenGLESMesh::USE_BUFFERS_INTERLEAVED;
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

OpenGLESMesh::OpenGLESMesh() : buffer(0), normalAction(NONE), vb(0), ib(0), interleaved(0)
{
    clear();
}

OpenGLESMesh::~OpenGLESMesh()
{
    clear();
}

void OpenGLESMesh::clear()
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
    vertices = normals = 0;
    uvs = 0;
    indices = 0;
    verticesCount = normalsCount = uvsCount = indicesCount = 0;
}

bool OpenGLESMesh::read(const char *filename)
{
    clear();
    FILE *file = fopen(globalTranslatePath(filename), "rb");
    if (!file)
    {
        globalErrorHandler("Unable to open file '%s'.", filename);
        return false;
    }    
    if (!file)
        return false;
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
    sz = *(size_t *)p;
    p += sizeof(size_t);
    vertices = (float *)p;
    p += sz;
    verticesCount = sz / (3 * sizeof(float));
    for (int i = 0, cnt = (int)(sz / sizeof(float)); i < cnt; i += 3)
    {
        Vector3 v(vertices[i + 0], vertices[i + 1], vertices[i + 2]);
        bounds.minimum = v.minimum(bounds.minimum);
        bounds.maximum = v.maximum(bounds.maximum);            
    }
    sz = *(size_t *)p;
    p += sizeof(size_t);
    normals = (float *)p;
    p += sz;
    normalsCount = sz / (3 * sizeof(float));
    sz = *(size_t *)p;
    p += sizeof(size_t);
    uvs = (float *)p;
    p += sz;
    uvsCount = sz / (2 * sizeof(float));
    sz = *(size_t *)p;
    p += sizeof(size_t);
    indices = (unsigned short *)p;
    p += sz;
    indicesCount = sz / sizeof(unsigned short);
    if ((size_t)(p - (char *)buffer) != totalsz)
        return false;
    
    const GLsizeiptr vertexSize = verticesCount * 3 * sizeof(float);
    const GLsizeiptr uvSize     = uvsCount      * 2 * sizeof(float);
    const GLsizeiptr normalSize = normalsCount  * 3 * sizeof(float);    
#ifndef WIN32    
    if (usage == USE_BUFFERS_INTERLEAVED)
    {
        interleaved = new float [totalsz / sizeof(float)];
    
        float *pi          = interleaved,
              *pv          = vertices,
              *pt          = uvs,
              *pn          = normals;
    
        for (int i = 0; i < (int)verticesCount; i++)
        {
            // GL_T2F_N3F_V3F
            *pi++ = *pt++; *pi++ = *pt++;
            *pi++ = *pn++; *pi++ = *pn++; *pi++ = *pn++;
            *pi++ = *pv++; *pi++ = *pv++; *pi++ = *pv++;
        }
        // BEGIN CREATE BUFFERS
        // http://playcontrol.net/ewing/jibberjabber/opengl_vertex_buffer_object.html
        
        // allocate a new buffer
        glGenBuffers(1, &vb);
        
        // bind the buffer object to use
        glBindBuffer(GL_ARRAY_BUFFER, vb);
        
        // allocate enough space for the VBO
        glBufferData(GL_ARRAY_BUFFER, vertexSize + uvSize + normalSize, 0, GL_STATIC_DRAW);

        void *vbuffer = MapBuffer();

        // transfer the vertex data to the VBO
        memcpy(vbuffer, interleaved, vertexSize + uvSize + normalSize);
        UnmapBuffer();
        
        // create index buffer
        glGenBuffers(1, &ib);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ib);
        // For constrast, instead of glBufferSubData and glMapBuffer, we can directly supply the data in one-shot
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesCount * sizeof(unsigned short), indices, GL_STATIC_DRAW);
        
        // END CREATE BUFFERS 
        
        glBindBuffer(GL_ARRAY_BUFFER, 0); 
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        
        PrintGLError();        
    }

    if (usage == USE_BUFFERS)
    {
        // BEGIN CREATE BUFFERS
        // http://playcontrol.net/ewing/jibberjabber/opengl_vertex_buffer_object.html

        // allocate a new buffer
        glGenBuffers(1, &vb);

        // bind the buffer object to use
        glBindBuffer(GL_ARRAY_BUFFER, vb);

        // allocate enough space for the VBO
        glBufferData(GL_ARRAY_BUFFER, vertexSize + uvSize + normalSize, 0, GL_STATIC_DRAW);

        void *vbuffer = MapBuffer(); 
        // transfer the vertex data to the VBO
        memcpy(vbuffer, vertices, vertexSize);
        // append color data to vertex data. To be optimal, data should probably be interleaved and not appended
        memcpy((char *)vbuffer + vertexSize,          uvs,     uvSize);
        memcpy((char *)vbuffer + vertexSize + uvSize, normals, normalSize);    
        UnmapBuffer();

        // create index buffer
        glGenBuffers(1, &ib);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ib);
        // For constrast, instead of glBufferSubData and glMapBuffer, we can directly supply the data in one-shot
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesCount * sizeof(unsigned short), indices, GL_STATIC_DRAW);
 
        // END CREATE BUFFERS 

        glBindBuffer(GL_ARRAY_BUFFER, 0); 
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
 
        PrintGLError();
    }
#endif

    return true;
}

void OpenGLESMesh::render()
{
    switch (normalAction)
    {
    default:                                     break; // FAST   if we're not scaling, do nothing here
    case RESCALE:   glEnable(GL_RESCALE_NORMAL); break; // MEDIUM if we're scaling uniformly, use this
    case NORMALIZE: glEnable(GL_NORMALIZE);      break; // SLOW   if we're scaling non-uniformly, use this
    }
    if (usage == USE_NO_BUFFERS)
    {
        // if we're not using buffers
        SetVertexPointer(vertices);
        SetNormalPointer(normals);
        SetUvPointer(uvs);
    
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indicesCount), GL_UNSIGNED_SHORT, indices);
    }
#ifndef WIN32
    else if (usage == USE_BUFFERS_INTERLEAVED)
    {
        SetBuffersInterleaved(vb, ib, verticesCount);
        
        // This is the actual draw command
        glDrawElements(GL_TRIANGLES, indicesCount, GL_UNSIGNED_SHORT, 0);
    }    
    else if (usage == USE_BUFFERS)
    {
        /*
        // Activate the VBOs to draw
        glBindBuffer(GL_ARRAY_BUFFER,         vb);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ib);
     
        const GLsizeiptr vertexSize = verticesCount * 3 * sizeof(float);
        const GLsizeiptr uvSize     = uvsCount      * 2 * sizeof(float);
        const GLsizeiptr normalSize = normalsCount  * 3 * sizeof(float);      
     
         // Describe to OpenGL where the vertex data is in the buffer
        glVertexPointer(3, GL_FLOAT, 0, (GLvoid*)((char*)NULL));
         // Describe to OpenGL where the uv data is in the buffer
        glTexCoordPointer(2, GL_FLOAT, 0, (GLvoid*)((char*)NULL + vertexSize));    
         // Describe to OpenGL where the normal data is in the buffer
        glNormalPointer(GL_FLOAT, 0, (GLvoid*)((char*)NULL + vertexSize + uvSize));
     
        // This could actually be moved into the setup since we never disable it
        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_NORMAL_ARRAY);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        */
        SetBuffers(vb, ib, verticesCount);
        
        // This is the actual draw command
        glDrawElements(GL_TRIANGLES, indicesCount, GL_UNSIGNED_SHORT, 0);
    
        //glBindBuffer(GL_ARRAY_BUFFER, 0); 
        //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }
#endif
        
    switch (normalAction)
    {
    default:                                      break; // FAST   if we're not scaling, do nothing here            
    case RESCALE:   glDisable(GL_RESCALE_NORMAL); break; // MEDIUM if we're scaling uniformly, use this        
    case NORMALIZE: glDisable(GL_NORMALIZE);      break; // SLOW   if we're scaling non-uniformly, use this 
    }
}