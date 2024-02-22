#ifndef _RECTANGLE_H
#define _RECTANGLE_H
    
class Rectangle
{
public:    

    int x, y, width, height;

    inline Rectangle(int x, int y, int w, int h) : x(x), y(y), width(w), height(h) {}
    inline Rectangle() : x(0), y(0), width(0), height(0) {}
    inline Rectangle(const Rectangle &r) : x(r.x), y(r.y), width(r.width), height(r.height) {}
};
    
#endif // _RECTANGLE_H