/*
 *  MenuItem.h
 *  GLGravity
 *
 *  Created by Paul Senzee on 1/9/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _MENUITEM_H
#define _MENUITEM_H

#include <string>

#include "core/aabox.h"
#include "core/xna.h"
#include "GameClock.h"

class OverheadCamera;
class GameUI;
class GameInput;

class MenuItem
{
public:

    typedef void (*function_t)(const MenuItem &item);
    
    std::string text;
    function_t  function;
    void       *context;    
    AABox       bounds;
    bool        active;
    
    inline MenuItem() : function(0), context(0), active(true) {}  
};

#endif // _MENUITEM_H