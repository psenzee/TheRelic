#ifndef _PLANEMAP_H
#define _PLANEMAP_H

#include "plane.h"
#include "triangle.h"
#include "triangleidlist.h"

class PlaneMap
{
    typedef std::map<Plane, TriangleIdList *> map_t;

    const SimpleMesh  *m_mesh;
    map_t              m_map;
	std::vector<Plane> m_list;

public:

    PlaneMap()                       : m_mesh(NULL) {}
    PlaneMap(const SimpleMesh *mesh) : m_mesh(mesh) { create(); }
	virtual ~PlaneMap()                             { clear();  }

    const std::vector<Plane> &getList(void)         { return m_list; }
    const map_t              &getMap(void)          { return m_map; }
    const SimpleMesh         *getMesh(void)         { return m_mesh; }

	void getPlanes(std::vector<Plane> &list)
	{
	    for (map_t::iterator it = m_map.begin(), end = m_map.end(); it != end; ++it)
	        list.push_back((*it).first);
	}
	void getTriangles(std::vector<TriangleIdList *> &list)
	{
	    for (map_t::iterator it = m_map.begin(), end = m_map.end(); it != end; ++it)
	        list.push_back((*it).second);
	}
	inline const Plane          &getPlane(int id)             { return m_list[id]; }
	inline int                   getPlaneCount(void)          { return m_list.size(); }
	inline const TriangleIdList *getTriangles(const Plane &p) { return m_map[p]; }

	void getIntersections(std::vector<PointLine> &lines)
	{
	    int sz = m_list.size();
		if (sz == 0)
		    return;
		PointLine line;
	    for (int i = 0; i < sz; i++)
		{
		    Plane &a = m_list[i];
		    for (int j = 0; j < sz; j++)
			{
			    Plane &b = m_list[j];
				if (a.normal != b.normal && a.intersects(line, b))
                    lines.push_back(line);
			}
		}
	}

    void clear(void)
	{
	    for (map_t::iterator it = m_map.begin(), end = m_map.end(); it != end; ++it)
	        delete (*it).second;
		m_map.clear();
		m_list.clear();
	}
	void set(SimpleMesh *mesh)
	{
	    m_mesh = mesh;
		create();
	}

private:

    void create(void)
    {
	    clear();
	    if (m_mesh != NULL)
		{
            Plane           n;
		    TriangleIdList *tl = NULL;
	        for (int i = 0, sz = m_mesh->triangles.size(); i < sz; i++)
	        {
	            const Triangle &tri = m_mesh->triangles[i];
		        n = tri.plane(m_mesh->vertices);
		        if ((tl = m_map[n]) == NULL)
			    {
		            m_map[n] = tl = new TriangleIdList;
				    m_list.push_back(n);
			    }
		        tl->add(i);
	        }
		}
    }
};

#endif  /** _PLANEMAP_H */
