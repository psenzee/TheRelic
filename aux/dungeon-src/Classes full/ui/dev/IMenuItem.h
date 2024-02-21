/*
 *  IMenuItem.h
 *  GLGravity
 *
 *  Created by Paul Senzee on 1/9/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _IMENUITEM_H
#define _IMENUITEM_H

#include <string>

#include "core/aabox.h"
#include "core/xna.h"
#include "GameClock.h"

class OverheadCamera;
class GameUI;
class GameInput;

class IMenuItem
{
public:

    typedef void (*function_t)(const IMenuItem &item);

    /*
    std::string text;
    function_t  function;
    void       *context;
    AABox       bounds;
    */
    
    virtual ~IMenuItem() {}
    
    virtual void Draw(GameUI &ui)                                                    = 0;
    virtual bool HandleTouch(const core::Point &point, const OverheadCamera &camera) = 0;
    virtual void SetFunction(function_t fn, void *context)                           = 0;
};

#endif // _IMENUITEM_H