#ifndef _OBJFILE_H
#define _OBJFILE_H

#include "faces.h"
#include <iostream>

class ObjOption
{
public:
    bool position,
         normal,
         uv,
         smooth,
         reverse;

    inline ObjOption(bool position = true, bool normal = true, bool uv = true, bool smooth = true, bool reverse = false) : 
        position(position), normal(normal), uv(uv), smooth(smooth), reverse(reverse) {}
};

class ObjGroup;

class ObjFile
{
    std::vector<ObjGroup *> m_groups;
    
public:

    ~ObjFile();

    void          append(const char *name, const FaceList &faces);
    std::ostream &print(const ObjOption &option, std::ostream &s) const;
    
    // convenience
    static void   write(const char *filename, const ObjOption &option, const FaceList &faces);
    static void   write(const char *filename, const FaceList &faces)
    {
        ObjOption options; // default
        write(filename, options, faces);
    }
};

#endif  // _OBJFILE_H