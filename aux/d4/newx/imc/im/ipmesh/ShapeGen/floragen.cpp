#include "faces.h"
#include "objfile.h"
#include "shapes.h"
#include "types/tuple2t.h"
#include "types/tuple3t.h"
#include "types/matrix4t.h"

void translate(const Tuple3T<float> &t, Face &face)
{
    for (std::vector<Vertex>::iterator i = face.vertices.begin(), e = face.vertices.end(); i != e; ++i)
    {
        Vertex &v = *i;
        v.position += t;
    }
}

void translate(const Tuple3T<float> &t, FaceList &faces)
{
    for (std::vector<Face>::iterator i = faces.faces.begin(), e = faces.faces.end(); i != e; ++i)
        translate(t, *i);
}


void create_up_quad(Face &face, const Tuple3T<float> &t, const Tuple3T<float> &s, const Tuple2T<float> &tt = Tuple2T<float>(0.0f, 0.0f), const Tuple2T<float> &ts = Tuple2T<float>(1.0f, 1.0f))
{
    face.clear();
    Vertex v;

    v.setnormal(Tuple3T<float>(0.f, 1.f, 0.f));

    v.setposition(Tuple3T<float>(-s.x + t.x, t.y, -s.z + t.z));
    v.setuv(0, Tuple2T<float>(tt.x, tt.y));
    face.add(v);

    v.setposition(Tuple3T<float>(-s.x + t.x, t.y,  s.z + t.z));
    v.setuv(0, Tuple2T<float>(tt.x, ts.y + tt.y));
    face.add(v);

    v.setposition(Tuple3T<float>( s.x + t.x, t.y,  s.z + t.z));
    v.setuv(0, Tuple2T<float>(ts.x + tt.x, ts.y + tt.y));
    face.add(v);

    v.setposition(Tuple3T<float>( s.x + t.x, t.y, -s.z + t.z));
    v.setuv(0, Tuple2T<float>(ts.x + tt.x, tt.x));
    face.add(v);
}

void create_up_quad(FaceList &faces, const Tuple3T<float> &t, const Tuple3T<float> &s, int divu = 1, int divv = 1)
{
    Face face;
    float du = 1.f / divu, dv = 1.f / divv;
    Tuple3T<float> ns(du / 2.f, 1.f, dv / 2.f),
                   at(0.f, 0.f, 0.f);
    Tuple2T<float> ts(du, dv), tt(0.f, 0.f);
    Matrix4T<float> m, im;
    m.scale(s);
    im.scale(1.f / s);
    im.transpose();
    for (int j = 0; j < divv; j++)
    {
        at.z = j * dv + dv / 2.f - 0.5f;
        tt.y = j * dv;
        for (int i = 0; i < divu; i++)
        {
            at.x = i * du + du / 2.f - 0.5f;
            tt.x = i * du;
            create_up_quad(face, at, ns, tt, ts);
            transform(m, im, face);
            translate(t, face);
            faces.add(face);
        }
    }
}

void create_tree_branches(FaceList &faces, unsigned count)
{
    enum { MAX_PLANES = 1024 };

    static double rangles[MAX_PLANES];
    static bool   rinited = false;

    if (!rinited)
    {
        for (unsigned i = 0; i < MAX_PLANES; i++)
            rangles[i] = rand() / (double)RAND_MAX * math::PI; // random through 180 degrees..
        rinited = true;
    }

    Face face;
    create_up_quad(face, Tuple3T<float>(0.f, 0.f, 0.f), Tuple3T<float>(5.f, 5.f, 5.f));
    Matrix4T<float> m;
    m.rotationz((float)(math::PI / 2.));
    transform(m, face);
    if (count > MAX_PLANES) count = MAX_PLANES;
    float d = (float)(math::PI / count);
    Matrix4T<float> mb;
    for (unsigned i = 0; i < count; i++)
    {
        Face f = face;
        m.rotationx((float)rangles[MAX_PLANES / count * i]);
        mb.rotationy(d * i);
        m *= mb;
        transform(m, f);
        faces.add(f);
    }
    m.translate(Tuple3T<float>(0.f, 3.f, 0.f));
    transform(m, faces);
}

void create_base_quad(float size, float y, FaceList &faces, int divu = 1, int divv = 1)
{
    create_up_quad(faces, Tuple3T<float>(0.f, y, 0.f), Tuple3T<float>(size, size, size), divu, divv);
}

void create_basic_grass(FaceList &faces, float size, unsigned layers, float thickness, float start, int divu = 1, int divv = 1)
{
    for (unsigned i = 0; i < layers; i++)
        create_base_quad(size, i * thickness + start, faces, divu, divv);
}

void create_basic_ground(FaceList &faces, float size, float y, int divu = 1, int divv = 1)
{
    create_base_quad(size, y, faces, divu, divv);
}

/*
inline float _max(float a, float b)
{
    return a < b ? b : a;
}

inline float normalize(float x, float range_a, float range_b) // returns 0 to 1 range
{
    float range = range_b - range_a;
    if (range == 0.f)
        return 0.f;
    return (x - range_a) / range;
}

void remap_texture_coords(WavefrontVertex &a, WavefrontVertex &b, WavefrontVertex &c, float scale)
{
    Tuple3f face_norm = face_normal(a.vertex, b.vertex, c.vertex);
    float   dy = fabs(dot(face_norm, Tuple3f( 0.f,  1.f,  0.f))),
            dx = fabs(dot(face_norm, Tuple3f( 1.f,  0.f,  0.f))),
            dz = fabs(dot(face_norm, Tuple3f( 0.f,  0.f,  1.f))),
            v  = _max(dy, _max(dx, dz)),
            min_scale = -scale / 2.f,
            max_scale =  scale / 2.f;
    if (v == dy) // top or bottom
    {
        a.uv.x = normalize(a.vertex.x, min_scale, max_scale);
        a.uv.y = normalize(a.vertex.z, min_scale, max_scale);
        b.uv.x = normalize(b.vertex.x, min_scale, max_scale);
        b.uv.y = normalize(b.vertex.z, min_scale, max_scale);
        c.uv.x = normalize(c.vertex.x, min_scale, max_scale);
        c.uv.y = normalize(c.vertex.z, min_scale, max_scale);
    }
    else if (v == dx) // left or right
    {
        a.uv.x = normalize(a.vertex.z, min_scale, max_scale);
        a.uv.y = normalize(a.vertex.y, min_scale, max_scale);
        b.uv.x = normalize(b.vertex.z, min_scale, max_scale);
        b.uv.y = normalize(b.vertex.y, min_scale, max_scale);
        c.uv.x = normalize(c.vertex.z, min_scale, max_scale);
        c.uv.y = normalize(c.vertex.y, min_scale, max_scale);
    }
    else if (v == dz) // left or right
    {
        a.uv.x = normalize(a.vertex.x, min_scale, max_scale);
        a.uv.y = normalize(a.vertex.y, min_scale, max_scale);
        b.uv.x = normalize(b.vertex.x, min_scale, max_scale);
        b.uv.y = normalize(b.vertex.y, min_scale, max_scale);
        c.uv.x = normalize(c.vertex.x, min_scale, max_scale);
        c.uv.y = normalize(c.vertex.y, min_scale, max_scale);
    }
}

void remap_texture_coords(Geometry &geo, float scale)
{
    unsigned         vertex_count = geo.vertices.count();
    WavefrontVertex *list         = (WavefrontVertex *)geo.vertices.data;
    for (size_t i = 0; i < vertex_count; i += 3)
        remap_texture_coords(list[i], list[i + 1], list[i + 2], scale);
}
*/

void write_basic_ground(const char *filename, float size, float y)
{
    FaceList faces;
    ObjOption option;
    char path[1024];
    for (unsigned i = 1; i <= 16; i *= 2)
    {
        faces.clear();
        create_basic_ground(faces, size, y, i, i);
        sprintf(path, "%s_t%u.obj", filename, i);
        ObjFile::write(path, option, faces);
    }
}

void write_basic_grass(const char *filename, float size, unsigned layers, float thickness, float start)
{
    FaceList faces;
    ObjOption option;
    char path[1024];
    create_basic_grass(faces, size, layers, thickness, start);
    sprintf(path, "%s.obj", filename);
    ObjFile::write(path, option, faces);
}

void write_tree_branches(const char *filename)
{
    FaceList faces;
    ObjOption option;
    char path[1024];
    for (unsigned i = 4; i <= 64; i *= 4)
    {
        faces.clear();
        create_tree_branches(faces, i);
        sprintf(path, "%s_t%u.obj", filename, i);
        ObjFile::write(path, option, faces);
    }
}

void floragen_main()
{
    write_basic_grass   ("basic-grass",   15.0f, 25, 0.02f, 0.01f);
    write_basic_ground  ("basic-ground",  15.0f,  0.0f);
    write_basic_ground  ("basic-street",  15.0f, -0.3f);
    write_tree_branches ("tree-branches");
}