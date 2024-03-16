#ifndef _TUPLES_H
#define _TUPLES_H

#include "math.h"
#include "tuple3t.h"

#include <iostream>
#include <float.h>
#include <math.h>

class Tuples
{
public:

	static inline const Tuple3T<float>  &zeros()                                                                                                         { static Tuple3T<float>  t(0, 0, 0); return t; }
	static inline const Tuple3T<double> &zerosd()                                                                                                        { static Tuple3T<double> t(0, 0, 0); return t; }
	static inline const Tuple3T<fix>    &zerosfp()                                                                                                       { static Tuple3T<fix>    t(0, 0, 0); return t; }
    static inline const Tuple3T<float>  &ones()                                                                                                          { static Tuple3T<float>  t(1, 1, 1); return t; }
    static inline const Tuple3T<double> &onesd()                                                                                                         { static Tuple3T<double> t(1, 1, 1); return t; }
    static inline const Tuple3T<fix>    &onesfp()                                                                                                        { static Tuple3T<fix>    t(1, 1, 1); return t; }
    static inline const Tuple3T<float>  &twos()                                                                                                          { static Tuple3T<float>  t(2, 2, 2); return t; }
    static inline const Tuple3T<double> &twosd()                                                                                                         { static Tuple3T<double> t(2, 2, 2); return t; }
    static inline const Tuple3T<fix>    &twosfp()                                                                                                        { static Tuple3T<fix>    t(2, 2, 2); return t; }
    static inline Tuple3T<float>         random()                                                                                                        { return random(ones(), zeros()); }
    static inline Tuple3T<double>        randomd()                                                                                                       { return random(onesd(), zerosd()); }
    static inline Tuple3T<fix>           randomfp()                                                                                                      { return random(onesfp(), zerosfp()); }

    static inline Tuple3T<float>         mins(const Tuple3T<float> &a, const Tuple3T<float> &b)                                                          { return Tuple3T<float>(a.x < b.x ? a.x : b.x, a.y < b.y ? a.y : b.y, a.z < b.z ? a.z : b.z); }
    static inline Tuple3T<double>        mins(const Tuple3T<double> &a, const Tuple3T<double> &b)                                                        { return Tuple3T<double>(a.x < b.x ? a.x : b.x, a.y < b.y ? a.y : b.y, a.z < b.z ? a.z : b.z); }
    static inline Tuple3T<fix>           mins(const Tuple3T<fix> &a, const Tuple3T<fix> &b)                                                              { return Tuple3T<fix>(a.x < b.x ? a.x : b.x, a.y < b.y ? a.y : b.y, a.z < b.z ? a.z : b.z); }
    static inline Tuple3T<float>         maxs(const Tuple3T<float> &a, const Tuple3T<float> &b)                                                          { return Tuple3T<float>(a.x > b.x ? a.x : b.x, a.y > b.y ? a.y : b.y, a.z > b.z ? a.z : b.z); }
    static inline Tuple3T<double>        maxs(const Tuple3T<double> &a, const Tuple3T<double> &b)                                                        { return Tuple3T<double>(a.x > b.x ? a.x : b.x, a.y > b.y ? a.y : b.y, a.z > b.z ? a.z : b.z); }
    static inline Tuple3T<fix>           maxs(const Tuple3T<fix> &a, const Tuple3T<fix> &b)                                                              { return Tuple3T<fix>(a.x > b.x ? a.x : b.x, a.y > b.y ? a.y : b.y, a.z > b.z ? a.z : b.z); }
	static inline Tuple3T<float>         facenormal(const Tuple3T<float> &a, const Tuple3T<float> &b, const Tuple3T<float> &c)                           { return (b - a).cross(c - a); }
	static inline Tuple3T<double>        facenormal(const Tuple3T<double> &a, const Tuple3T<double> &b, const Tuple3T<double> &c)                        { return (b - a).cross(c - a); }
	static inline Tuple3T<fix>           facenormal(const Tuple3T<fix> &a, const Tuple3T<fix> &b, const Tuple3T<fix> &c)                                 { return (b - a).cross(c - a); }
    static inline float                  areasq2(const Tuple3T<float> &a, const Tuple3T<float> &b, const Tuple3T<float> &c)                              { return facenormal(a, b, c).lengthsq(); }
    static inline double                 areasq2(const Tuple3T<double> &a, const Tuple3T<double> &b, const Tuple3T<double> &c)                           { return facenormal(a, b, c).lengthsq(); }
    static inline fix                    areasq2(const Tuple3T<fix> &a, const Tuple3T<fix> &b, const Tuple3T<fix> &c)                                    { return facenormal(a, b, c).lengthsq(); }
    static inline float                  area(const Tuple3T<float> &a, const Tuple3T<float> &b, const Tuple3T<float> &c)                                 { return facenormal(a, b, c).length() * 0.5f; }
    static inline double                 area(const Tuple3T<double> &a, const Tuple3T<double> &b, const Tuple3T<double> &c)                              { return facenormal(a, b, c).length() * 0.5; }
    static inline fix                    area(const Tuple3T<fix> &a, const Tuple3T<fix> &b, const Tuple3T<fix> &c)                                       { static fix half(0.5); return facenormal(a, b, c).length() * half; }
    static inline Tuple3T<float>         average(const Tuple3T<float> &a, const Tuple3T<float> &b)                                                       { return Tuple3T<float>((a.x + b.x) * 0.5f, (a.y + b.y) * 0.5f, (a.z + b.z) * 0.5f); }
    static inline Tuple3T<double>        average(const Tuple3T<double> &a, const Tuple3T<double> &b)                                                     { return Tuple3T<double>((a.x + b.x) * 0.5, (a.y + b.y) * 0.5, (a.z + b.z) * 0.5); }
    static inline Tuple3T<fix>           average(const Tuple3T<fix> &a, const Tuple3T<fix> &b)                                                           { static fix half(0.5); return Tuple3T<fix>((a.x + b.x) * half, (a.y + b.y) * half, (a.z + b.z) * half); }
    static inline Tuple3T<float>         average(const Tuple3T<float> &a, const Tuple3T<float> &b, const Tuple3T<float> &c)                              { static float third = 1.0f / 3.0f; return Tuple3T<float>((a.x + b.x + c.x) * third, (a.y + b.y + c.y) * third, (a.z + b.z + c.z) * third); }
    static inline Tuple3T<double>        average(const Tuple3T<double> &a, const Tuple3T<double> &b, const Tuple3T<double> &c)                           { static double third = 1.0 / 3.0;   return Tuple3T<double>((a.x + b.x + c.x) * third, (a.y + b.y + c.y) * third, (a.z + b.z + c.z) * third); }
    static inline Tuple3T<fix>           average(const Tuple3T<fix> &a, const Tuple3T<fix> &b, const Tuple3T<fix> &c)                                    { static fix third(1.0 / 3.0);    return Tuple3T<fix>((a.x + b.x + c.x) * third, (a.y + b.y + c.y) * third, (a.z + b.z + c.z) * third); }
    static inline Tuple3T<float>         average(const Tuple3T<float> &a, const Tuple3T<float> &b, const Tuple3T<float> &c, const Tuple3T<float> &d)     { return Tuple3T<float>((a.x + b.x + c.x + d.x) * 0.25f, (a.y + b.y + c.y + d.y) * 0.25f, (a.z + b.z + c.z + d.z) * 0.25f); }
    static inline Tuple3T<double>        average(const Tuple3T<double> &a, const Tuple3T<double> &b, const Tuple3T<double> &c, const Tuple3T<double> &d) { return Tuple3T<double>((a.x + b.x + c.x + d.x) * 0.25, (a.y + b.y + c.y + d.y) * 0.25, (a.z + b.z + c.z + d.z) * 0.25); }
    static inline Tuple3T<fix>           average(const Tuple3T<fix> &a, const Tuple3T<fix> &b, const Tuple3T<fix> &c, const Tuple3T<fix> &d)             { static fix quart(0.25); return Tuple3T<fix>((a.x + b.x + c.x + d.x) * quart, (a.y + b.y + c.y + d.y) * quart, (a.z + b.z + c.z + d.z) * quart); }
    static inline Tuple3T<float>         crossat(const std::vector< Tuple3T<float> >  &v, int i)                   	                                     { size_t sz = v.size(); return (v[(i - 1) % sz] - v[i % sz]).cross(v[(i - 1) % sz] - v[i % sz]); }
    static inline Tuple3T<double>        crossat(const std::vector< Tuple3T<double> > &v, int i)                   	                                     { size_t sz = v.size(); return (v[(i - 1) % sz] - v[i % sz]).cross(v[(i - 1) % sz] - v[i % sz]); }
    static inline Tuple3T<fix>           crossat(const std::vector< Tuple3T<fix> >    &v, int i)                   	                                     { size_t sz = v.size(); return (v[(i - 1) % sz] - v[i % sz]).cross(v[(i - 1) % sz] - v[i % sz]); }
	static inline const Tuple3T<float>  &index(const std::vector< Tuple3T<float> > &v, int i)                                                            { return v[i % v.size()]; }
	static inline const Tuple3T<double> &index(const std::vector< Tuple3T<double> > &v, int i)                                                           { return v[i % v.size()]; }
  	static inline const Tuple3T<fix>    &index(const std::vector< Tuple3T<fix> > &v, int i)                                                              { return v[i % v.size()]; }

    static inline Tuple3T<float>         tofloat(const Tuple3T<double> &t) { return Tuple3T<float>((float)t.x, (float)t.y, (float)t.z); }
    static inline Tuple3T<float>         tofloat(const Tuple3T<fix>    &t) { return Tuple3T<float>((float)t.x, (float)t.y, (float)t.z); }

    static inline Tuple3T<double>        todouble(const Tuple3T<float> &t) { return Tuple3T<double>((double)t.x, (double)t.y, (double)t.z); }
    static inline Tuple3T<double>        todouble(const Tuple3T<fix>   &t) { return Tuple3T<double>((double)t.x, (double)t.y, (double)t.z); }

    static inline Tuple3T<fix>           tofixed(const Tuple3T<float>  &t) { return Tuple3T<fix>(fix(t.x), fix(t.y), fix(t.z)); }
    static inline Tuple3T<fix>           tofixed(const Tuple3T<double> &t) { return Tuple3T<fix>(fix(t.x), fix(t.y), fix(t.z)); }

    static int                           maxarea(const Tuple3T<float> &a, const Tuple3T<float> &b, const std::vector< Tuple3T<float> > &vs);
    static int                           maxarea(const Tuple3T<double> &a, const Tuple3T<double> &b, const std::vector< Tuple3T<double> > &vs);
    static int                           maxarea(const Tuple3T<fix> &a, const Tuple3T<fix> &b, const std::vector< Tuple3T<fix> > &vs);
    static int                           minarea(const Tuple3T<float> &a, const Tuple3T<float> &b, const std::vector< Tuple3T<float> > &vs);
    static int                           minarea(const Tuple3T<double> &a, const Tuple3T<double> &b, const std::vector< Tuple3T<double> > &vs);
    static int                           minarea(const Tuple3T<fix> &a, const Tuple3T<fix> &b, const std::vector< Tuple3T<fix> > &vs);
    static int                           furthest(const Tuple3T<float> &a, const std::vector< Tuple3T<float> > &vs);
    static int                           furthest(const Tuple3T<double> &a, const std::vector< Tuple3T<double> > &vs);
    static int                           furthest(const Tuple3T<fix> &a, const std::vector< Tuple3T<fix> > &vs);
    static Tuple3T<float>                average(const std::vector< Tuple3T<float> > &vs);
    static Tuple3T<double>               average(const std::vector< Tuple3T<double> > &vs);
    static Tuple3T<fix>                  average(const std::vector< Tuple3T<fix> > &vs);
	static Tuple3T<float>                random(const Tuple3T<float> &scale, const Tuple3T<float> &trans);
	static Tuple3T<double>               random(const Tuple3T<double> &scale, const Tuple3T<double> &trans);
    static Tuple3T<fix>                  random(const Tuple3T<fix> &scale, const Tuple3T<fix> &trans);
    static void                          random(int count, const Tuple3T<float> &scale, const Tuple3T<float> &trans, std::vector< Tuple3T<float> > &ps);
    static void                          random(int count, const Tuple3T<double> &scale, const Tuple3T<double> &trans, std::vector< Tuple3T<double> > &ps);
    static void                          random(int count, const Tuple3T<fix> &scale, const Tuple3T<fix> &trans, std::vector< Tuple3T<fix> > &ps);
    static void                          extremes(std::vector< Tuple3T<float> >::const_iterator begin, std::vector< Tuple3T<float> >::const_iterator end,
	                                              Tuple3T<float> &minp, Tuple3T<float> &maxp);
    static void                          extremes(std::vector< Tuple3T<double> >::const_iterator begin, std::vector< Tuple3T<double> >::const_iterator end,
	                                              Tuple3T<double> &minp, Tuple3T<double> &maxp);
    static void                          extremes(std::vector< Tuple3T<fix> >::const_iterator begin, std::vector< Tuple3T<fix> >::const_iterator end,
  	                                              Tuple3T<fix> &minp, Tuple3T<fix> &maxp);
    static bool                          convex(const std::vector< Tuple3T<float> > &v);
    static bool                          convex(const std::vector< Tuple3T<double> > &v);
    static bool                          convex(const std::vector< Tuple3T<fix> > &v);
    static void                          clean(std::vector< Tuple3T<float> > &v);
    static void                          clean(std::vector< Tuple3T<double> > &v);
    static void                          clean(std::vector< Tuple3T<fix> > &v);
};

#endif  /** _TUPLES_H */
