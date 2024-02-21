#ifndef _WAVEFRONTREADER_H_
#define _WAVEFRONTREADER_H_

class RenderMesh;

#include "tuple3f.h"
#include "tuple2f.h"

class WavefrontVertex
{
public:
    Tuple3f vertex, normal;
    Tuple2f uv;

    inline bool operator<(const WavefrontVertex &wv) const
    {
        if      (vertex < wv.vertex) return true;
        else if (vertex > wv.vertex) return false;
        if      (normal < wv.normal) return true;
        else if (normal > wv.normal) return false;
        if      (uv     < wv.uv)     return true;
        return false;
    }
};

class WavefrontReader
{
public:
    bool read(const char *filename, RenderMesh &mesh) const;
};

#endif // _WAVEFRONTREADER_H_