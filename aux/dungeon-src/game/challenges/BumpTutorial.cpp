/*
 *  BumpTutorial.cpp
 *  GLGravity
 *
 *  Created by Paul Senzee on 2/3/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#include "BumpTutorial.h"

#include "core/xna.h"
#include "core/bitmap.h"
#include "core/random.h"

#include "game/GameState.h"
#include "game/BallCharacter.h"
#include "game/Actors.h"
#include "game/Level.h"

#include "ui/GameUI.h"

#include "time/Timer.h"

#include "map/Map.h"
#include "map/MarkedMap.h"

#include "objects/Explosion.h"
#include "objects/RollingBall.h"
#include "audio/Audio.h"

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

BumpTutorial::BumpTutorial(GameState *state, bool tutorial) : state(state), bumpTime(0), tutorial(tutorial ? TUTORIAL_START : TUTORIAL_DONE), unlocked(false)
{
    state->actors.SetPlayerIndex(0);    
    state->actors.GetPlayer()->GetRollingBall()->SetHaloState(RollingBall::BUMP);    
    state->actors.SetPlayerIndex(0);    
    // code to add balls to state->actors
    core::Random r(1342);
    Map        *map  = state->GetLevel()->GetMap();
    core::Point pp   = map->FindEvent("ExitLevel");
    Vector3     ppos = map->GetPointInWorld(pp);
    BallCharacter *superball = CreateBall("SuperBall", state, ppos + Vector3(0.0f, 16.0f, 0.0f), r, 1.0f);
    state->actors.Add(superball);
    state->actors.AddCollisionNotifiable(BumpTutorial::NotifyCollision, this);
    state->GetLevel()->LockLevelExit();
}

BumpTutorial::~BumpTutorial()
{
    state->actors.RemoveCollisionNotifiable(BumpTutorial::NotifyCollision, this);
}

void BumpTutorial::Update(const GameTime &time)
{
    if (tutorial == TUTORIAL_START)
    {
        ShowTutorial();
        tutorial = TUTORIAL_BUMP_SUPERBALL;
    }
}


void SetIndigoAsActivePlayer(void *context)
{
    GameState *state = (GameState *)context;
    state->actors.SetPlayerIndex(0);
}

void SetPlayer1AsTemporaryActivePlayer(void *context)
{
    GameState *state = (GameState *)context;
    state->actors.SetPlayerIndex(1);
    state->eventTimer.Add(SetIndigoAsActivePlayer, state, 2000); // and in two seconds, set indigo back    
}

void SetPlayer2AsTemporaryActivePlayer(void *context)
{
    GameState *state = (GameState *)context;
    state->actors.SetPlayerIndex(2);
    state->eventTimer.Add(SetPlayer1AsTemporaryActivePlayer, state, 2000); // and in two seconds, set indigo back    
}

void BumpTutorial::ShowTutorial()
{
    if (state->state != GameState::GAME || state->uicontroller.IsAnyVisible())
        return;    
    
    static const char *text[] =
    {
        "\nOther balls ain't got\n"
        "no \\#ffff00soul\\#ffffff. \\#6600ffIndigo\\#ffffff has to\n"
        "possess them to make\n"
        "them do what you want.\n",
        
        "\\#ffff00Bump\\#ffffff into them with\n"
        "some force to control\n"
        "them, but remember\n"
        "that only \\#6600ffIndigo\\#ffffff can\n"
        "leave the level!",
        
        "\n\\#ffff00Bump\\#ffffff the \\#ff0000s\\#ffff00u\\#00ff00p\\#0000ffe\\#ffff00r\\#00ff00b\\#ff0000a\\#ffff00l\\#0000ffl\\#ffffff at\n"
        "least once to unlock\n"
        "the level.\n",
        
        0
    };
    state->SetPaused(true);
    state->eventTimer.Add(SetPlayer1AsTemporaryActivePlayer, state, 2000); // and in two seconds, set indigo back
    state->uicontroller.ShowText("\\#6600ffThe Bump!", text);
}

void BumpTutorial::ShowBumpedSuperballTutorial()
{
    if (state->state != GameState::GAME)
        return;
    
    static const char *text[] =
    {
        "\n\nYou \\#ffff00bump\\#ffffffed the\n"
        "\\#ff0000s\\#ffff00u\\#00ff00p\\#0000ffe\\#ffff00r\\#00ff00b\\#ff0000a\\#ffff00l\\#0000ffl\\#ffffff!",
        
        "\n\nNow \\#ffff00bump\\#ffffff \\#6600ffIndigo\\#ffffff back to\n"
        "to leave the level.\n",
        
        0
    };
    state->SetPaused(true);    
    state->uicontroller.ShowText("\\#6600ffCongratulations!", text);
}

void BumpTutorial::ShowBumpedIndigoTutorial()
{
    if (state->state != GameState::GAME)
        return;    
    
    static const char *text[] =
    {
        "\n\nThe level is unlocked, you\n"
        "can leave now!",
        
        0
    };
    state->SetPaused(true);    
    state->uicontroller.ShowText("\\#6600ffGood Job!", text);
}

const char *BumpTutorial::GetMessage() const
{
    return 0;
}

bool BumpTutorial::NotifyCollision(BallCharacter *a, BallCharacter *b, const Vector3 &v)
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
    if (!unlocked)
    {
        state->GetLevel()->UnlockLevelExit();
    state->actors.GetPlayer()->GetRollingBall()->SetHaloState(RollingBall::LEAVE);        
        state->ui->SendBallMessage("\\#ffff00Unlocked!");
        state->audio->Play("unlock");
        unlocked = true;
    }
    state->actors.SetPlayer((a == state->actors.GetPlayer()) ? b : a);
    bool isIndigo = (state->actors.GetPlayerIndex() == 0);
    state->GetLevel()->SetExitState(isIndigo ? Level::EXIT_VALID : Level::EXIT_INVALID);
    // do tutorial stuff
    if (tutorial == TUTORIAL_BUMP_SUPERBALL && !isIndigo)
    {
        ShowBumpedSuperballTutorial();
        tutorial = TUTORIAL_BUMP_INDIGO;        
    }
    else if (tutorial == TUTORIAL_BUMP_INDIGO && isIndigo)
    {
        ShowBumpedIndigoTutorial();
        tutorial = TUTORIAL_DONE;
    }    
    return true;
}

bool BumpTutorial::NotifyCollision(BallCharacter *a, BallCharacter *b, const Vector3 &v, void *context)
{
    if (!context) return false;
    BumpTutorial *self = (BumpTutorial *)context;
    return self->NotifyCollision(a, b, v);
}

BallCharacter *BumpTutorial::GetCharacter(int index)
{
    return state->actors.list[index];
}