#ifndef _POINTINSERTERT_H
#define _POINTINSERTERT_H

#include "math.h"
#include "tuple3t.h"

#include <iterator>
#include <algorithm>
#include <vector>

template <class T>
class PointInserterT
{
public:

    typedef Tuple3T<T>         value_type;
    typedef Tuple3T<T> &       reference;
    typedef const Tuple3T<T> & const_reference;

    std::vector<value_type> &points;

    inline PointInserterT(std::vector<value_type> &points) : points(points) {}

	inline void push_back(const value_type &p)
	{
	    size_t sz = points.size();
	    if (sz == 0)
		    points.push_back(p);
		else if (sz == 1)
		{
		    if (points.back() != p)
                points.push_back(p);
		}
		else
		{
	        if (points.back() != p)
			{
		        value_type a  = points[sz - 2],
			               b  = points.back(),
				           ab = a - b,
				           c  = (p - b).cross(ab);
		        if (math::eq(c.lengthsq(), (T)0.0))
			        points.pop_back();
                points.push_back(p);
            }
		}
	}
	static inline void copy(const std::vector<value_type> &from, std::vector<value_type> &to)
	{
        PointInserterT<T> ins(to);
		std::copy(from.begin(), from.end(), std::back_insert_iterator< PointInserterT<T> >(ins));
	}
	static inline void clean(std::vector<value_type> &points)
	{
	    std::vector<value_type> pcopy;
		copy(points, pcopy);
		points = pcopy;
	}
};

#endif  /** _POINTINSERTERT_H */
