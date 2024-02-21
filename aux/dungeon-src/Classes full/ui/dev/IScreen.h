/*
 *  IScreen.h
 *  GLGravity
 *
 *  Created by Paul Senzee on 1/9/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _ISCREEN_H
#define _ISCREEN_H

#include "core/xna.h"
#include "GameClock.h"

class OverheadCamera;
class GameUI;

class IScreen
{
public:
        
    virtual ~IScreen() {}
        
    virtual void Set(GameUI &ui)                                                           = 0;
    virtual bool HandleTouch(const core::Point &point, const OverheadCamera &camera)       = 0;
    virtual bool IsVisible()                                                         const = 0;
    virtual void SetVisible(bool v)                                                        = 0;
};

#endif // _ISCREEN_H