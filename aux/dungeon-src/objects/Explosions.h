/*
 *  Explosions.h
 *  GLGravity
 *
 *  Created by Paul Senzee on 12/30/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _EXPLOSIONS_H
#define _EXPLOSIONS_H

#include "core/xna.h"
#include <vector>

class LocalGameServices;
class OverheadCamera;
class Renderable;
class XmlReadContext;
class Explosion;

class Explosions
{
public:

    Explosions(LocalGameServices *services, XmlReadContext *context, int duration);
    ~Explosions();
    
    void Explode(const Vector3 &at);
    
    void Update(const GameTime &time);
    void Render(const OverheadCamera &camera);
    
    inline int GetDuration() const { return duration; }

private:

    int                        duration;    
    LocalGameServices         *services;
    XmlReadContext            *context;
    std::vector<Explosion *>   explosions;
    
    void ClearCompleted();
    void Clear();         
};

#endif  // _EXPLOSIONS_H