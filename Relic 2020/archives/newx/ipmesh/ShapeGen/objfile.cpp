#include "objfile.h"

#include "types/tuple3t.h"
#include "types/tuple2t.h"

#include "indext.h"
#include "vertex.h"

#include <vector>
#include <set>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <assert.h>

class ObjVertex
{
public:    
    size_t position,
           normal,
           uv;

    inline ObjVertex() : position(0), normal(0), uv(0) {}
    inline ObjVertex(const ObjVertex &v) : position(v.position), normal(v.normal), uv(v.uv) {}
    inline ObjVertex(size_t position, size_t normal, size_t uv) : position(position), normal(normal), uv(uv) {}
    inline ObjVertex &operator=(const ObjVertex &v) { position = v.position; normal = v.normal; uv = v.uv; return *this; }    
    
    inline bool operator==(const ObjVertex &v) const { return position == v.position; }
    inline bool operator!=(const ObjVertex &v) const { return position != v.position; }

    inline std::ostream &print(const ObjOption &option, std::ostream &s) const
    {
        if (option.position) s << (unsigned int)(position + 1);
        s << "/";
        if (option.uv)       s << (unsigned int)(uv + 1);
        if (option.normal)   s << "/" << (unsigned int)(normal + 1);
        return s;
    }
};

class ObjFace
{
public:

    std::vector<ObjVertex> vertices;
    
    inline void insert(const ObjVertex &v)
    {
        if (vertices.empty() || (v != vertices.back() && v != vertices.front()))
            vertices.push_back(v);
    }
    std::ostream &print(const ObjOption &option, std::ostream &s) const
    {
        if (vertices.size() > 2)
        {
            s << "f";
            if (option.reverse)
            {
                std::vector<ObjVertex>::const_reverse_iterator i = vertices.rbegin(), e = vertices.rend();
                for (; i != e; ++i)
                    (*i).print(option, s << " ");
            }
            else
            {
                std::vector<ObjVertex>::const_iterator i = vertices.begin(), e = vertices.end();
                for (; i != e; ++i)
                    (*i).print(option, s << " ");
            }
            s << std::endl;
        }
        return s;
    }
};

class ObjIndex
{
public:

    IndexT< Tuple3T<float> > positions,
                             normals;
    IndexT< Tuple2T<float> > uvs;
    
    inline ObjVertex insert(const Vertex &v)
    {
        return ObjVertex(positions.insert(v.getposition()),
                         normals.insert(v.getnormal()),
                         uvs.insert(v.getuv(0)));
    }
    inline std::ostream &print(const ObjOption &option, std::ostream &s) const
    {
        if (option.position) m_print(s, "v ", positions.list());
        if (option.normal)   m_print(s, "vn", normals.list());
        if (option.uv)       m_print(s, "vt", uvs.list());
        return s;
    }
    
private:

    static void m_print(std::ostream &s, const char *type, const std::vector< Tuple3T<float> > &v)
    {
        if (!v.empty())
        {
            std::vector< Tuple3T<float> >::const_iterator i = v.begin(), e = v.end();
            char xbuf[64], ybuf[64], zbuf[64];
            for (; i != e; ++i)
            {
                sprintf(xbuf, "%.6f", (*i).x); sprintf(ybuf, "%.6f", (*i).y); sprintf(zbuf, "%.6f", (*i).z);
                s << type << " " << xbuf << " " << ybuf << " " << zbuf << std::endl;
            }
        }
    }
    static void m_print(std::ostream &s, const char *type, const std::vector< Tuple2T<float> > &v)
    {
        if (!v.empty())
        {        
            std::vector< Tuple2T<float> >::const_iterator i = v.begin(), e = v.end();
            char xbuf[64], ybuf[64];
            for (; i != e; ++i)
            {
                sprintf(xbuf, "%.6f", (*i).x); sprintf(ybuf, "%.6f", (*i).y);
                s << type << " " << xbuf << " " << ybuf << " " << std::endl;
            }
        }
    }
};

class ObjGroup
{
    void m_face(const Face &face, ObjFace &oface)
    {
        const std::vector<Vertex> &v = face.vertices;
        std::vector<Vertex>::const_iterator i = v.begin(), e = v.end();
        Vertex wv;
        for (; i != e; ++i)
        {
            wv = *i;
            // $TODO this needs to be an option
            //wv.setnormal(face.normal);
            oface.insert(index.insert(wv));
        }
    }
    void m_facelist(const FaceList &flist)
    {
        const std::vector<Face> &fl = flist.faces;
        std::vector<Face>::const_iterator i = fl.begin(), e = fl.end();
        for (; i != e; ++i)
        {
            ObjFace *face = new ObjFace;
            m_face((*i), *face);
            faces.push_back(face);
        }                
    }
    
public:

    std::string            name;
    ObjIndex               index;
    std::vector<ObjFace *> faces;
    
    ObjGroup(const char *name, const FaceList &faces) : name(name ? name : "")
    {
        m_facelist(faces);
    }
    ~ObjGroup()
    {
        std::vector<ObjFace *>::const_iterator i = faces.begin(), e = faces.end();
        for (; i != e; ++i) delete (*i);
        faces.clear();                
    }
    std::ostream &print(const ObjOption &option, int smoothgroup, std::ostream &s) const
    {
        if (!name.empty())
            s << "g " << name << std::endl << std::endl;
        if (!option.smooth)
            s << "s off" << std::endl << std::endl;
        else if (!option.normal)
            s << "s " << smoothgroup << std::endl << std::endl;
        index.print(option, s);
        s << std::endl;
        std::vector<ObjFace *>::const_iterator i = faces.begin(), e = faces.end();
        int count = 0;
        for (; i != e; ++i)
        {
            (*i)->print(option, s);
            count++;
        }
        s << "# " << count << " faces" << std::endl << std::endl;
        return s;
    }    
};

ObjFile::~ObjFile()
{
    std::vector<ObjGroup *>::const_iterator i = m_groups.begin(), e = m_groups.end();
    for (; i != e; ++i) delete (*i);
    m_groups.clear();
}

void ObjFile::append(const char *name, const FaceList &faces)
{
    m_groups.push_back(new ObjGroup(name, faces));
}

std::ostream &ObjFile::print(const ObjOption &option, std::ostream &s) const
{
    int sgroup = 1;
    std::vector<ObjGroup *>::const_iterator i = m_groups.begin(), e = m_groups.end();
    for (; i != e; ++i) (*i)->print(option, sgroup++, s);
    return s;
}

void ObjFile::write(const char *filename, const ObjOption &option, const FaceList &faces)
{
    std::ofstream file(filename, std::ios::trunc);
    if (!file)
        throw std::exception("unable to open output file");
    ObjFile obj;
    obj.append("object", faces);
    obj.print(option, file);
    file.close();
}