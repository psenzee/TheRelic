/*
 *  IChallenge.h
 *  GLGravity
 *
 *  Created by Paul Senzee on 1/23/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _ICHALLENGE_H
#define _ICHALLENGE_H

#include "core/xna.h"

class IChallenge
{
public:

    virtual ~IChallenge() {}
    
    virtual void        Update(const GameTime &time)       = 0;
    virtual const char *GetMessage()                 const = 0;
    virtual void        ShowTutorial()                     = 0;
};

#endif // _ICHALLENGE_H