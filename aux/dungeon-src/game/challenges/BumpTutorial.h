/*
 *  BumpTutorial.h
 *  GLGravity
 *
 *  Created by Paul Senzee on 2/3/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */


#ifndef _BALLTUTORIAL_H
#define _BALLTUTORIAL_H

#include "game/IChallenge.h"

class BitMap;
class GameState;
class BallCharacter;
class Explosion;
class GameTime;

class BumpTutorial : public IChallenge
{
public:
        
    BumpTutorial(GameState *state, bool tutorial);
    ~BumpTutorial();
        
    void        Update(const GameTime &time);
    const char *GetMessage() const;
    void        ShowTutorial();
    
    void        ShowBumpedSuperballTutorial();
    void        ShowBumpedIndigoTutorial();    
        
    bool        NotifyCollision(BallCharacter *a, BallCharacter *b, const Vector3 &v);
                
private:

    enum TutorialState { TUTORIAL_START, TUTORIAL_BUMP_SUPERBALL, TUTORIAL_BUMP_INDIGO, TUTORIAL_DONE };
    
    GameState                *state;
    int                       bumpTime;
    TutorialState             tutorial;
    bool                      unlocked;
        
    BallCharacter            *GetCharacter(int index);
        
    static bool NotifyCollision(BallCharacter *a, BallCharacter *b, const Vector3 &v, void *context);
};

#endif // _BALLTUTORIAL_H