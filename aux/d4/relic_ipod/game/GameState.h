#ifndef _GAMESTATE_H
#define _GAMESTATE_H

#include "core/core.h"
#include "core/global.h"

#include "time/GameClock.h"

#include "ui/Menu.h"
#include "ui/UIController.h"

#include "render/GameEffects.h"
#include "events/EventTimer.h"
#include "events/EventDispatcher.h"
#include "events/IEventListener.h"

#include "game/Multiplayer.h"

#include "luautil/LuaInterpreter.h"

#include "render/LocalGameServices.h" // $HACK$

class GameInput;
class OverheadCamera;
class LocalGameServices;
class DeviceTexture;
class Level;
class LevelManager;
class Player;
class Character;
class GameUI;
class XmlReadContext;
class ITileInstance;
class GameTime;
class IAnimator;
class QuadRenderer;
class RenderContext;
class DynamicMap;
class UiInstance;

class Audio;

struct lua_State;

class GameState : public IEventListener
{
public:
    
    EventTimer         eventTimer;
    EventDispatcher    eventDispatcher;
    
    LocalGameServices *services;
    DeviceTexture     *background;
    DeviceTexture     *title;
    OverheadCamera    *camera;
    GameInput         *input;
    QuadRenderer      *quads;
        
    Audio             *audio;
    
    UIController      *uicontroller;
           
    int                frames;
        
    LevelManager      *levels;
//	DynamicMap        *dynamicMap;
	Character         *player;
    
    Multiplayer        multiplayer;

    GameUI            *ui;

    GameEffects        effects;
    XmlReadContext    *renderables;
        
    GameState(const core::Size &framesz);
    ~GameState();

    bool           Notify(const char *event);
    
    Level         *GetLevel();
    Character     *GetPlayer();
    void           SetPlayer(Character *p);
    
    Vector3        GetPlayerTileCenter();
    ITileInstance *GetPlayerTileAt();
    void           SetPlayerTile(int index);
    
    int            GetTileIdAt(const Vector2 &p);

    void           Start();
    void           Restart();
    void           StartNewLife();
    
    bool           Update(const GameTime &time);
    void           Draw(const GameTime &time);
    
    void           PlayUISound();
       
    bool           IsPaused() const;
    void           SetPaused(bool v);

    void                   ShowMainMenu();

    inline LuaInterpreter *GetInterpreter() { return mInterpreter; }
    inline LuaThread      *GetGameThread()  { return mGameThread; }
    
    inline XmlReadContext *GetRenderables() { return renderables; }
    inline OverheadCamera *GetCamera()      { return camera; }

private:
    
    bool            paused;    
    bool            anyRendered;

	LuaInterpreter *mInterpreter;
    LuaThread      *mGameThread;
    UiInstance     *mUiInstance;
    
    void            Initialize(const core::Size &framesz);
    void            DrawHud(RenderContext &context);
    void            RenderEffects(const GameTime &time);
};

void           SetGlobalRenderContext(const RenderContext &rc);
RenderContext &GetGlobalRenderContext();

#endif // _GAMESTATE_H