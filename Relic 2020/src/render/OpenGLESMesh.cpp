#include "OpenGLESMesh.h"
#include "core/global.h"
#include "render/GLUtils.h"
#include "render/GLIncludes.h"
#include "GLAbstract.h"
#include "glError.h"
#include "CommonVertex.h"

#include <stdio.h>
#include <string.h>

OpenGLESMesh::OpenGLESMesh() : mData(0), mNormalAction(NORMALIZE_ACTION_NONE), mInterleaved(0), mOwner(false), mBuffer(options<uint16_t>(CommonVertex(), GL_TRIANGLE_STRIP))
{
    Clear();
}

OpenGLESMesh::~OpenGLESMesh()
{
    Clear();
}

void OpenGLESMesh::Clear()
{
    mInterleaved = 0; // mInterleaved points into mData, so it's not deletable
    if (mOwner && mData) {
        delete [] (char *)mData;
    }
    mData = 0;
    mIndices = 0;
    mVerticesCount = mIndicesCount = 0;
    mOwner = false;
}

void OpenGLESMesh::CalculateBounds()
{
    mBounds = AABox();
    for (int i = 0; i < mVerticesCount; i++) {
        Vector3 v(mInterleaved[i].position);
        mBounds.minimum = v.minimum(mBounds.minimum);
        mBounds.maximum = v.maximum(mBounds.maximum);
    }
}

bool OpenGLESMesh::Read(const char *filename)
{
    Clear();
    FILE *file = fopen(globalTranslatePath(filename), "rb");
    if (!file) {
        return false;
    }    
    uint32_t sz = 0, totalsz = 0;
    if (!fread(&sz, sizeof(sz), 1, file) || sz == 0)
        return false;
    totalsz = sz;
    mData = new char [sz];
    if (!fread(mData, sz, 1, file)) {
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
    sz = *(uint32_t *)p;
    p += sizeof(uint32_t);
    mInterleaved = (GLVertex *)p;
    p += sz;
    mVerticesCount = sz / sizeof(GLVertex);
    CalculateBounds();

    uint32_t dataSize = sz;
    mVerticesCount = sz / sizeof(GLVertex);
    sz = *(uint32_t *)p;
    p += sizeof(uint32_t);
    mIndices = (unsigned short *)p;
    p += sz;
    uint32_t indicesSize = sz;
    mIndicesCount = sz / sizeof(unsigned short);
    if ((uint32_t)(p - (char *)mData) != totalsz) {
        return false;
    }
    CreateBuffers(dataSize, indicesSize);
    return true;
}

void OpenGLESMesh::CreateBuffers(int dataSize, int indicesSize)
{
    mBuffer.set(mInterleaved, dataSize, mIndices, indicesSize);
}

bool OpenGLESMesh::ReadFromData(const char *data, int size)
{
    Clear();
    uint32_t sz = (*(const int *)data), totalsz = 0;
    totalsz = sz;
    if (!data || !sz || size != sz + sizeof(int32_t)) {
        mData = 0;
        return false;
    }
    data += sizeof(int32_t);
    mData = (void *)data;
    mOwner = false;
    char *p = (char *)mData;
    mType = *(Type *)p;
    p += sizeof(Type);
    sz = *(uint32_t *)p;
    p += sizeof(uint32_t);
    mInterleaved = (GLVertex *)p;
    p += sz;
    mVerticesCount = sz / sizeof(GLVertex);
    CalculateBounds();
    
    uint32_t dataSize = sz;
    mVerticesCount = sz / sizeof(GLVertex);
    sz = *(uint32_t *)p;
    p += sizeof(uint32_t);
    mIndices = (unsigned short *)p;
    p += sz;
    uint32_t indicesSize = sz;
    mIndicesCount = sz / sizeof(unsigned short);
    if ((uint32_t)(p - (char *)mData) != totalsz) {
        return false;
    }
    CreateBuffers(dataSize, indicesSize);
    return true;
}

void OpenGLESMesh::Render(RenderContext &context)
{
    mBuffer.render(context);
}
