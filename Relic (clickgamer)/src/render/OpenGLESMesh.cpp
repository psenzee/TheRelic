#include "OpenGLESMesh.h"
#include "core/global.h"
#include "render/GLUtils.h"
#include "platform/GLIncludes.h"

#include <stdio.h>
#include <string.h>

#ifdef WIN32
const OpenGLESMesh::Usage OpenGLESMesh::usage = OpenGLESMesh::USE_NO_BUFFERS;
#else
const OpenGLESMesh::Usage OpenGLESMesh::usage = OpenGLESMesh::USE_BUFFERS;
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

OpenGLESMesh::OpenGLESMesh() : mData(0), mNormalAction(NONE), mVb(0), mIb(0), mInterleaved(0), mCompact(0), mOwner(false)
{
    Clear();
}

OpenGLESMesh::~OpenGLESMesh()
{
    Clear();
}

void OpenGLESMesh::Clear()
{
    DestroyBuffers();
    mInterleaved = 0; // mInterleaved points into mData, so it's not deletable
    if (mOwner && mData)
        delete [] (char *)mData;
    if (mCompact)
        delete [] (char *)mCompact;
    mData = 0;
    mIndices = 0;
    mCompact = 0;
    mVerticesCount = mIndicesCount = 0;
    mOwner = false;
}

void OpenGLESMesh::Compact()
{
    mCompact = new GLVertexSmall [mVerticesCount];
    for (int i = 0; i < mVerticesCount; i++)
    {
        mCompact[i].x = static_cast<short>(mInterleaved[i].position.x);
        mCompact[i].y = static_cast<short>(mInterleaved[i].position.y);
        mCompact[i].z = static_cast<short>(mInterleaved[i].position.z);
        mCompact[i].uv = mInterleaved[i].uv;
    }
}

void OpenGLESMesh::CalculateBounds()
{
    mBounds = AABox();
    for (int i = 0; i < mVerticesCount; i++)
    {
        Vector3 v(mInterleaved[i].position);
        mBounds.minimum = v.minimum(mBounds.minimum);
        mBounds.maximum = v.maximum(mBounds.maximum);
    }
}

bool OpenGLESMesh::Read(const char *filename, bool asCompact)
{
    Clear();
    FILE *file = fopen(globalTranslatePath(filename), "rb");
    if (!file)
    {
   //   globalErrorHandler("Unable to open file '%s'.", filename);
        return false;
    }    
    size_t sz = 0, totalsz = 0;
    if (!fread(&sz, sizeof(sz), 1, file) || sz == 0)
        return false;
    totalsz = sz;
    mData = new char [sz];
    if (!fread(mData, sz, 1, file))
    {
        fclose(file);
        delete [] (char *)mData;
        mData = 0;
        return false;
    }
    mOwner = true;
    fclose(file);
    char *p = (char *)mData;
    mType = *(Type *)p;
    p += sizeof(Type);
    sz = *(size_t *)p;
    p += sizeof(size_t);    
    mInterleaved = (GLVertex *)p;
    p += sz;
    mVerticesCount = sz / sizeof(GLVertex);
    
    if (asCompact)
        Compact();
    CalculateBounds();

    size_t dataSize = sz;
    mVerticesCount = sz / sizeof(GLVertex);
    sz = *(size_t *)p;
    p += sizeof(size_t);
    mIndices = (unsigned short *)p;
    p += sz;
    size_t indicesSize = sz;
    mIndicesCount = sz / sizeof(unsigned short);
    if ((size_t)(p - (char *)mData) != totalsz)
        return false;

    CreateBuffers(dataSize, indicesSize);
    return true;
}

void OpenGLESMesh::CreateBuffers(int dataSize, int indicesSize)
{
#ifndef WIN32    
    if (usage == USE_BUFFERS)
    {
        // BEGIN CREATE BUFFERS
        // http://playcontrol.net/ewing/jibberjabber/opengl_vertex_buffer_object.html
        
        // allocate a new buffer
        glGenBuffers(1, &mVb);
        
        // bind the buffer object to use
        glBindBuffer(GL_ARRAY_BUFFER, mVb);
        
        // allocate enough space for the VBO
        glBufferData(GL_ARRAY_BUFFER, dataSize, 0, GL_STATIC_DRAW);

        void *vbuffer = MapBuffer();

        // transfer the vertex data to the VBO
        memcpy(vbuffer, mInterleaved, dataSize);
        UnmapBuffer();
        
        // create index buffer
        glGenBuffers(1, &mIb);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIb);
        // For constrast, instead of glBufferSubData and glMapBuffer, we can directly supply the data in one-shot
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesSize, mIndices, GL_STATIC_DRAW);
        
        // END CREATE BUFFERS 
        
        glBindBuffer(GL_ARRAY_BUFFER, 0); 
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        
        PrintGLError();        
    }
#endif
}

void OpenGLESMesh::DestroyBuffers()
{
    if (mIb || mVb)
    {
#ifndef WIN32
        glDeleteBuffers(1, &mIb);
        glDeleteBuffers(1, &mVb);
#endif
    }
    mIb = mVb = 0;
}

bool OpenGLESMesh::ReadFromData(const char *data, int size, bool asCompact)
{
    Clear();
    size_t sz = (*(const int *)data), totalsz = 0;
    totalsz = sz;
    if (!data || !sz || size != sz + sizeof(int))
    {
        mData = 0;
        return false;
    }
    data += sizeof(int);
    mData = (void *)data;
    mOwner = false;
    char *p = (char *)mData;
    mType = *(Type *)p;
    p += sizeof(Type);
    sz = *(size_t *)p;
    p += sizeof(size_t);    
    mInterleaved = (GLVertex *)p;
    p += sz;
    mVerticesCount = sz / sizeof(GLVertex);
    
    if (asCompact)
        Compact();
    CalculateBounds();
    
    size_t dataSize = sz;
    mVerticesCount = sz / sizeof(GLVertex);
    sz = *(size_t *)p;
    p += sizeof(size_t);
    mIndices = (unsigned short *)p;
    p += sz;
    size_t indicesSize = sz;
    mIndicesCount = sz / sizeof(unsigned short);
    if ((size_t)(p - (char *)mData) != totalsz)
        return false;

    CreateBuffers(dataSize, indicesSize);
    return true;
}

void OpenGLESMesh::Render()
{
    bool useNormals = (mCompact == 0); // we don't use normals for compact data
#ifndef NORMALS
    useNormals = false;
#endif
    //glEnable(GL_RESCALE_NORMAL); // $HACK testing    
    if (useNormals)
    {
        // we want to cache these values somewhere..    
        switch (mNormalAction)
        {
        default:                                     break; // FAST   if we're not scaling, do nothing here
        case RESCALE:   glEnable(GL_RESCALE_NORMAL); break; // MEDIUM if we're scaling uniformly, use this
        case NORMALIZE: glEnable(GL_NORMALIZE);      break; // SLOW   if we're scaling non-uniformly, use this
        }
    }
    //glEnable(GL_CULL_FACE);
    if (usage == USE_NO_BUFFERS)
    {
        if (mCompact)
            SetPointersInterleavedShortPos(mCompact, static_cast<unsigned>(mVerticesCount), false);
        else
            SetPointersInterleaved(mInterleaved, static_cast<unsigned>(mVerticesCount), useNormals);
        glDrawElements(mType == INTERLEAVED_STRIP ? GL_TRIANGLE_STRIP : GL_TRIANGLES, static_cast<GLsizei>(mIndicesCount), GL_UNSIGNED_SHORT, mIndices);
    }
#ifndef WIN32
    else if (usage == USE_BUFFERS)
    {
        if (mCompact)
            SetBuffersInterleavedShortPos(mVb, mIb, static_cast<unsigned>(mVerticesCount), false);
        else
            SetBuffersInterleaved(mVb, mIb, static_cast<unsigned>(mVerticesCount), useNormals);
        // This is the actual draw command
        glDrawElements(mType == INTERLEAVED_STRIP ? GL_TRIANGLE_STRIP : GL_TRIANGLES, mIndicesCount, GL_UNSIGNED_SHORT, 0);
    }
#endif

    if (useNormals)
    {
        // we want to cache these values somewhere..
        switch (mNormalAction)
        {
        default:                                      break; // FAST   if we're not scaling, do nothing here            
        case RESCALE:   glDisable(GL_RESCALE_NORMAL); break; // MEDIUM if we're scaling uniformly, use this        
        case NORMALIZE: glDisable(GL_NORMALIZE);      break; // SLOW   if we're scaling non-uniformly, use this 
        }
    }
}