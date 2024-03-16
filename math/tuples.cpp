#include "tuples.h"

#include <float.h>
#include <math.h>

#include "pointinsertert.h"

#include <vector>

template <class T>
class _tuples
{
public:

    typedef Tuple3T<T>          tup3_t;
	typedef std::vector<tup3_t> tup3vec_t;

    static int maxarea(const tup3_t &a, const tup3_t &b, const tup3vec_t &vs)
    {
        T    dm = -FLT_MAX, f = 0;
        int  mx = -1;
        for (int i = 0, sz = vs.size(); i < sz; i++)
	        if ((f = Tuples::areasq2(a, b, vs[i])) > dm) { mx = i; dm = f; }
        return mx;
    }
    static int minarea(const tup3_t &a, const tup3_t &b, const tup3vec_t &vs)
    {
        T    dm = FLT_MAX, f = 0;
        int  mn = -1;
        for (int i = 0, sz = vs.size(); i < sz; i++)
    	    if ((f = Tuples::areasq2(a, b, vs[i])) < dm) { mn = i; dm = f; }
        return mn;
    }
    static int furthest(const tup3_t &a, const tup3vec_t &vs)
    {
        T    dm = 0, f = 0;
        int	 mx = -1;
        for (int i = 0, sz = vs.size(); i < sz; i++)
        {
	        f = (vs[i] - a).lengthsq();
	        if (f > dm) { mx = i; dm = f; }
        }
        return mx;
    }
    static tup3_t average(const tup3vec_t &vs)
    {
        tup3_t a(0, 0, 0);
        if (vs.size() > 0)
        {
            for (int i = 0, sz = vs.size(); i < sz; i++)
                a += vs[i];
            a /= T(vs.size());
        }
        return a;
    }
    static inline tup3_t random(const tup3_t &scale, const tup3_t &trans)
    {
        return tup3_t((T(rand()) / T(RAND_MAX)) * scale.x + trans.x,
                      (T(rand()) / T(RAND_MAX)) * scale.y + trans.y,
	                  (T(rand()) / T(RAND_MAX)) * scale.z + trans.z);
    }
    static void random(int count, const tup3_t &scale, const tup3_t &trans, tup3vec_t &ps)
    {
        for (int i = 0; i < count; i++)
    	    ps.push_back(random(scale, trans));
    }
    static void extremes(typename tup3vec_t::const_iterator begin, typename tup3vec_t::const_iterator end,
                         tup3_t &minp, tup3_t &maxp)
    {
        tup3vec_t::const_iterator i = begin, e = end;
        tup3_t minq(FLT_MAX, FLT_MAX, FLT_MAX), maxq(-FLT_MAX, -FLT_MAX, -FLT_MAX);
        for (; i != e; ++i) { minq = Tuples::mins(*i, minq); maxq = Tuples::maxs(*i, maxq); }
        minp = minq; maxp = maxq;
    }
	static bool convex(const tup3vec_t &v)
    {
        tup3_t c, co;
	    c = Tuples::crossat(v, 0);
        for (int i = 1, sz = v.size(); i < sz; i++)
	    {
	        co = c; c = Tuples::crossat(v, i);
		    if (c.dot(co) < 0) return false;
	    }
	    return true;
    }
    static inline void clean(std::vector< Tuple3T<T> > &v)
    {
	    PointInserterT<T>::clean(v);
    }
};

int             Tuples::maxarea(const Tuple3T<float> &a, const Tuple3T<float> &b, const std::vector< Tuple3T<float> > &v)    { return _tuples<float>::maxarea(a, b, v); }
int             Tuples::maxarea(const Tuple3T<double> &a, const Tuple3T<double> &b, const std::vector< Tuple3T<double> > &v) { return _tuples<double>::maxarea(a, b, v); }
int             Tuples::maxarea(const Tuple3T<fix> &a, const Tuple3T<fix> &b, const std::vector< Tuple3T<fix> > &v)          { return _tuples<fix>::maxarea(a, b, v); }

int             Tuples::minarea(const Tuple3T<float> &a, const Tuple3T<float> &b, const std::vector< Tuple3T<float> > &v)    { return _tuples<float>::minarea(a, b, v); }
int             Tuples::minarea(const Tuple3T<double> &a, const Tuple3T<double> &b, const std::vector< Tuple3T<double> > &v) { return _tuples<double>::minarea(a, b, v); }
int             Tuples::minarea(const Tuple3T<fix> &a, const Tuple3T<fix> &b, const std::vector< Tuple3T<fix> > &v)          { return _tuples<fix>::minarea(a, b, v); }

int             Tuples::furthest(const Tuple3T<float> &a, const std::vector< Tuple3T<float> > &v)                            { return _tuples<float>::furthest(a, v); }
int             Tuples::furthest(const Tuple3T<double> &a, const std::vector< Tuple3T<double> > &v)                          { return _tuples<double>::furthest(a, v); }
int             Tuples::furthest(const Tuple3T<fix> &a, const std::vector< Tuple3T<fix> > &v)                                { return _tuples<fix>::furthest(a, v); }

Tuple3T<float>  Tuples::average(const std::vector< Tuple3T<float> > &v)                                                      { return _tuples<float>::average(v); }
Tuple3T<double> Tuples::average(const std::vector< Tuple3T<double> > &v)                                                     { return _tuples<double>::average(v); }
Tuple3T<fix>    Tuples::average(const std::vector< Tuple3T<fix> > &v)                                                        { return _tuples<fix>::average(v); }

Tuple3T<float>  Tuples::random(const Tuple3T<float> &s, const Tuple3T<float> &t)                                             { return _tuples<float>::random(s, t); }
Tuple3T<double> Tuples::random(const Tuple3T<double> &s, const Tuple3T<double> &t)                                           { return _tuples<double>::random(s, t); }
Tuple3T<fix>    Tuples::random(const Tuple3T<fix> &s, const Tuple3T<fix> &t)                                                 { return _tuples<fix>::random(s, t); }

void            Tuples::random(int c, const Tuple3T<float> &s, const Tuple3T<float> &t, std::vector< Tuple3T<float> > &p)    { _tuples<float>::random(c, s, t, p); }
void            Tuples::random(int c, const Tuple3T<double> &s, const Tuple3T<double> &t, std::vector< Tuple3T<double> > &p) { _tuples<double>::random(c, s, t, p); }
void            Tuples::random(int c, const Tuple3T<fix> &s, const Tuple3T<fix> &t, std::vector< Tuple3T<fix> > &p)          { _tuples<fix>::random(c, s, t, p); }

void            Tuples::extremes(std::vector< Tuple3T<float> >::const_iterator b, std::vector< Tuple3T<float> >::const_iterator e,
                                 Tuple3T<float> &minp, Tuple3T<float> &maxp)                                                 { _tuples<float>::extremes(b, e, minp, maxp); }
void            Tuples::extremes(std::vector< Tuple3T<double> >::const_iterator b, std::vector< Tuple3T<double> >::const_iterator e,
                                 Tuple3T<double> &minp, Tuple3T<double> &maxp)                                               { _tuples<double>::extremes(b, e, minp, maxp); }
void            Tuples::extremes(std::vector< Tuple3T<fix> >::const_iterator b, std::vector< Tuple3T<fix> >::const_iterator e,
                                 Tuple3T<fix> &minp, Tuple3T<fix> &maxp)                                                     { _tuples<fix>::extremes(b, e, minp, maxp); }

bool            Tuples::convex(const std::vector< Tuple3T<float> > &v)                                                       { return _tuples<float>::convex(v); }
bool            Tuples::convex(const std::vector< Tuple3T<double> > &v)                                                      { return _tuples<double>::convex(v); }
bool            Tuples::convex(const std::vector< Tuple3T<fix> > &v)                                                         { return _tuples<fix>::convex(v); }

void            Tuples::clean(std::vector< Tuple3T<float> > &v)                                                              { _tuples<float>::clean(v); }
void            Tuples::clean(std::vector< Tuple3T<double> > &v)                                                             { _tuples<double>::clean(v); }
void            Tuples::clean(std::vector< Tuple3T<fix> > &v)                                                                { _tuples<fix>::clean(v); }
