/*
 *  RollingBall.h
 *  GLGravity
 *
 *  Created by Paul Senzee on 12/30/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _ROLLINGBALL_H
#define _ROLLINGBALL_H

#include "core/xna.h"
#include "core/quaternion.h"
#include "time/GameClock.h"

class XmlElement;
class LocalGameServices;
class OverheadCamera;
class Renderable;
class XmlReadContext;

class RollingBall
{
public:

    static Vector3 BALL_OFFSET;    
    
    enum HaloState { NORMAL, MUTUAL_DESTRUCTION, BUMP, LEAVE };
    
    Quaternion rotation;
    float      bounciness,
               conservation;
    Fader      halofader;
    HaloState  haloState;
    
    RollingBall(XmlElement *options, const char *name);
    
    void SetHaloState(HaloState state) { haloState = state; }
    
    void Load(LocalGameServices *services, XmlReadContext *context, const char *type);    
    void Render(const OverheadCamera &camera, const Vector3 &position, const Vector3 &offset);
    
    // this assumes that Render() was called earlier this frame
    void RenderShadow(const OverheadCamera &camera);
    
    // this assumes that Render() was called earlier this frame
    void RenderHalo(const OverheadCamera &camera);    
    
private:
        
    LocalGameServices *services;
    Renderable        *ballStatic,
                      *ballDynamic,
                      *ballShadow,
                      *ballHalo,
                      *ballHaloLeave,
                      *ballHaloMAD,
                      *ballHaloBump;
    Vector3            last;
    Matrix             translate;
    
    static float       RADIUS;
    static Vector3     THIS_UP;
};

#endif  // _ROLLINGBALL_H