/*
 *  IEventListener.h
 *  GLGravity
 *
 *  Created by Paul Senzee on 12/29/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _IEVENTLISTENER_H
#define _IEVENTLISTENER_H

class IEventListener
{
public:
        
    virtual ~IEventListener() {}
        
    virtual bool Notify(const char *event) = 0;
};

#endif // _IEVENTLISTENER_H