#ifndef _FACES_H
#define _FACES_H

#include "types/tuple3t.h"
#include "types/tuple2t.h"
#include "vertex.h"

#include <vector>

class Face
{
public:
    std::vector<Vertex> vertices;
//  Tuple3T<float>      normal;

    inline void add(const Vertex &u) { vertices.push_back(u); }
    inline void clear()              { vertices.clear(); }
};

class FaceList
{
public:

    std::vector<Face> faces;

    inline void add(const Face &u) { faces.push_back(u); }
    inline void clear()            { faces.clear(); }
    void        append(const FaceList &u)
    {
        for (std::vector<Face>::const_iterator i = u.faces.begin(), e = u.faces.end(); i != e; ++i)
            add(*i);
    }
};

#endif  // _FACES_H