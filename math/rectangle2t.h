#ifndef _RECTANGLE2T_H
#define _RECTANGLE2T_H

#include "point2t.h"

template <typename T>
class Rectangle2T
{
public:

    typedef Rectangle2T<T> self;
    typedef Point2T<T>     point_t;

    union
    {
        struct
        {
            point_t ul, lr;
        };
        struct
        {
            T left,
              top,
              right,
              bottom;
        };
    };

    inline Rectangle2T()                                                          {}
    inline Rectangle2T(const self &r)                        : ul(r.ul), lr(r.lr) {}
    inline Rectangle2T(const point_t &ul, const point_t &lr) : ul(ul), lr(lr)     {}
    inline Rectangle2T(int x, int y, int u, int v)           : ul(x, y), lr(u, v) {}

    inline int            width(void)                           const { return lr.x - ul.x; }
    inline int            height(void)                          const { return lr.y - ul.y; }
    inline point_t        size(void)                            const { return point_t(width(), height()); }
    inline void           setSize(const point_t &s)                   { lr.x = ul.x + s.x; lr.y = ul.y + s.y; }
    inline void           set(const point_t &a, const point_t &b)     { ul = a; lr = b; }
    inline void           set(int x, int y, int u, int v)             { ul.x = x; ul.y = y; lr.x = u; lr.y = v; }

    inline self          &operator+=(const point_t &p)                { r.ul += p; r.lr += p; return *this; }
    inline self          &operator-=(const point_t &p)                { r.ul -= p; r.lr -= p; return *this; }

    inline self           operator+ (const point_t &p)                { self r(*this); r += p; return r; }
    inline self           operator- (const point_t &p)                { self r(*this); r -= p; return r; }

    inline bool           inside(const point_t &p)                    { return p.x >= ul.x && p.x < lr.x && p.y >= ul.y && p.y < lr.y; }
	inline void           normalize(void)                             { point_t a = point_t::minimum(ul, lr), b = point_t::maximum(ul, lr); ul = a; lr = b; }
	inline void           read(std::istream &s)                       { ul.read(s);  lr.read(s); }
	inline void           write(std::ostream &s)                const { ul.write(s); lr.write(s); }

    // utilities..
	static inline self getIntersection(const self &a, const self &b)
	{
	    self ra(a), rb(b); ra.normalize(); rb.normalize();
	    return self(point_t::minimum(ra.ul, rb.ul), point_t::minimum(ra.lr, rb.lr));
	}
	static inline self getUnion(const self &a, const self &b)
	{
	    self ra(a), rb(b); ra.normalize(); rb.normalize();
	    return self(point_t::maximum(ra.ul, rb.ul), point_t::maximum(ra.lr, rb.lr));
	}
};

#endif  /** _RECTANGLE2T_H */