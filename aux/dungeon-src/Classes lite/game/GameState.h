/*
 *  GameState.h
 *  GLGravity
 *
 *  Created by Paul Senzee on 12/29/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _GAMESTATE_H
#define _GAMESTATE_H

#include "core/xna.h"
#include "core/global.h"

#include "time/GameClock.h"

#include "game/Actors.h"
#include "game/saves/HighScores.h"
#include "game/saves/SaveGame.h"

#include "ui/Menu.h"
#include "ui/UIController.h"

#include "render/GameEffects.h"
#include "events/EventTimer.h"

class GameInput;
class OverheadCamera;
class LocalGameServices;
class DeviceTexture;
class Level;
class LevelSequence;
class Player;
class BallCharacter;
class GameUI;
class XmlReadContext;
class ITileInstance;
class GameTime;
class IAnimator;
class Explosions;
class IChallenge;
class QuadRenderer;

class Audio;

class GameState
{
public:
    
    enum { POINTS_PER_SECOND = 25, WARNING_TIME = 15000 };
    
    enum State { INTRO, TITLE, GAME, PAUSE, WAITING };
    
    enum GameMode { INDIGO_OCHO, MAZES_ONLY };
#ifdef LITE_VERSION        
    enum SpecialLevel { INITIAL_TUTORIAL_LEVEL = 2, BUMP_TUTORIAL_LEVEL = 2, MUTUAL_DESTRUCTION_TUTORIAL_LEVEL = 3, LAST_LEVEL = 6 };
#else
    enum SpecialLevel { INITIAL_TUTORIAL_LEVEL = 2, BUMP_TUTORIAL_LEVEL = 6, MUTUAL_DESTRUCTION_TUTORIAL_LEVEL = 8, LAST_LEVEL = 35 };    
#endif
    
    typedef bool (*predicate_t)(void *context);
    
    EventTimer         eventTimer;
    
    LocalGameServices *services;
    DeviceTexture     *background;
    DeviceTexture     *title;
    OverheadCamera    *camera;
    GameInput         *input;
    QuadRenderer      *quads;
    
    GameMode           mode;
    
    predicate_t        exitReachable;
    
    Audio              *audio;
    
    UIController       uicontroller;
    
    
    State              state;
        
    int                frames;
        
    LevelSequence     *levels;
    Actors             actors;
        
    GameUI            *ui;
        
    unsigned           score,
                       levelScore;
    bool               score2x,
                       score3x,
                       reachedExit,
                       timeExpired,
                       trappedTutorial;
    int                timeRemaining;

    GameEffects        effects;
    XmlReadContext    *renderables;
    Explosions        *explosions;
    
    IChallenge        *challenge;
    
    HighScores         scores;
    HighScoreContext   scoreContext;
    SaveGame           save;
        
    GameState(const core::Size &framesz);
    
    void           SetExitReachablePredicate(predicate_t f) { exitReachable = f; }
    bool           IsExitReachable() { return exitReachable ? exitReachable(this) : true; }    
    
    Level         *GetLevel();
    Level         *GetPreviousLevel();
    BallCharacter *GetPlayer();
    
    ITileInstance *GetPlayerTileAt();
    void           SetPlayerTile(int index);
    void           ClearPickup();
    
    float          GetBumpMagnitude();
    float          GetBallVelocityScale();    
    
    void           Explode(const Vector3 &at, float blastradius);
    
    int            GetTileIdAt(const Vector2 &p);
    void           Start(int level = -1);
    void           SavedStart();
    void           Restart();
    void           SendPointsMessage(unsigned score, int delay);
    void           StartNewLife();
    void           RunEvent(const char *event);
    
    bool           Update(const GameTime &time);
    void           Draw(const GameTime &time);
    
    unsigned       GetLevelScore() const;
    
    void           PlayUISound();
    
    void           ShowMainMenu();
    void           ShowTutorial();
    void           ShowMazeTutorial();    
    void           ShowTrappedTutorial();    
    void           EndGame();
    
    void           GameOver();
    
    void           Save();
    void           NewActiveGame();
    void           NewMazeOnlyGame();
    void           SaveActiveGame(int index);
    void           LoadActiveGame(int index);
    
    bool           IsPaused() const;
    void           SetPaused(bool v);
    
    bool           SpaceOpen(const core::Point &p, int ignore_ball = -1);    
    void           UncollideBall(int index);    
    void           UncollideBalls();
    
    // events
    void           Event_OnPlayerKilled();
    void           Event_OnExitReached();
    void           Event_OnTimeExpired();
    void           Event_OnTrapped();    
    void           Event_OnSwirlEnd(IAnimator *animator);
    void           Event_OnSwirl();
    void           Event_OnPickup2x();
    void           Event_OnPickup3x();
    
    void           _DebugRenderBoundsQuad(const AABox &box);
    
private:
    
    bool paused;
    
    bool           anyRendered;
    
    void           Initialize(const core::Size &framesz);
    void           DrawHud();
    void           Narration();
    void           RenderEffects(const GameTime &time);    
};

#endif // _GAMESTATE_H