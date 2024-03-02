#ifndef _SIZE_H
#define _SIZE_H    

namespace core
{

class Size
{
public:    

    int32_t width, height;

    inline Size(int32_t w = 0, int32_t h = 0) : width(w), height(h) {}
};
    
}
    
#endif // _SIZE_H
