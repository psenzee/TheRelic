#ifndef _POINT_H
#define _POINT_H

namespace core
{

class Point
{
public:    

    int32_t x, y;

    inline Point(int32_t x = 0, int32_t y = 0) : x(x), y(y) {}
    
    inline Point Minimum(const Point &p) const { return Point(p.x < x ? p.x : x, p.y < y ? p.y : y); }
    inline Point Maximum(const Point &p) const { return Point(p.x > x ? p.x : x, p.y > y ? p.y : y); } 

    inline bool operator==(const Point &p) const { return p.x == x && p.y == y; }
    inline bool operator!=(const Point &p) const { return p.x != x || p.y != y; }
};
    
}
    
#endif // _POINT_H
