#include "shapes.h"
#include "objfile.h"
#include "uvmap.h"
#include "types/matrix4t.h"
#include "commands.h"
#include "meshfile.h"

#include <math.h>
#include <stdio.h>

int read(int argc, int start, const char **args, SphereOptions &options)
{
    int i = start;
    for (; i < argc; i++)
    {
        const char *s = args[i];
        if      (stricmp(s, "-ccw") == 0)
            options.ccw = true;
        else if (stricmp(s, "-cw") == 0)
            options.ccw = false;
        else if (stricmp(s, "-levels") == 0)
            i = read(argc, i + 1, args, &options.levels);
        else return i - 1;
    }
    return i;
}

int read(int argc, int start, const char **args, SuperQuadricOptions &options)
{
    int i = start;
    for (; i < argc; i++)
    {
        const char *s = args[i];
        if      (stricmp(s, "-ccw") == 0)
            options.ccw = true;
        else if (stricmp(s, "-cw") == 0)
            options.ccw = false;
        else if (stricmp(s, "-NER") == 0)
            i = read(argc, i + 1, args, options.ner);
        else if (stricmp(s, "-increment") == 0)
            i = read(argc, i + 1, args, options.increment);
        else return i - 1;
    }
    return i;
}

int main(int argc, const char **args)
{
    if (argc < 3)
    {
        fprintf(stderr, "Usage: %s [-read <options>|-sphere <options>|-superquadric <options>] -filename <output.obj> [(-scale|-translate|-rotate) <x> <y> <z>]*\n\n"
                        "       %s -read <input.txt>\n"
                        "       %s -sphere       [-ccw|-cw] [-levels <n>]\n"
                        "       %s -superquadric [-ccw|-cw] [-NER <n> <e> <r>] [-increment <u> <v>]\n",
                        args[0], args[0], args[0], args[0]);
        return -1;
    }
    FaceList faces;
    Matrix4T<float> xform, m;
    const char *filename = "out.obj";
    for (int i = 1; i < argc; i++)
    {
        if (stricmp(args[i], "-filename") == 0 || stricmp(args[i], "-file") == 0)
        {
            filename = args[i + 1];
            i++;
        }
        else if (stricmp(args[i], "-scale") == 0)
        {
            Tuple3T<float> v;
            i = read(argc, i + 1, args, v);
            m.scale(v);
            xform = xform * m;
        }
        else if (stricmp(args[i], "-rotate") == 0)
        {
            Tuple3T<float> v;
            i = read(argc, i + 1, args, v);
            m.eulerrotation(v);
            xform = xform * m;
        }
        else if (stricmp(args[i], "-translate") == 0)
        {
            Tuple3T<float> v;
            i = read(argc, i + 1, args, v);
            m.translate(v);
            xform = xform * m;
        }
        else if (stricmp(args[i], "-sphere") == 0)
        {
            SphereOptions options;
            i = read(argc, i + 1, args, options);
            sphere(faces, options);
        }
        else if (stricmp(args[i], "-superquadric") == 0)
        {
            SuperQuadricOptions options;
            i = read(argc, i + 1, args, options);
            superquadric(faces, options);
        }
        else if (stricmp(args[i], "-read") == 0)
        {
            read(args[i + 1], faces);
            i++;
        }
        else if (stricmp(args[i], "-pov") == 0)
        {
            read_pov(args[i + 1], faces);
            i++;
        }
    }
    if (faces.faces.empty())
    {
        fprintf(stderr, "No geometry to write!\n");
    }
    else
    {
        transform(xform, faces);
        ObjFile::write(filename, faces);
    }
}