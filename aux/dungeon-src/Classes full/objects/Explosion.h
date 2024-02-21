/*
 *  Explosion.h
 *  GLGravity
 *
 *  Created by Paul Senzee on 12/30/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _EXPLOSION_H
#define _EXPLOSION_H

#include "core/xna.h"
#include "time/GameClock.h"
#include <vector>

class LocalGameServices;
class OverheadCamera;
class Renderable;
class XmlReadContext;

class Explosion
{
public:

    Explosion(const Vector3 &position);
    
    void        Load(LocalGameServices *services, XmlReadContext *context, const char *type);

    void        Update(const GameTime &time);
    void        Render(const OverheadCamera &camera);

    void        Start(int time);
    
    inline bool IsDone() const { return timer.GetValue() >= 1.0f; }

private:

    struct Item
    {
        Renderable *object;
        Fader       time;
        
        inline Item(Renderable *object) : object(object), time(0.0f) {}
    };
    
    LocalGameServices         *services;
    std::vector<Item>          renderables;
    Matrix                     translate;
    Fader                      timer;
};

#endif  // _EXPLOSION_H