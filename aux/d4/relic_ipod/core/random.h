#ifndef _RANDOM_H
#define _RANDOM_H

namespace core
{
    
class Random
{
public:
            
    inline Random(int seed) : state(seed) {}
            
    inline int integer()
    {
        return ((state = state * 214013 + 2531011) >> 16) & 0x7fff;
    }
    inline double real()
    {
        static const double FACTOR = 1.0 / static_cast<double>(0x7fff);
        return FACTOR * integer();
    }
    inline float realf()
    {
        static const float FACTOR = 1.0f / static_cast<float>(0x7fff);
        return FACTOR * integer();
    }
            
private:
            
    int state;
};
    
}

#endif // _RANDOM_H
