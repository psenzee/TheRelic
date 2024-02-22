#ifndef _RANDOM_H
#define _RANDOM_H

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
        const double factor = 1.0 / double(0x7fff);
        return factor * integer();
    }

private:

    int state;
};

#endif // _RANDOM_H
