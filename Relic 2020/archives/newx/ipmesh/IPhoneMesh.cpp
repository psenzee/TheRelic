#include "iphonemesh.h"

#include <stdio.h>

IPhoneMesh::IPhoneMesh() : buffer(0)
{
    clear();
}

IPhoneMesh::~IPhoneMesh()
{
    clear();
}

void IPhoneMesh::clear()
{
    if (buffer)
        delete buffer;
    buffer = 0;
    vertices = normals = 0;
    uvs = 0;
    interleaved = 0;
    indices = 0;
    verticesCount = normalsCount = uvsCount = indicesCount = 0;
    type = NONINTERLEAVED_LIST;
}

bool IPhoneMesh::read(const char *filename)
{
    clear();
    FILE *file = fopen(filename, "rb");
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
        delete buffer;
        buffer = 0;
        return false;
    }
    fclose(file);
    char *p = (char *)buffer;
    type = *(Type *)p;
    if (type != INTERLEAVED_STRIP && type != INTERLEAVED_LIST)
        type = NONINTERLEAVED_LIST;
    switch (type)
    {
    case INTERLEAVED_STRIP:
    case INTERLEAVED_LIST:    return read_interleaved    (file, (unsigned)(totalsz - sizeof(Type)), (char *)buffer + sizeof(Type));
    case NONINTERLEAVED_LIST: return read_noninterleaved (file, (unsigned) totalsz,                 (char *)buffer);
    }
    clear();
    return false;
    /*
    char *p = (char *)buffer;
    sz = *(size_t *)p;
    p += sizeof(size_t);
    vertices = (float *)p;
    p += sz;
    verticesCount = sz / (3 * sizeof(float));
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
    if (p - (char *)buffer != totalsz)
        return false;
    return true;
    */
}

bool IPhoneMesh::read_noninterleaved(FILE *file, unsigned totalsz, char *buffer)
{
    char *p = (char *)buffer;
    size_t sz = *(size_t *)p;
    p += sizeof(size_t);
    vertices = (float *)p;
    p += sz;
    verticesCount = sz / (3 * sizeof(float));
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
    if (p - (char *)buffer != totalsz)
        return false;
    return true;
}

bool IPhoneMesh::read_interleaved(FILE *file, unsigned totalsz, char *buffer)
{
    char *p = (char *)buffer;
    size_t sz = *(size_t *)p;
    p += sizeof(size_t);
    vertices = (float *)p;
    p += sz;
    verticesCount = normalsCount = uvsCount = sz / VERTEX_SIZE;
    sz = *(size_t *)p;
    p += sizeof(size_t);
    indices = (unsigned short *)p;
    p += sz;
    indicesCount = sz / sizeof(unsigned short);
    if (p - (char *)buffer != totalsz)
        return false;
    return true;
}

void IPhoneMesh::printVertex(int i) const
{
    if (vertices)
        printf("v (%f, %f, %f)\n", vertices[i * 3], vertices[i * 3 + 1], vertices[i * 3 + 2]);
    else if (interleaved)
    {
        GLVertex &iv = ((GLVertex *)interleaved)[i];
        Tuple3f &pos = iv.position;
        printf("v (%f, %f, %f)\n", pos.x, pos.y, pos.z);
    }
}