#ifndef _IPHONEMESH_H
#define _IPHONEMESH_H

#include <stdio.h>

#include "core/tuple3f.h"
#include "core/tuple2f.h"

class GLVertex
{
    // corresponds to
    // GL_T2F_N3F_V3F format
public:
    Tuple2f uv;
    Tuple3f normal,
            position;
};

class IPhoneMesh
{
public:

    enum Type { NONINTERLEAVED_LIST = 0, INTERLEAVED_LIST = 1, INTERLEAVED_STRIP = 2, _ = 0x7fffffff };

    void           *buffer;
    Type            type;
    float          *vertices,
                   *normals,
                   *uvs,
                   *interleaved;
    unsigned short *indices;
    size_t          verticesCount,
                    normalsCount,
                    uvsCount,
                    indicesCount;

    IPhoneMesh();
    ~IPhoneMesh();

    bool read(const char *filename);
    void clear();

    void printVertex(int index) const;

private:

    enum { VERTEX_SIZE = sizeof(GLVertex) };

    bool read_interleaved(FILE *file, unsigned totalsz, char *buffer);
    bool read_noninterleaved(FILE *file, unsigned totalsz, char *buffer);
};

#endif  // _IPHONEMESH_H