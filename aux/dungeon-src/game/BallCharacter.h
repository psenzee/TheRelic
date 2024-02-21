/*
 *  BallCharacter.h
 *  GLGravity
 *
 *  Created by Paul Senzee on 12/30/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _BALLCHARACTER_H
#define _BALLCHARACTER_H

#include "core/xna.h"
#include "core/random.h"

#include <string>

class GameState;
class RollingBall;
class Map;
class XmlElement;
class GameTime;
class Actors;

class BallCharacter
{
public:
            
    enum { BALL_SIZE = 32 };
    
    static const float BUMP_MAGNITUDE;// = 0.25f; /*2.5f*/
    static const float VELOCITY_SCALE;// = 20.0f; /*200.0f*/
        
    BallCharacter(GameState *state, XmlElement *level, const Vector2 &position);
    ~BallCharacter();
    
    void        SetForceFactor(float v);
    void        SetForce(const Vector2 &force);
    void        SetMap(Map *map);
    Vector3     GetPosition() const;
    void        SetPosition(const Vector2 &position) { Position = position; }

    const char *GetType() const;
    
    void        LoadContent(const char *type);
    void        Reset(const Vector2 &position);

    void        Draw(const GameTime &time);
    void        DrawShadow(); // this assumes that Draw was called earlier in the frame
    void        DrawHalo(); // this assumes that Draw was called earlier in the frame    
    void        Update(const GameTime &time, Actors &actors);
    
    bool        IsDestroyed() const  { return destroyed; }
    void        SetDestroyed(bool v) { destroyed = v; }
    
    RollingBall *GetRollingBall() { return ball; }
    
private:    
    
    void    Fixup(Actors &actors);
    bool    Collides(const Vector3 &position, Actors &actors, bool *which);    
    bool    Collides(const Vector3 &position, Actors &actors);
    void    ApplyPhysics(const GameTime &time, Actors &actors);
    bool    CollidesAt(const Vector2 &p);
    bool    HandleCollisions(const Vector2 &move, Actors &actors);
    
    GameState    *state;
    RollingBall  *ball;
    Map          *map;
    bool          destroyed;
    core::Random  jitter;
    
    float         forceFactor;
    
    Vector2       Position;
    Vector2       Force;
    Vector2       Velocity;
    
    std::string   type;
};

#endif // _BALLCHARACTER_H