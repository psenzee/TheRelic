#ifndef _GAMESTATE_H
#define _GAMESTATE_H

#include "core/core.h"
#include "core/global.h"

#include "time/GameClock.h"
#include "GameDimensions.h"

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
class UiCore;
class XmlReadContext;
class ITile;
class GameTime;
class IAnimator;
class QuadRenderer;
class RenderContext;
class DynamicMap;
class UiInstance;
class Game;

struct lua_State;

class GameState : public IEventListener
{
public:
    
    static GameState       *GetInstance();
    static bool             HasInstance();
    static GameState       *CreateInstance(const GameDimensions &dimensions);
    static void             DestroyInstance();

    bool                    Notify(const char *event, const char *info);
    
    UiCore                 *GetUiCore();
    void                    LoadRenderables();
    void                    StartAudio();

    bool                    Update(const GameTime &time);
    void                    Draw(const GameTime &time);

    void                    StartGameState();
       
    inline Game            *GetGame()                             { return mGame; }
    inline int              GetFrames() const                     { return mFrames; }

    inline void             SetOverlayColor(const Vector4 &color) { mOverlayColor = color; }
    inline const Vector4   &GetOverlayColor() const               { return mOverlayColor; }

    inline const GameDimensions &GetDimensions() const            { return mDimensions; }
    
    inline GameEffects     &GetEffects()                          { return mEffects; }
    inline GameUI          &GetGameUi()                           { return *mGameUi; }
    inline GameInput       &GetInput()                            { return *mInput; }
    inline QuadRenderer    &GetQuadRenderer()                     { return *mQuads; }

    inline EventDispatcher *GetEventDispatcher()                  { return &mEventDispatcher; }
    inline EventDispatcher *GetUiEventDispatcher()                { return &mUiEventDispatcher; }
    inline EventTimer      *GetEventTimer()                       { return &mEventTimer; }

    inline LuaThread       *GetGameThread()                       { return mGameThread; }
    
    inline XmlReadContext  *GetRenderables()                      { return mRenderables; }
    inline OverheadCamera  *GetCamera()                           { return mCamera; }

	static void             PrintLoadedTextures();
	
	float                   GetFpsAverage() const;
	void                    ResetFpsAverage();
	
	void                    UnloadAll();
	
private:
    
    GameState(const GameDimensions &dimensions);
    ~GameState();

    OverheadCamera    *mCamera;
    XmlReadContext    *mRenderables;

    EventTimer         mEventTimer;
    EventDispatcher    mEventDispatcher;
    EventDispatcher    mUiEventDispatcher;

    Multiplayer        mMultiplayer;

    LuaThread         *mGameThread;
    UiInstance        *mUiInstance;    

    UiCore            *mUiCore;

    GameEffects        mEffects;
    GameUI            *mGameUi;

    DeviceTexture     *mBackground;
    GameInput         *mInput;
    QuadRenderer      *mQuads;
    Game              *mGame;

    Vector4            mOverlayColor;

    int                mFrames;
	
	unsigned           mFpsFrames;
	unsigned           mFpsTimeMs;
	double             mFpsRunningAverage;
    GameDimensions     mDimensions;
    
    static GameState  *mInstance;
    
    void               Initialize(const GameDimensions &dimensions);
    void               RenderEffects(const GameTime &time);
	void               UpdateFpsAverage(unsigned framesToAdd, unsigned elapsed);
};

void            SetGlobalRenderContext(const RenderContext &rc);
RenderContext  &GetGlobalRenderContext();
Game           *GetGlobalGame();
GameInput      *GetGlobalInput();
OverheadCamera *GetGlobalCamera();

extern "C" void SetScreenTouchBegan(int index, int x, int y);
extern "C" void SetScreenTouchMoved(int index, int x, int y);
extern "C" void SetScreenTouchStationary(int index, int x, int y);
extern "C" void SetScreenTouchEnded(int index, int x, int y);

#endif // _GAMESTATE_H
