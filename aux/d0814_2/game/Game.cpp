#include "Game.h"
#include "Character.h"
#include "Sync.h"
#include "level/LevelManager.h"
#include "level/Level.h"
#include "input/GameInput.h"
#include "map/TileMap.h"
#include "map/Map.h"
#include "render/OverheadCamera.h"
#include "ui/GameUI.h"
#include "ui/UiCore.h"
#include "render/DrawList.h"
#include "render/Material.h"
#include "render/GraphicsDevice.h"
#include "render/RenderContext.h"
#include "render/ParticleEffects.h"
#include "render/RenderSet.h"
#include "render/ContentLoader.h"
#include "render/QuadRenderer.h"
#include "time/Timer.h"
#include "luautil/LuaEvents.h"
#include "luautil/LocationEvents.h"
#include "events/EventDispatcher.h"
#include "events/EventTimer.h"

#include "LuaFunctions.h"

#include "game/UiInstance.h"

#include "core/global.h"
#include "core/strs.h"

#include "time/FrameRateTracker.h"
#include "core/Profiler.h"

#include "render/ContentLoader.h"

#include "luautil/LuaThread.h"

#include "serialize/LuaSerialize.h"
#include "render/ParticleFxDrawable.h"

extern "C"
{
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}

FrameRateTracker game_track("track", 40);

extern void SetDefaultLightingType(int type);
extern void PushLightingType();
extern void PopLightingType();

#include "xml/XmlReadContext.h"
#include "gamecore/IGameObject.h"
#include "gamecore/IRenderable.h"
#include "audio/SoundManager.h"

#include <string.h>

extern void             SetGlobalRenderContext(const RenderContext &rc);
extern EventDispatcher *GetGlobalEventDispatcher();
extern EventDispatcher *GetGlobalUiEventDispatcher();
extern EventTimer      *GetGlobalEventTimer();
extern OverheadCamera  *GetGlobalCamera();
extern GameInput       *GetGlobalInput();
extern bool             IsLowEndDevice();

extern XmlReadContext  *ReadXmlConfigText(const char *);

Game::Game(UiCore *uiCore, LuaThread *luaThread, Multiplayer *multiplayer) :
    mFrames(0), mLevels(0), mPlayer(0), 
    mPaused(false), mRenderPaused(false), mCharacterPaused(false), mGameInProgress(false),
    mGameThread(luaThread), mUiCore(uiCore), mMultiplayer(multiplayer)
{
    GraphicsDevice *device = GraphicsDevice::GetInstance();    

    mLevels = new LevelManager(mMultiplayer);
    mLevels->SetLuaThread(mGameThread);
}

Game::~Game()
{
    EndGame();
    if (mLevels) delete mLevels;
    mLevels = 0;
}

// $TODO needs to go away, everything should be done through LevelManager
Level *Game::GetLevel()
{
    return mLevels->GetLevel();
}

// $TODO probably needs to be owned by Level/LevelManager
Character *Game::GetPlayer()
{
    return mPlayer;
}

// $TODO this needs to go in LevelManager
static Vector2 GetPositionVector2(const Vector3 &player, const Vector3 &map)
{
    return Vector2(player.x - map.x, player.y - map.y);
}

// $TODO this needs to go in LevelManager
static core::Point GetPositionPoint(const Vector3 &player, const Vector3 &map)
{
    return core::Point(int(player.x - map.x), int(player.y - map.y));
}

// $TODO this needs to go in LevelManager
Vector3 Game::GetPlayerTileCenter()
{
    Map *map = mLevels->GetMap();
    if (!map)
        return Vector3(0.f, 0.f, 0.f);
    Vector3 mp(map->GetPosition());
    core::Point p = TileMaps::GetTileLocationAtWorld(map->GetMapModel(), GetPositionPoint(GetPlayer()->GetPosition(), map->GetPosition()));
    core::Size tileSz = map->GetMapModel()->GetTileSize();
    return Vector3(p.x * float(tileSz.width)  + mp.x,
                   p.y * float(tileSz.height) + mp.y, 0.f);
}

// $TODO this needs to go in LevelManager
ITile *Game::GetPlayerTileAt()
{
    return mLevels->GetTileAt(Level::BASE_MAP, GetPlayer()->GetPosition());
}

bool Game::IsPaused() const
{
    return mPaused;
}

void Game::SetPaused(bool v)
{
    mPaused = v;
}

bool Game::IsRenderPaused() const
{
    return mRenderPaused;
}

void Game::SetRenderPaused(bool v)
{
    mRenderPaused = v;
}

UiCore *Game::GetUiCore()
{
    return mUiCore;
}

void Game::StartGame(const GameDescription &description)
{
    if (!mGameInProgress)
    {
        mLevels->LoadLevel(description.level); // $TODO one for now

        Map *map = GetLevel()->GetMap();
        core_assert(map);
        if (!map)
            return;

        Vector3 position(map->GetPosition() + map->GetStartPoint());
        Vector3 pos(position.x, position.y, 0.f);    

        if (mPlayer)
        {
            mPlayer->SetDynamicMap(0);
            delete mPlayer;
            mPlayer = 0;
        }

        mGameThread->Execute("StartGame");
        GetGlobalInput()->enabled = true;
        mGameInProgress = true;
        LuaCall(mGameThread->GetLuaState(), "OnStartGameComplete", 0, 0);
    }
}

void Game::EndGame()
{
//    if (mGameInProgress)
    {
        mGameThread->Execute("EndGame");
        SetPaused(true);
        GetGlobalInput()->enabled = false;
        mLevels->UnloadLevel();
        if (mPlayer)
        {
            mPlayer->SetDynamicMap(0);
            delete mPlayer;
            mPlayer = 0;
        }
        mGameInProgress = false;
        LuaCall(mGameThread->GetLuaState(), "OnEndGameComplete", 0, 0);
    }
}

FrameRateTracker  game_leveldrawlogic  ("level draw logic",         0),
                  game_leveldrawsubmit ("level draw submit",        0),
                  game_uidraw          ("ui draw",                  0),
                  game_gamestatedraw   ("game state draw (total)",  0),
                  game_total           ("total",                   40),
                  game_update          ("update",                   0);

void Game::SetPlayer(Character *p)
{
    mPlayer = p;
    if (p != NULL)
        printf("*** Yay! Setting player #%d***\n", p->GetId());
    else 
        printf("*** Clearing player ***\n");
    if (GetLevels()->IsLevelLoaded() && GetLevel()->GetCharacterSync())
        GetLevel()->GetCharacterSync()->SetPlayer(mPlayer);
    else if (p != NULL)
    {
//        printf("***\x07 ERROR! trying to set player before level is loaded!!***\n");
    }
}

bool Game::Update(const GameTime &time)
{
    GraphicsDevice *device = GraphicsDevice::GetInstance();
    if (mGameInProgress)
    {
        mMultiplayer->Update();
        game_update.StartFrame();
        mFrames++;
        RenderContext rc(*device, *GetGlobalCamera());
        mLevels->Update(rc, time, PauseState(IsRenderPaused(), IsPaused(), IsCharacterPaused()), *this);
        mUiCore->GetGameUi().Update(time);
        mGameThread->Execute("UpdateGame");
  //    GetGlobalUiEventDispatcher()->Update();
        if (!IsPaused() && !IsCharacterPaused() && GetPlayer())
        {
            mLevels->RunEvents(GetPlayer()->GetPosition(), *this);
            GetGlobalEventTimer()->Update(time.elapsed);
            GetGlobalEventDispatcher()->Update();
            LocationEvents::GetInstance()->NotifyLocation(GetPlayer()->GetPosition());
        }
        game_update.EndFrame();
    }
    return true;
}

bool Game::Notify(const char *e, const char *info)
{
    GetGlobalEventDispatcher()->Fire(e, info);
    return true;
}

extern void SetLuaLights(GraphicsDevice &device);

void Game::SetFog(bool enable)
{
    GraphicsDevice *device = GraphicsDevice::GetInstance();
    if (enable)
    {
        device->SetColor(Vector4(1.f, 1.f, 1.f, 0.5f));
        device->SetFog(Vector4(0.2f, 0.2f, 0.4f, 0.0f), 0.3f);
        device->SetFog(Vector4(0.1f, 0.1f, 0.2f, 0.0f), 0.3f);
    }
    device->EnableFog(enable);
}

void Game::Draw(const GameTime &time)
{
    GraphicsDevice *device = GraphicsDevice::GetInstance();

    game_total.EndFrame();
    game_total.StartFrame();

    static Material material;

    device->EnableColorMaterial(false);

    ParticleFxDrawable::SetGameTime(time);
    RenderContext renderContext(*device, *GetGlobalCamera());

    SetGlobalRenderContext(renderContext);

    SetLuaLights(*device);

    if (GetPlayer())
        GetGlobalCamera()->SetLookAt(GetPlayer()->GetPosition());
    else GetGlobalCamera()->SetLookAt(Vector3(0.f, 0.f, 0.f));
        
    device->SetProjection(GetGlobalCamera()->GetProjection());
    device->SetMaterial(material);

    if (!mRenderPaused)
    {
        game_track.StartFrame();
        mLevels->Draw(renderContext, time, PauseState(IsRenderPaused(), IsPaused(), IsCharacterPaused()));
        game_track.EndFrame();

        game_leveldrawsubmit.StartFrame();
        RenderSet::GetInstance()->Render("level", renderContext);
        device->EnableFog(false);
        RenderSet::GetInstance()->Render("level-floor", renderContext);
        if (!IsLowEndDevice())
            RenderSet::GetInstance()->Render("level-floor-overlay", renderContext);
        RenderSet::GetInstance()->Render("fx", renderContext);
        RenderSet::GetInstance()->Render("main",  renderContext);
        game_leveldrawsubmit.EndFrame();

        RenderSet::GetInstance()->Render("selector", renderContext);
        QuadRenderer::RenderDeferred();
        
        profiler->StartTime("messages_hud");        
        game_uidraw.StartFrame();
        mUiCore->GetGameUi().Draw(renderContext);
        game_uidraw.EndFrame();
        profiler->EndTime("messages_hud");
    }
}