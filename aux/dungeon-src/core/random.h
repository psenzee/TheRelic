/*
 *  Random_.h
 *  GLGravity
 *
 *  Created by Paul Senzee on 12/29/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

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
        const double factor = 1.0 / double(0x7fff);
        return factor * integer();
    }
            
private:
            
    int state;
};
    
}

#endif // _RANDOM_H
