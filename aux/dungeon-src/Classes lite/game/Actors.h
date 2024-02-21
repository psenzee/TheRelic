/*
 *  Actors.h
 *  GLGravity
 *
 *  Created by Paul Senzee on 12/30/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _ACTORS_H
#define _ACTORS_H

#include "BallCharacter.h"

#include "core/xna.h"
#include "core/bitmap.h"
#include "time/GameClock.h"
        
#include <vector>

class Actors
{
public:

    std::vector<BallCharacter *> list;
        
    typedef bool (*notify_collision_fn_t)(BallCharacter *a, BallCharacter *b, const Vector3 &velocity, void *context);

    Actors();
    
    void Add(BallCharacter *p);
    void SetForce(const Vector2 &force);
    Vector2 GetRepelForce(BallCharacter *ball, const char *type);
    void Clear();
    void Update(const GameTime &time);
    void Draw(const GameTime &time);
    void DrawShadow(); // assumes that Draw was called earlier in the frame
    
    int  GetIndex(const BallCharacter *p) const;
    
    void SetPlayerIndex(int i);
    void SetPlayer(const BallCharacter *p) { SetPlayerIndex(GetIndex(p)); }
    
    Vector3 GetLookAt();
    
    int            GetPlayerIndex() const  { return player; }
    BallCharacter *GetPlayer()             { return list[player]; }
    
    void AddCollisionNotifiable(notify_collision_fn_t notifiable, void *context);
    void RemoveCollisionNotifiable(notify_collision_fn_t notifiable, void *context);
    bool NotifyOfCollision(BallCharacter *a, BallCharacter *b, const Vector3 &velocity);

private:
    
    int   player, previousplayer;
    Fader transition;
    
    struct Notifiable
    {
        notify_collision_fn_t  function;
        void                  *context;
        
        inline bool operator==(const Notifiable &u) const { return function == u.function && context == u.context; }
    };
    
    std::vector<Notifiable> notifiables;
};

#endif // _ACTORS_H