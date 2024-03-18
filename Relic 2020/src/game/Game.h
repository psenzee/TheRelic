#ifndef _GAME_H
#define _GAME_H

/* THIS APPEARS TO BE TOTALLY DEPRECATED IN FAVOR OF GameState!!  MAYBE IT'S FROM I8 */


#include "core/core.h"
#include "core/global.h"

#include "time/GameClock.h"
#include "events/IEventListener.h"
#include "game/Multiplayer.h"

class GameInput;
class OverheadCamera;
class Level;
class LevelManager;
class Character;
class UiCore;
class ITile;
class GameTime;
class IAnimator;
class RenderContext;
class LuaThread;

class GameDescription
{
public:

    GameDescription(int level = 1) : level(level) {}

    int level;
};

class Game : public IEventListener
{
public:
        
    Game(UiCore *uiCore, LuaThread *luaThread, Multiplayer *multiplayer);
    ~Game();

    bool                    Notify(const char *event, const char *info);
    
    UiCore                 *GetUiCore();

    LevelManager           *GetLevels() { return mLevels; }
    Level                  *GetLevel();
    Character              *GetPlayer();
    void                    SetPlayer(Character *p);
    
    Vector3                 GetPlayerTileCenter();
    ITile                  *GetPlayerTileAt();

    inline int              GetFrames() const { return mFrames; }

    bool                    Update(const GameTime &time);
    void                    Draw(const GameTime &time);
       
    bool                    IsPaused() const;
    void                    SetPaused(bool v);
    bool                    IsRenderPaused() const;
    void                    SetRenderPaused(bool v);
    bool                    IsCharacterPaused() const { return mCharacterPaused; }
    void                    SetCharacterPaused(bool v) { mCharacterPaused = v; }

    inline LuaThread       *GetGameThread()        { return mGameThread; }

    void                    StartGame(const GameDescription &description);
    void                    EndGame();

    Multiplayer            &GetMultiplayer()       { return *mMultiplayer; }

    void                    Visit(const core::Rectangle &range);

	void                    Autosave();

private:

    LevelManager           *mLevels;
    Character              *mPlayer;

    bool                    mPaused;
    bool                    mRenderPaused;
    bool                    mCharacterPaused;
    bool                    mGameInProgress;

    LuaThread              *mGameThread;

    UiCore                 *mUiCore;
        
    int                     mFrames;
    
    Multiplayer            *mMultiplayer;
    
    void                    Initialize();
};

#endif // _GAME_H
