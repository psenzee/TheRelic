#include <conio.h>

#include "core/rendermesh.h"
#include "core/wavefront.h"

#include <map>
#include <vector>

#include "iphonemesh.h"

enum MeshType { NONINTERLEAVED_LIST = 0, INTERLEAVED_LIST = 1, INTERLEAVED_STRIP = 2, _ = 0x7fffffff };

bool save_iphone_mesh_interleaved_list(RenderMesh &mesh, const char *filename);
bool save_iphone_mesh_noninterleaved_list(RenderMesh &mesh, const char *filename);
bool save_iphone_mesh_interleaved_strip(RenderMesh &mesh, const char *filename);

bool save_iphone_mesh_interleaved(RenderMesh &mesh, const char *filename)
{
    Geometry &geometry = mesh.sections[0].geometry;
    switch (geometry.type)
    {
    case GeometryType::TRIANGLE_LIST:  return save_iphone_mesh_interleaved_list  (mesh, filename);
    case GeometryType::TRIANGLE_STRIP: return save_iphone_mesh_interleaved_strip (mesh, filename);
    }
    return false;
}

bool save_iphone_mesh(RenderMesh &mesh, const char *filename, bool interleaved)
{
    Geometry &geometry = mesh.sections[0].geometry;
    if (interleaved || geometry.type == GeometryType::TRIANGLE_STRIP) // if it's a strip, then we only support interleaved
        return save_iphone_mesh_interleaved(mesh, filename);
    return save_iphone_mesh_noninterleaved_list(mesh, filename);
}

void wavefront2ip(const WavefrontVertex *v, int count, std::vector<GLVertex> &u)
{
    for (int i = 0; i < count; i++)
    {
        WavefrontVertex wv(v[i]);
        GLVertex ipv;
        ipv.position = wv.vertex;
        ipv.uv       = wv.uv;
        ipv.normal   = wv.normal;
        u.push_back(ipv);
    }
}

bool save_iphone_mesh_interleaved_list(RenderMesh &mesh, const char *filename)
{
    if (mesh.sections.empty())
        return false;
    FILE *file = fopen(filename, "wb");
    if (!file)
        return false;
    Geometry &geometry = mesh.sections[0].geometry;
    // these don't have indices
    const char *vertices = (const char *)geometry.vertices.data;
    int stride = geometry.vertices.stride;

    std::map<WavefrontVertex, size_t> map;
    std::vector<GLVertex>             wvertices;
    std::vector<unsigned short>       indices;

    // TRIANGLE LIST ONLY
    for (int i = 0, sz = geometry.vertices.count(); i < sz; i++)
    {
        WavefrontVertex v = *(const WavefrontVertex *)vertices;
        size_t index = 0;
        if (map.find(v) != map.end())
            index = map[v];
        else
        {
            index = wvertices.size();
            GLVertex iv;
            iv.uv       = v.uv;
            iv.normal   = v.normal;
            iv.position = v.vertex;
            wvertices.push_back(iv);
            map[v] = index;
        }
        indices.push_back((unsigned short)index);
        vertices += stride;
    }
    size_t vsize   = wvertices.size()  * sizeof(GLVertex),
           isize   = indices.size()    * sizeof(unsigned short),
           type    = INTERLEAVED_LIST,
           totalsz = sizeof(type) + sizeof(vsize) + vsize + sizeof(isize) + isize;
    bool success = fwrite(&totalsz,      sizeof(size_t), 1, file) &&
                   fwrite(&type,         sizeof(type),   1, file) &&
                   fwrite(&vsize,        sizeof(size_t), 1, file) &&
                   fwrite(&wvertices[0], vsize,          1, file) &&
                   fwrite(&isize,        sizeof(size_t), 1, file) &&
                   fwrite(&indices[0],   isize,          1, file);
    fclose(file);
    return success;
}

void CopyIntsToUShorts(const int *v, int count, std::vector<unsigned short> &u)
{
    u.clear();
    for (int i = 0; i < count; i++)
        u.push_back((unsigned short)v[i]);
}

bool save_iphone_mesh_interleaved_strip(RenderMesh &mesh, const char *filename)
{
    if (mesh.sections.empty())
        return false;
    FILE *file = fopen(filename, "wb");
    if (!file)
        return false;
    Geometry &geometry = mesh.sections[0].geometry;
    // these don't have indices
    const char *wv_vertices = (const char *)geometry.vertices.data;
    const int  *int_indices = (const int  *)geometry.indices.data;
    std::vector<unsigned short> indices;
    for (int i = 0; i < geometry.indices.size / sizeof(int); i++)
        indices.push_back((unsigned short)int_indices[i]);
    std::vector<GLVertex> vertices;
    wavefront2ip((const WavefrontVertex *)wv_vertices, geometry.vertices.size / geometry.vertices.stride, vertices);
    size_t vsize   = vertices.size() * sizeof(GLVertex),
           isize   = indices.size() * sizeof(unsigned short),
           type    = INTERLEAVED_STRIP,
           totalsz = sizeof(type) + sizeof(vsize) + vsize + sizeof(isize) + isize;
    bool success = fwrite(&totalsz,     sizeof(size_t), 1, file) &&
                   fwrite(&type,        sizeof(type),   1, file) &&
                   fwrite(&vsize,       sizeof(size_t), 1, file) &&
                   fwrite(&vertices[0], vsize,          1, file) &&
                   fwrite(&isize,       sizeof(size_t), 1, file) &&
                   fwrite(&indices[0],  isize,          1, file);
    fclose(file);
    return success;
}

// TO BE DEPRECATED
bool save_iphone_mesh_noninterleaved_list(RenderMesh &mesh, const char *filename)
{
    if (mesh.sections.empty())
        return false;
    FILE *file = fopen(filename, "wb");
    if (!file)
        return false;
    Geometry &geometry = mesh.sections[0].geometry;
    // these don't have indices
    const char *vertices = (const char *)geometry.vertices.data;
    int stride = geometry.vertices.stride;

    std::map<WavefrontVertex, size_t> map;
    std::vector<Tuple3f>              positions,
                                      normals;
    std::vector<Tuple2f>              uvs;
    std::vector<unsigned short>       indices;

    // TRIANGLE LIST ONLY
    for (int i = 0, sz = geometry.vertices.count(); i < sz; i++)
    {
        WavefrontVertex v = *(const WavefrontVertex *)vertices;
        size_t index = 0;
        if (map.find(v) != map.end())
            index = map[v];
        else
        {
            index = positions.size();
            positions.push_back(v.vertex);
            normals.push_back(v.normal);
            uvs.push_back(v.uv);
            map[v] = index;
        }
        indices.push_back((unsigned short)index);
        vertices += stride;
    }
    size_t psize   = positions.size() * sizeof(Tuple3f),
           nsize   = normals.size()   * sizeof(Tuple3f),
           usize   = uvs.size()       * sizeof(Tuple2f),
           isize   = indices.size()   * sizeof(unsigned short),
           totalsz = psize + nsize + usize + isize + (4 * sizeof(size_t));
    bool success = fwrite(&totalsz, sizeof(size_t), 1, file) &&
                   fwrite(&psize, sizeof(size_t), 1, file)   &&
                   fwrite(&positions[0], psize, 1, file)     &&
                   fwrite(&nsize, sizeof(size_t), 1, file)   &&
                   fwrite(&normals[0], nsize, 1, file)       &&
                   fwrite(&usize, sizeof(size_t), 1, file)   &&
                   fwrite(&uvs[0], usize, 1, file)           &&
                   fwrite(&isize, sizeof(size_t), 1, file)   &&
                   fwrite(&indices[0], isize, 1, file);
    fclose(file);
    return success;
}

#include "fileiterator.h"

bool convert_file(const char *filename, bool interleaved)
{
    bool verify = false;
    RenderMesh mesh;
    WavefrontReader r;
    r.read(filename, mesh);
    char newfilename[1024];
    if (interleaved)
        sprintf(newfilename, "%s.ipi", filename);
    else
        sprintf(newfilename, "%s.ipm", filename);
    if (!save_iphone_mesh(mesh, newfilename, interleaved))
    {
        printf("Conversion of '%s' failed!\n", filename);
        return false;
    }
    if (verify)
    {
        IPhoneMesh ipm;
        if (!ipm.read(newfilename))
        {
            printf("Conversion of '%s' failed!\n", filename);
            return false;
        }
        for (int i = 0; i < ipm.verticesCount; i++)
            ipm.printVertex(i);
        printf("%d vertices\n", ipm.verticesCount);
        printf("%d indices\n",  ipm.indicesCount);
    }
    return true;
}

int main(int argc, const char **argv)
{
    if (argc < 2)
    {
        printf("Usage: %s <filespec> [-ni]\n"
               "       -ni noninterleaved position, normal, uv data\n\n", argv[0]);
        return -1;
    }

    FileIterator fi(argv[1]);
    bool interleaved = !(argc > 2 && stricmp(argv[2], "-ni") == 0);
    while (fi.next())
        convert_file(fi.name(), interleaved);

    return 0;
}
