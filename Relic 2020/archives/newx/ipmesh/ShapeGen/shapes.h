#ifndef _SHAPES_H
#define _SHAPES_H

#include "objfile.h"
#include "types/matrix4t.h"

void sphere(FaceList &faces, bool ccw, int maxlevel);
void superquadric(FaceList &faces, double n, double e, double r = 0., double incv = 0.05, double incu = 0.05);

struct SphereOptions
{
    bool           ccw;
    int            levels;

    inline SphereOptions() : ccw(true), levels(4) {}
};

struct SuperQuadricOptions
{
    bool           ccw;
    Tuple2T<float> increment;
    Tuple3T<float> ner;

    inline SuperQuadricOptions() : ccw(true), increment(0.5f, 0.5f), ner(1.f, 0.1f, 1.5f) {}
};

void sphere      (FaceList &faces, const SphereOptions       &options);
void superquadric(FaceList &faces, const SuperQuadricOptions &options);

// these aliases allow us to template some reading code
void create      (FaceList &faces, const SphereOptions       &options);
void create      (FaceList &faces, const SuperQuadricOptions &options);

void transform(const Matrix4T<float> &m, const Matrix4T<float> &it, Vertex &v);
void transform(const Matrix4T<float> &m, const Matrix4T<float> &it, Face &face);
void transform(const Matrix4T<float> &m, Face &face); // inefficient..
void transform(const Matrix4T<float> &m, const Matrix4T<float> &it, FaceList &faces);
void transform(const Matrix4T<float> &m, FaceList &faces);

void transform(const Matrix4T<double> &m, FaceList &faces);

#endif // _SHAPES_H