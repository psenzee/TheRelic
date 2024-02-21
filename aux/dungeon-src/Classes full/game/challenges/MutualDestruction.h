/*
 *  MutualDestruction.h
 *  GLGravity
 *
 *  Created by Paul Senzee on 1/23/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _MUTUALDESTRUCTION_H
#define _MUTUALDESTRUCTION_H

#include "game/IChallenge.h"

class BitMap;
class GameState;
class BallCharacter;
class Explosion;
class GameTime;

class MutualDestruction : public IChallenge
{
public:
        
    MutualDestruction(GameState *state, int pairs, bool tutorial);
    ~MutualDestruction();
        
    void        Update(const GameTime &time);
    const char *GetMessage() const;
    void        ShowTutorial();
    void        ShowBumpedTutorial();
    void        ShowDetonationTutorial();    
    
    bool NotifyCollision(BallCharacter *a, BallCharacter *b, const Vector3 &v);
    
    bool NotifyCollisionBump(BallCharacter *a, BallCharacter *b, const Vector3 &v);
        
    static bool FLIP_MINUS_DIRECTION;
    
    bool IsExitPossible();
    
private:
        
    enum { POSITIVE = 0, NEGATIVE = 1 };
    
    enum TutorialState { TUTORIAL_START, TUTORIAL_BUMP_BALL, TUTORIAL_DETONATION, TUTORIAL_DONE };
        
    GameState                *state;
    int                       begin, pairs, destroyed;
    int                       bumpTime;
    TutorialState             tutorial;
        
    BallCharacter            *GetCharacter(int index);
    
    static bool NotifyCollision(BallCharacter *a, BallCharacter *b, const Vector3 &v, void *context);
    static bool NotifyCollisionBump(BallCharacter *a, BallCharacter *b, const Vector3 &v, void *context);    
};

#endif // _MUTUALDESTRUCTION_H