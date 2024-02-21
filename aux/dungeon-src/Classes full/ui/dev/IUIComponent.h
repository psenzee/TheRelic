/*
 *  IUIComponent.h
 *  GLGravity
 *
 *  Created by Paul Senzee on 1/9/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _IUICOMPONENT_H
#define _IUICOMPONENT_H

#include "core/xna.h"
#include "GameClock.h"

class OverheadCamera;
class GameUI;

class IUIComponent
{
public:
        
    virtual ~IUIComponent() {}
        
    virtual void        Set(GameUI &ui)                                                           = 0;
    virtual core::Size  GetSize()                                                           const = 0;
    virtual core::Point GetOrigin()                                                         const = 0;
    virtual bool        HandleTouch(const core::Point &point, const OverheadCamera &camera)       = 0;
    virtual bool        IsVisible()                                                         const = 0;
    virtual void        SetVisible(bool v)                                                        = 0;
};

#endif // _IUICOMPONENT_H