#ifndef _POLYGON_H
#define _POLYGON_H

#include <set>
#include <map>
#include <vector>
#include "line.h"
#include "linelist.h"
#include "point.h"

class Polygon
{
    LineList                  m_list;
	const std::vector<Point> &m_points;

public:

    Polygon(const LineList &list, const std::vector<Point> &points) : m_list(list), m_points(points) {}

	bool isSimple(void)
	{
	    if (m_list.size() > 2)
		{
	        std::vector<int> &ls = m_list.list;
		    Point qn((m_points[ls[1]] - m_points[ls[0]]).normalize()),
				  pn((m_points[ls[2]] - m_points[ls[1]]).normalize());
		    for (int i = 3, sz = m_list.size(); i < sz; i++)
		    {
				pn = (m_points[ls[i]] - m_points[ls[i - 1]]).normalize();
				if (pn.dot(qn) < 0)
				    return false;
				qn = pn;
		    }
			return true;
        }
		return false;
	}
};

#endif  /** _POLYGON_H */
