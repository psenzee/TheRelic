#include "meshfile.h"
#include "shapes.h"
#include "strs.h"
#include "file.h"

#include "povparser.h"
#include "ipovvisitor.h"
#include "povobjects.h"
#include "povtypes.h"

#include "types/matrix4t.h"

#include <stdio.h>

static bool read(FILE *file, Vertex &v)
{
    enum { LINE_SIZE = 64 * 1024, COMPONENTS = 8 };
    char line[LINE_SIZE + 1];
    do
    {
        if (fgets(line, LINE_SIZE, file) == 0)
            return false;
    } while (*line == '#' || isspace(line));
    float info[COMPONENTS];
    const char  *p = (const char *)line,
               **s = (const char **)&p;
    read(s, info, COMPONENTS);
    v.setposition(   Tuple3T<float>(info[0], info[1], info[2]));
    v.setuv      (0, Tuple2T<float>(info[3], info[4]));
    v.setnormal  (   Tuple3T<float>(info[5], info[6], info[7]));
    return true;
}

static bool read_triangle(FILE *file, Face &face)
{
    face.clear();
    Vertex v;
    if (!read(file, v)) return false;
    face.add(v);
    if (!read(file, v)) return false;
    face.add(v);
    if (!read(file, v)) return false;
    face.add(v);
    return true;
}

bool read(const char *filename, FaceList &faces)
{
    Face face;
    FILE *file = fopen(filename, "r");
    if (!file)
        return false;
    while (read_triangle(file, face))
        faces.add(face);
    fclose(file);
    return true;
}

void transform(const Matrix4T<double> &m, FaceList &faces)
{
    Matrix4T<float> mf;
    for (int i = 0; i < 4 * 4; i++)
        mf.data[i] = (float)m.data[i];
    transform(mf, faces);
}

void transform(const Matrix4T<double> &m, const Matrix4T<double> &it, FaceList &faces)
{
    Matrix4T<float> mf, itf;
    for (int i = 0; i < 4 * 4; i++)
    {
        mf.data[i]  = (float)m.data[i];
        itf.data[i] = (float)it.data[i];
    }
    transform(mf, itf, faces);
}

class PovMesher : public IPovVisitor
{
public:

    static void mesh(IPovEntity *object, FaceList &faces)
    {
        PovMesher pm;
        if (object)
            object->accept(pm, &faces);
    }

    void visit(PovReal               &u, void *) { }
    void visit(PovVector             &u, void *) { }
    void visit(PovColor              &u, void *) { }
    void visit(PovImageFile          &u, void *) { }

    void visit(PovTransform          &u, void *) { }
    void visit(PovIgnorableAttribute &u, void *) { }

    void visit(PovNull               &u, void *) { }
    void visit(PovRef                &u, void *) { }

    void visit(PovBox                &u, void *) { /* mesh */ }
    void visit(PovCone               &u, void *) { /* mesh */ }
    void visit(PovCylinder           &u, void *) { /* mesh */ }
    void visit(PovSphere             &u, void *context)
    {
        FaceList &parent = *(FaceList *)context;
        FaceList faces;

        SphereOptions options;
        sphere(faces, options);
        Matrix4T<double> m, mt;
        m.scale(u.radius);
        mt.translate(u.center);
        m = m * mt;
        transform(m, faces);

        transform(u.transform().matrix, u.transform().inverse, faces);
        parent.append(faces);
    }
    void visit(PovHeightField        &u, void *) { /* mesh */ }
    void visit(PovModel              &u, void *) { /* mesh */ }
    void visit(PovFnCylinder         &u, void *) { /* mesh */ }
    void visit(PovGroup              &u, void *context)
    {
        FaceList &parent = *(FaceList *)context;
        FaceList faces;

        std::vector<PovObject *>::iterator i = u.objects.begin(), e = u.objects.end();
        for (; i != e; ++i)
            (*i)->accept(*this, &faces);

        transform(u.transform().matrix, u.transform().inverse, faces);
        parent.append(faces);
    }
    void visit(PovPlane              &u, void *) { /* mesh */ }
    void visit(PovPyramid            &u, void *) { /* mesh */ }
    void visit(PovTorus              &u, void *) { /* mesh */ }
    void visit(PovWedge              &u, void *) { /* mesh */ }
    void visit(PovTaperedWedge       &u, void *) { /* mesh */ }
    void visit(PovCamera             &u, void *) { }
    void visit(PovLightSource        &u, void *) { }
    void visit(PovGlobalSettings     &u, void *) { }
    void visit(PovDefaultSpec        &u, void *) { }
    void visit(PovTexture            &u, void *) { }
    void visit(PovBackground         &u, void *) { }
    void visit(PovPigment            &u, void *) { }
    void visit(PovFinish             &u, void *) { }
    void visit(PovMaterial           &u, void *) { }
    void visit(PovFog                &u, void *) { }
};

IPovEntity *parse(const char *s)
{
    IPovEntity *e = 0;
    try { e = PovParser::parse(s); }
    catch (std::exception &ex) { printf("error: %s\r\n", ex.what()); e = 0; }
    return e;
}

void print(const PovBound &bound)
{
    AABoxT<double> b(bound.bounds);
    printf("AABox <(%f, %f, %f)-(%f, %f, %f) inverted: %s>\r\n", b.mn.x, b.mn.y, b.mn.z, b.mx.x, b.mx.y, b.mx.z, bound.inverted ? "yes" : "no");
}

bool read_pov(const char *filename, FaceList &faces)
{
    std::string data(file::read_file_string(filename));
    IPovEntity *object = parse(data.c_str());

    PovMesher::mesh(object, faces);

    if (object && object->object())
    {
        PovBound bounds = object->object()->bounds();
        print(bounds);
    }
    return true;
}