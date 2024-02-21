/*
 *  Player.h
 *  GLGravity
 *
 *  Created by Paul Senzee on 12/30/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _PLAYER_H
#define _PLAYER_H

#include "core/xna.h"

#include "BallCharacter.h"

class GameState;
class RollingBall;
class Map;
class XmlElement;
class GameTime;

class Player : public BallCharacter
{
public:

    bool         IsAlive;
    Vector2      TiltBias;

    Player(GameState *state, XmlElement *level, const Vector2 &position);
    
    void Reset(const Vector2 &position);
        
    void OnKilled();
};

#endif // _PLAYER_H