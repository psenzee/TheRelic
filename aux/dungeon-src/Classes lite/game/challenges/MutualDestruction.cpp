/*
 *  MutualDestruction.cpp
 *  GLGravity
 *
 *  Created by Paul Senzee on 1/23/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#include "MutualDestruction.h"

#include "core/xna.h"
#include "core/bitmap.h"
#include "core/random.h"

#include "game/GameState.h"
#include "game/BallCharacter.h"
#include "game/Actors.h"
#include "game/Level.h"

#include "objects/Explosion.h"
#include "objects/RollingBall.h"

#include "ui/GameUI.h"
#include "time/Timer.h"

#include "map/Map.h"
#include "map/MarkedMap.h"
#include "map/MazeSolver.h"

#include "audio/Audio.h"

bool MutualDestruction::FLIP_MINUS_DIRECTION = false;

static BallCharacter *CreateBall(const char *type, GameState *state, const Vector3 &pos, core::Random &r, float direction)
{
    Level *level = state->GetLevel();
    Map   *map   = level->GetMap();
    BallCharacter *ball = new BallCharacter(state, level->GetOptions()->configuration, v2(pos));
    ball->SetForceFactor((r.real() * 0.5f + 0.6f) * direction);
    ball->LoadContent(type);
    ball->SetMap(map);
    return ball;
}

bool IsExitPossible(void *context)
{
    GameState         *state = (GameState *)context;
    MutualDestruction *md    = (MutualDestruction *)(state->challenge);
    return md ? md->IsExitPossible() : 0;
}

MutualDestruction::MutualDestruction(GameState *state, int pairs, bool tutorial) : state(state), begin(0), pairs(pairs), destroyed(0), bumpTime(0), tutorial(tutorial ? TUTORIAL_START : TUTORIAL_DONE)
{
    begin = (int)state->actors.list.size();
    state->actors.SetPlayerIndex(0);
    state->actors.GetPlayer()->GetRollingBall()->SetHaloState(RollingBall::MUTUAL_DESTRUCTION);
    state->actors.SetPlayerIndex(0);    
    // code to add balls to state->actors
    core::Random r(1342);
    Map        *map  = state->GetLevel()->GetMap();
    core::Point pp   = map->FindEvent("Pickup2x"),
                np   = map->FindEvent("Pickup3x");
    MarkedMap   marks(map->MapModel->GetTileMap());
    for (int i = 0; i < pairs; i++)
    {
        core::Point pp0(0, 0), np0(0, 0);
        if (!marks.FindClosestUnmarkedFloor(pp, pp0)) pp0 = pp;
        marks.Mark(pp0);
        if (!marks.FindClosestUnmarkedFloor(np, np0)) np0 = np;
        marks.Mark(np0);
        BallCharacter *positive = CreateBall("PositiveBall", state, map->GetPointInWorld(pp0) + Vector3(0.0f, 16.0f, 0.0f), r,                                1.0f),
                      *negative = CreateBall("NegativeBall", state, map->GetPointInWorld(np0) + Vector3(0.0f, 16.0f, 0.0f), r, FLIP_MINUS_DIRECTION ? -1.0f : 1.0f);
        state->actors.Add(positive);
        state->actors.Add(negative);
    }
//  state->UncollideBalls();
    state->actors.AddCollisionNotifiable(MutualDestruction::NotifyCollision, this);
    state->actors.AddCollisionNotifiable(MutualDestruction::NotifyCollisionBump, this);
    state->SetExitReachablePredicate(::IsExitPossible);
    state->GetLevel()->LockLevelExit();
}

MutualDestruction::~MutualDestruction()
{
    state->SetExitReachablePredicate(0);    
    state->actors.RemoveCollisionNotifiable(MutualDestruction::NotifyCollision, this);
    state->actors.RemoveCollisionNotifiable(MutualDestruction::NotifyCollisionBump, this);    
}

void MutualDestruction::Update(const GameTime &time)
{
    if (tutorial == TUTORIAL_START)
    {
        ShowTutorial();
        tutorial = TUTORIAL_BUMP_BALL;
    }
}

bool MutualDestruction::IsExitPossible()
{
    Map *map = state->GetLevel()->GetMap();
    std::vector<BallCharacter *> &v = state->actors.list;
    bool pos = false, neg = false;
    core::Point player = map->GetTileLocationAt(v2(state->actors.GetPlayer()->GetPosition()));
    // if there are balls, check reachability to both a plus and a minus, else check reachability to the end
    int count = 0;
    for (std::vector<BallCharacter *>::iterator i = v.begin(), e = v.end(); i != e; ++i)
    {
        core::Point iat = map->GetTileLocationAt(v2((*i)->GetPosition()));
        if (*i && !(*i)->IsDestroyed() && strcmp((*i)->GetType(), "PositiveBall") == 0)
        {
            // check reachability to any positive ball
            pos |= Reachable(map, player, iat);
            count++;
        }
        if (*i && !(*i)->IsDestroyed() && strcmp((*i)->GetType(), "NegativeBall") == 0)
        {
            // check reachability to any negative ball            
            neg |= Reachable(map, player, iat);
            count++;
        }
        if (pos && neg)
            return true;
    }
    if (!count)
        return Reachable(map, player, map->GetEndTileLocation());
    return false;
}

void SetPlayer2AsTemporaryActivePlayer(void *);

void MutualDestruction::ShowTutorial()
{
    if (state->state != GameState::GAME || state->uicontroller.IsAnyVisible())
        return;        

    static const char *text[] =
    {
        "\nClear each level of\n"
        "\\#ff0000plus\\#ffffff and \\#404040minus\\#ffffff balls\n"
        "to proceed to the next.",
        
        "\nAssure their mutual\n"
        "destruction. \\#ffff00Bump\\#ffffff each\n"
        "dangerous ball and slam\n"
        "it into its opposite.", 
        
        "\nDon't you worry about\n"
        "\\#6600ffIndigo\\#ffffff. He's \\#ffff00radiation\\#ffffff\n"
        "hardened.",
        
        "\nThe \\#00ff00walls\\#ffffff will come\n"
        "crumbling down.  Use\n"
        "that to your advantage!\n",
        
        "\n\nBut..\n"
        "\\#ff0000DON'T GET TRAPPED!",
        
        0
    };
    state->SetPaused(true);    
    state->eventTimer.Add(SetPlayer2AsTemporaryActivePlayer, state, 2000);    
    state->uicontroller.ShowText("\\#6600ff`Fusion!", text);
}

void MutualDestruction::ShowBumpedTutorial()
{
    if (state->state != GameState::GAME)
        return;    
    
    static const char *text[] =
    {
        "\nYou possessed an\n"
        "atomic ball. Now smash\n"
        "into its opposite.",
        
        0
    };    
    state->SetPaused(true);    
    state->uicontroller.ShowText("\\#6600ff`Perfect!", text);
}

void MutualDestruction::ShowDetonationTutorial()
{
    if (state->state != GameState::GAME)
        return;    
    
    static const char *text[] =
    {
        "\n\nYou've just detonated a\n"
        "small atomic blast.",
        
        0
    };    
    state->SetPaused(true);    
    state->uicontroller.ShowText("\\#6600ff`Great!", text);
}

const char *MutualDestruction::GetMessage() const
{
    static char buffer[1024];
    sprintf(buffer, "%u/%u \\#ff0000+\\#000000-", destroyed, pairs);
    return buffer;
}

bool MutualDestruction::NotifyCollision(BallCharacter *a, BallCharacter *b, const Vector3 &v)
{
    if (a->IsDestroyed() || b->IsDestroyed() || destroyed >= pairs)
        return false;
    const float BLAST_RADIUS = 96.0f;    
    const char *at = a->GetType(), *bt = b->GetType();
    if ((strcmp(at, "PositiveBall") != 0 || strcmp(bt, "NegativeBall") != 0) &&
        (strcmp(at, "NegativeBall") != 0 || strcmp(bt, "PositiveBall") != 0))
        return false;
    if (a != state->actors.GetPlayer() && b != state->actors.GetPlayer())
        return false; // require one of the balls to be a player
    a->SetDestroyed(true);
    b->SetDestroyed(true);
    state->timeRemaining += 30000; // add 30sec back
    if (a == state->actors.GetPlayer() || b == state->actors.GetPlayer())
    {
        state->GetLevel()->SetExitState(Level::EXIT_VALID);
        state->actors.SetPlayerIndex(0);
    }
    state->Explode((a->GetPosition() + b->GetPosition()) * 0.5f, BLAST_RADIUS);
    destroyed++;
    if (!state->actors.GetPlayer()->IsDestroyed())
    {
        if (destroyed == pairs)
        {
            state->GetLevel()->UnlockLevelExit();
            state->actors.GetPlayer()->GetRollingBall()->SetHaloState(RollingBall::LEAVE);
            state->ui->SendBallMessage("\\#ffff00Unlocked!");
            state->audio->Play("unlock");
        }
        else if (destroyed < pairs)
        {
            char message[1024];
            sprintf(message, "\\#00ff00%u/%u pairs", destroyed, pairs);
          //state->ui->SendBallMessage(message);
        }
    }
    if (tutorial == TUTORIAL_DETONATION)
    {
        ShowDetonationTutorial();
        tutorial = TUTORIAL_DONE;
    }    
    return true;
}

bool MutualDestruction::NotifyCollisionBump(BallCharacter *a, BallCharacter *b, const Vector3 &v)
{
    if (a->IsDestroyed() || b->IsDestroyed() || (a != state->GetPlayer() && b != state->GetPlayer()))
        return false;
    if (v.length() < state->GetBumpMagnitude()) // how hard to bump?
        return false;
    int time = GetCurrentTimeMs();
    if (time - bumpTime < 500) // a delay between bumps
        return false;
    bumpTime = time;
    state->audio->Play("bump", 0.15f);
    state->actors.SetPlayer((a == state->actors.GetPlayer()) ? b : a);
    state->GetLevel()->SetExitState(state->actors.GetPlayerIndex() == 0 ? Level::EXIT_VALID : Level::EXIT_INVALID);
    // tutorial stuff
    if (tutorial == TUTORIAL_BUMP_BALL && state->actors.GetPlayerIndex() != 0)
    {
        ShowBumpedTutorial();
        tutorial = TUTORIAL_DETONATION;
    }
    return true;
}

bool MutualDestruction::NotifyCollision(BallCharacter *a, BallCharacter *b, const Vector3 &v, void *context)
{
    if (!context) return false;
    MutualDestruction *mad = (MutualDestruction *)context;
    return mad->NotifyCollision(a, b, v);
}

bool MutualDestruction::NotifyCollisionBump(BallCharacter *a, BallCharacter *b, const Vector3 &v, void *context)
{
    if (!context) return false;
    MutualDestruction *mad = (MutualDestruction *)context;
    return mad->NotifyCollisionBump(a, b, v);
}

BallCharacter *MutualDestruction::GetCharacter(int index)
{
    return state->actors.list[index];
}