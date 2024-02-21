#include "wavefront.h"
#include "rendermesh.h"
#include "format.h"

#include <stdio.h>
#include <ctype.h>
#include <math.h>
#include <vector>

static std::string read_identifier(const char **s)
{
    std::string str;
    while (isspace(**s) && **s != '\n' && **s != '\r') (*s)++;
    if (!**s || **s == '\n' || **s == '\r')
        return str;
    while (!isspace(**s))
    {
        str += **s;        
        (*s)++;
    }
    return str;
}

static unsigned QNAN_FOUND = 0;

static bool read(const char **s, float &u)
{
    while (isspace(**s)) (*s)++;
	const char *ends = *s;
    u = (float)strtod(*s, const_cast<char **>(&ends));
	if (ends && ends != *s)
	{
		*s = ends;
        const char ind[] = "#IND";
        if (memicmp(*s, ind, sizeof(ind) - 1) == 0)
        {
            QNAN_FOUND++;
            u = 0; // QNaN, just set to zero for now..
            *s += (sizeof(ind) - 1);
            while (**s == '0') (*s)++;
        }
		return true;
	}
	return false;
}

static bool read(const char **s, int &u)
{
    while (isspace(**s)) (*s)++;
	const char *ends = *s;
    u = (int)strtoul(*s, const_cast<char **>(&ends), 10);
	if (ends && ends != *s)
	{
		*s = ends;
		return true;
	}
	return false;
}

static inline bool read_point(const char *s, Tuple3f &p)
{
	return read(&s, p.x) && read(&s, p.y) && read(&s, p.z);
}

static inline bool read_point(const char *s, Tuple2f &p)
{
	return read(&s, p.x) && read(&s, p.y);
}

static bool read_ids(const char **s, int &v, int &n, int &t)
{
	v = -1;
	if (!read(s, v) || *(*s)++ != '/')
        return false;
	t = -1;
	if (**s != '/' && !read(s, t))
		return false;
	n = -1;
	char c = *(*s)++;
	if (isspace(c))
		return true;
	else if (c == '/' && read(s, n))
		return true;
	return false;
}

struct Closer
{
    FILE *file;
    inline Closer(FILE *file) : file(file) {}
    inline ~Closer() { fclose(file); }
};

void CreateListMesh(RenderMesh &mesh, RenderMeshSection *section, const std::vector<WavefrontVertex> &tris)
{
    enum { VSIZE = sizeof(WavefrontVertex) };
    unsigned count = (unsigned)tris.size();
    Buffer va(new unsigned char [VSIZE * count], VSIZE, count);
    memcpy(va.data, (const void *)&tris[0], va.size);
    section->geometry = Geometry(GeometryType::TRIANGLE_LIST, va);
    mesh.sections.push_back(*section);
}

void CreateFullVertexList(std::vector<WavefrontVertex> &vertices, const std::vector<Tuple3f> &positions, const std::vector<Tuple3f> &normals, const std::vector<Tuple2f> &uvs)
{
    size_t psz = positions.size(),
           nsz = normals.size(),
           usz = uvs.size();           
    if (!psz)
        return;
    for (size_t i = 0; i < psz; i++)
    {
        WavefrontVertex v;
        v.vertex = positions[i];
        if (nsz > 0) v.normal = normals[i % nsz];
        if (usz > 0) v.uv     = uvs[i % usz];
        vertices.push_back(v);
    }
}

void CreateStripMesh(RenderMesh &mesh, RenderMeshSection *section, const std::vector<int> &indices, const std::vector<Tuple3f> &positions, const std::vector<Tuple3f> &normals, const std::vector<Tuple2f> &uvs)
{
    enum { VSIZE = sizeof(WavefrontVertex), ISIZE = sizeof(int) };
    std::vector<WavefrontVertex> vertices;
    CreateFullVertexList(vertices, positions, normals, uvs);
    unsigned vcount = (unsigned)vertices.size(), icount = (unsigned)indices.size();
    Buffer va(new unsigned char [VSIZE * vcount], VSIZE, vcount);
    memcpy(va.data, (const void *)&vertices[0], va.size);
    Buffer ia(new unsigned char [ISIZE * icount], ISIZE, icount);
    memcpy(ia.data, (const void *)&indices[0], ia.size);
    section->geometry = Geometry(GeometryType::TRIANGLE_STRIP, va, ia);
    mesh.sections.push_back(*section);
}

bool WavefrontReader::read(const char *filename, RenderMesh &mesh) const
{
	FILE *file = fopen(filename, "r");

	if (!file)
    {
        printf("Unable to open file '%s' for reading!", filename);
		return false;
    }

    Closer closer(file);

    const int linesz = 1024;

	char line[linesz + 1];

	std::vector< Tuple3f > vertices, normals;
	std::vector< Tuple2f > uvs;
	Tuple3f point3;
	Tuple2f point2;

	std::vector<WavefrontVertex> tris;
	std::vector<int>             indices;

    RenderMeshSection *msection = new RenderMeshSection;

	while (fgets(line, linesz, file))
	{
        const char *p = line;
		if (*p == '#' || isspace(*p))
			continue;
        else if (strncmp(p, "usemtl", sizeof("usemtl") - 1) == 0)
        {
            p += sizeof("usemtl") - 1;
            msection->material = read_identifier(&p);
            continue;
        }
        else if (*p == 'g')
        {
            if (!tris.empty() || !indices.empty())
            {
                // flush existing
                if (!tris.empty())
                    CreateListMesh(mesh, msection, tris);
                else if (!indices.empty())
                    CreateStripMesh(mesh, msection, indices, vertices, normals, uvs);
                delete msection;
                // create new
                msection = new RenderMeshSection;
            }
            // set name
            p++;
            msection->name = read_identifier(&p);
            // reset data
            tris.clear();
            indices.clear();
            continue;
        }
        else if (*p == 'o' || *p == 's') // skippable stuff here..
            continue;
		else if (*p == 'v')
		{
			p++;
			char c = *p++;
			while (isspace(*p)) p++;
			switch (c)
			{
			case 't': if (!read_point(p, point2)) return false;      uvs.push_back(point2); break;
            case 'n': if (!read_point(p, point3)) return false;  normals.push_back(point3); break;
            case ' ': if (!read_point(p, point3)) return false; vertices.push_back(point3); break;
			}
		}
		else if (*p == 'f')
		{
			p++;
            Tuple3f mt3; Tuple2f mt2;
			int vid = -1, nid = -1, tid = -1, count = 0;
            WavefrontVertex wv, first, previous;
			while (read_ids(&p, vid, nid, tid))
			{
				if (vid - 1 >= (int)vertices.size() ||
                    nid - 1 >= (int) normals.size() ||
                    tid - 1 >= (int)     uvs.size())
					break; // $todo this is an error!!!

				wv.vertex = vid == -1 ? mt3 : vertices[vid - 1];
				wv.normal = nid == -1 ? mt3 :  normals[nid - 1];
				wv.uv     = tid == -1 ? mt2 :      uvs[tid - 1];

				if (!count)
					first = wv;
				// add to 'vertex' array..
				if (count > 2)
				{
					tris.push_back(first);
					tris.push_back(previous);
				}
                tris.push_back(wv);
				vid = -1; nid = -1; tid = -1; count++;
				previous = wv;
			}
            if (count < 3)
            {
                printf("WARNING: read face with less than three vertices.\r\n");
                while (count--) tris.pop_back();
            }
		}
	    /*
        		    
        t 10 9 8     # Start of triangle strip 1 
        q 4          # Continuation of triangle strip 1 
        q 2          # Continuation of triangle strip 1 
        q 3 1        # Continuation of triangle strip 1 
        
        t 3 4 3 5    # Start of triangle strip 2 
        q 6 7        # Continuation of triangle strip 2
        
        ///
        
        start:
        when t starts
          - if not first
            - dupe the last index of the last strip
            - dupe the first index of this strip
          - add all the indices from t
          - and subsequent q(s) until the next t or other marker is found
          - if t is found goto start

        */
		else if (*p == 't' || *p == 'q') // for the STRIPE .objf format
		{
		    char type = *p;
		    // start/continue a strip
		    p++;
		    
		    int index = 0, count = 0;
            while (::read(&p, index))
		    {
		        index--; // $HUGE BUG!  indices in objf are indexed from 1, here from zero!		        
		        if (type == 't' && count == 0 && !indices.empty())  // we're starting a new strip
		        {
//		            indices.push_back(indices[indices.size() - 1]); // dupe the last one
//		            indices.push_back(index);                       // dupe the next one
		        }		        
		        indices.push_back(index - 1); // $HUGE BUG!!!!! ok, stripping wasn't working because this needed to be index ****-1*****, because the indices are 1-based!!!
		        count++;
		    }
		}
	}
    if      (!tris.empty())
        CreateListMesh (mesh, msection, tris);
    else if (!indices.empty())
        CreateStripMesh(mesh, msection, indices, vertices, normals, uvs);
    delete msection;

    if (QNAN_FOUND)
        printf("WARNING: %u QNaN(s) found in vertex data!\n", QNAN_FOUND);
    if (!indices.empty())
        printf("read %d triangles/%d indices/%d vertices from file %s\r\n", indices.size() - 2, indices.size(), vertices.size(), filename);
    else
        printf("read %d triangles/%d vertices from file %s\r\n", tris.size() / 3, tris.size(), filename);

    tris.clear();
    indices.clear();
	return true;
}