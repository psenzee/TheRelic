#include "OpenGLESMesh.h"
#include "core/global.h"
#include "render/GLUtils.h"
#include "render/GLIncludes.h"
#include "GLAbstract.h"
#include "glError.h"

#include <stdio.h>
#include <string.h>

OpenGLESMesh::OpenGLESMesh() : mData(0), mNormalAction(NORMALIZE_ACTION_NONE), mVb(0), mIb(0), mInterleaved(0), mCompact(0), mOwner(false)
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
    for (int i = 0; i < mVerticesCount; i++) {
        mCompact[i].x = static_cast<short>(mInterleaved[i].position.x);
        mCompact[i].y = static_cast<short>(mInterleaved[i].position.y);
        mCompact[i].z = static_cast<short>(mInterleaved[i].position.z);
        mCompact[i].uv = mInterleaved[i].uv;
    }
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

bool OpenGLESMesh::Read(const char *filename, bool asCompact)
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
    
    if (asCompact) {
        Compact();
    }
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
    auto ids = GLCreateBuffers(mInterleaved, mIndices, dataSize, indicesSize);
    mVb = ids[0];
    mIb = ids[1];
}

void OpenGLESMesh::DestroyBuffers()
{
    std::array<unsigned, 2> ids { mIb, mVb };
    GLDestroyBuffers(ids);
    mIb = mVb = 0;
}

bool OpenGLESMesh::ReadFromData(const char *data, int size, bool asCompact)
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
    
    if (asCompact) {
        Compact();
    }
    CalculateBounds();
    
    uint32_t dataSize = sz;
    mVerticesCount = sz / sizeof(GLVertex);
    sz = *(uint32_t *)p;
    p += sizeof(uint32_t);
    mIndices = (unsigned short *)p;
    p += sz;
    uint32_t indicesSize = sz;
    mIndicesCount = sz / sizeof(unsigned short);
    if ((uint32_t)(p - (char *)mData) != totalsz)
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
    GLSetNormalAction(useNormals, mNormalAction, true);
    GLSetEnabled(GL_CULL_FACE, true);
    SetBuffersInterleaved(mVb, mIb, static_cast<unsigned>(mVerticesCount), useNormals);
    // This is the actual draw command
    GLDrawElements(mType == INTERLEAVED_STRIP ? GL_TRIANGLE_STRIP : GL_TRIANGLES, mIndicesCount);
    //_GLv(glDrawElements(mType == INTERLEAVED_STRIP ? GL_TRIANGLE_STRIP : GL_TRIANGLES, mIndicesCount, GL_UNSIGNED_SHORT, 0));
    GLSetNormalAction(useNormals, mNormalAction, false);
}
