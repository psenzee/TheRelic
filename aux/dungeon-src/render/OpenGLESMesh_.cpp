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
#include <OpenGLES/ES1/glext.h>
#include "GLUtils.h"

#include <stdio.h>
#include <string.h>

OpenGLESMesh::OpenGLESMesh() : buffer(0), normalAction(NONE)
{
    memset(buffers, 0, sizeof(buffers));
    clear();
}

OpenGLESMesh::~OpenGLESMesh()
{
    clear();
}

void OpenGLESMesh::clear()
{
    /*
    for (int i = 0; i < 4; i++)
        if (glIsBuffer(buffers[i]))
            glDeleteBuffers(1, &buffers[i]);
    memset(buffers, 0, sizeof(buffers));            
     */
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
    for (int i = 0, cnt = sz / sizeof(float); i < cnt; i += 3)
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
/*
    glGenBuffers(4, buffers);
    PrintGLError();
    glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
    PrintGLError();    
    glBufferData(GL_ARRAY_BUFFER, verticesCount * 3 * sizeof(float), vertices, GL_STATIC_DRAW);
    PrintGLError();
*/    
    /*
    glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
    PrintGLError();    
    glBufferData(GL_ARRAY_BUFFER, verticesCount * 3 * sizeof(float), vertices, GL_STATIC_DRAW);
    PrintGLError();    
    glBindBuffer(GL_ARRAY_BUFFER, buffers[1]);
    PrintGLError();    
    glBufferData(GL_ARRAY_BUFFER, normalsCount * 3 * sizeof(float), normals, GL_STATIC_DRAW);
    PrintGLError();    
    glBindBuffer(GL_ARRAY_BUFFER, buffers[2]);
    PrintGLError();    
    glBufferData(GL_ARRAY_BUFFER, uvsCount * 2 * sizeof(float), uvs, GL_STATIC_DRAW);
    PrintGLError();    
    glBindBuffer(GL_ARRAY_BUFFER, buffers[3]);
    PrintGLError();    
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesCount * sizeof(unsigned short), indices, GL_STATIC_DRAW);
    PrintGLError();    
     */
    return true;
}

void OpenGLESMesh::render()
{
    switch (normalAction)
    {
    default:                                     break;    // FAST   if we're not scaling, do nothing here            
    case RESCALE:   glEnable(GL_RESCALE_NORMAL); break;    // MEDIUM if we're scaling uniformly, use this        
    case NORMALIZE: glEnable(GL_NORMALIZE);      break; // SLOW   if we're scaling non-uniformly, use this 
    }
    
    SetVertexPointer(vertices);
    SetNormalPointer(normals);
    SetUvPointer(uvs);
    
    glDrawElements(GL_TRIANGLES, indicesCount, GL_UNSIGNED_SHORT, indices);
    
    switch (normalAction)
    {
    default:                                      break; // FAST   if we're not scaling, do nothing here            
    case RESCALE:   glDisable(GL_RESCALE_NORMAL); break; // MEDIUM if we're scaling uniformly, use this        
    case NORMALIZE: glDisable(GL_NORMALIZE);      break; // SLOW   if we're scaling non-uniformly, use this 
    }
}