#include "GameState.h"
#include "Character.h"
#include "Sync.h"
#include "level/LevelManager.h"
#include "level/Level.h"

#include "input/GameInput.h"
#include "text/Text.h"
#include "map/TileMap.h"
#include "map/Map.h"
#include "render/QuadRenderer.h"
#include "render/LocalGameServices.h"
#include "render/OverheadCamera.h"
#include "render/DeviceTexture.h"
#include "ui/GameUI.h"
#include "text/Glyph.h"
#include "render/DrawList.h"
#include "render/Material.h"
#include "render/GraphicsDevice.h"
#include "render/RenderContext.h"
#include "render/ParticleEffects.h"
#include "render/RenderSet.h"
#include "render/ContentLoader.h"
#include "audio/Audio.h"
#include "time/Timer.h"

#include "game/UiInstance.h"

#include "core/global.h"
#include "core/strs.h"

#include "time/FrameRateTracker.h"
#include "core/Profiler.h"

#include "render/ContentLoader.h"

#include "luautil/LuaThread.h"

#include "core/Dictionary.h"
#include "serialize/LuaSerialize.h"
#include "render/ParticleFxDrawable.h"

extern "C"
{
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}

#include <string.h>

Vector2    _globalTouchPoint;

RenderContext *_globalRenderContext = 0;

void SetGlobalRenderContext(const RenderContext &rc)
{
    if (_globalRenderContext)
        delete _globalRenderContext;
    _globalRenderContext = new RenderContext(rc);
}

RenderContext &GetGlobalRenderContext()
{
    return *_globalRenderContext;
}

GameState::GameState(const core::Size &framesz) :
    services(0), background(0), title(0), camera(0), input(0), quads(0), 
    frames(0), levels(0), ui(0), player(0), 
    paused(false),
    renderables(0), anyRendered(false), audio(0), 
    mInterpreter(0), mGameThread(0)
{
    if (!Serialize::GetDictionary())
    {
        Dictionary *dictionary = new Dictionary;
        Serialize::SetDictionary(dictionary);
    }
    if (!UIController::GetInstance())
        UIController::CreateInstance(this);
    if (!LuaInterpreter::GetInstance())
        LuaInterpreter::CreateInstance();
    if (!ParticleEffects::GetInstance())
        ParticleEffects::CreateInstance();
    if (!ContentLoader::GetInstance())
        ContentLoader::CreateInstance();
    uicontroller = UIController::GetInstance();
    mUiInstance = new UiInstance;
    Initialize(framesz);
}

GameState::~GameState()
{
    const Dictionary *dictionary = Serialize::GetDictionary();
    if (dictionary)
    {
        Serialize::SetDictionary(0);
        delete dictionary;
    }
    if (UIController::GetInstance())
        UIController::DestroyInstance();
    if (LuaInterpreter::GetInstance())
        LuaInterpreter::DestroyInstance();
    if (ParticleEffects::GetInstance())
        ParticleEffects::DestroyInstance();
    delete mUiInstance;
    // $TODO a lot more to destroy here
}

Level *GameState::GetLevel()
{
    return levels->GetLevel();        
}

Character *GameState::GetPlayer()
{
    return player;
}

static Vector2 GetPositionVector2(const Vector3 &player, const Vector3 &map)
{
    return Vector2(player.x - map.x, player.y - map.y);
}

static core::Point GetPositionPoint(const Vector3 &player, const Vector3 &map)
{
    return core::Point(int(player.x - map.x), int(player.y - map.y));
}

Vector3 GameState::GetPlayerTileCenter()
{
    Map *map = GetLevel()->GetMap();
    Vector3 mp(map->GetPosition());
    core::Point p = TileMaps::GetTileLocationAtWorld(map->GetMapModel(), GetPositionPoint(GetPlayer()->GetPosition(), map->GetPosition()));
    core::Size tileSz = map->GetMapModel()->GetTileSize();
    return Vector3(p.x * float(tileSz.width)  + mp.x,
                   p.y * float(tileSz.height) + mp.y, 0.f);
}

ITileInstance *GameState::GetPlayerTileAt()
{
    Map *map = GetLevel()->GetMap();
    return TileMaps::GetTileInstanceAtWorld(map->GetMapModel(), GetPositionPoint(GetPlayer()->GetPosition(), map->GetPosition()));
}

void GameState::SetPlayerTile(int index)
{
    Map *map = GetLevel()->GetMap();    
    map->SetPlayerTile(GetPositionVector2(GetPlayer()->GetPosition(), map->GetPosition()), index);
}
            
int GameState::GetTileIdAt(const Vector2 &p)
{
    return GetLevel()->GetMap()->GetTileIdAt(p);
}

extern XmlReadContext *ReadXmlConfigText(const char *);

bool inside(const core::Point &point, const core::Rectangle &area)
{
    int areax2 = area.x + area.width, areay2 = area.y + area.height;
    return (point.x >= area.x && point.x <= areax2 &&
            point.y >= area.y && point.y <= areay2);
}

core::Point touch(0, 0);

void StartChanged(Toggle *toggle, void *context)
{
    if (toggle->GetState() && context)
    {
        GameState *state = (GameState *)context;
        touch = state->input->GetTouch();
        if (false/*state->state == GameState::INTRO*/)
        {
//          state->state = GameState::GAME;
            state->effects.FadeBackgroundToGame();
            state->ShowMainMenu();            
        }
        else if (UIController::GetInstance()->HandleTouch(touch, *(state->camera)))
        {
            //..
        }
        else if (true/*state->state == GameState::GAME*/)
        {
            state->SetPaused(true);
        }
        toggle->SetState(false);
    }    
}

bool GameState::IsPaused() const
{
    return uicontroller->IsAnyVisible();
}

void GameState::SetPaused(bool v)
{
    if (!IsPaused())
        uicontroller->SetVisible(UIController::PAUSE_MENU);
}

void GameState::Initialize(const core::Size &framesz)
{
    if (!services)
    {
        effects.Darken(1);
        effects.FadeGameToBackground(1);
        services    = new LocalGameServices(framesz, new GraphicsDevice(framesz), ContentLoader::GetInstance());
        
        input       = new GameInput;
        input->start.SetStateChangedFunction(StartChanged, this);
        
        renderables = ReadXmlConfigText(globalReadTextFile("general.xml"));        
        background  = new DeviceTexture("Black");
        title       = new DeviceTexture("Black");
        camera      = new OverheadCamera(*(services->device), 45.0f);
        camera->SetDepthScale(0.8f);

        RenderContext rc(*(services->device), *camera);

        quads       = new QuadRenderer(/*rc*/);
        levels      = new LevelManager(services->loader, &multiplayer);
//      const char *pages[] = { "Ringbearer_S64B0I0.xml", "Ringbearer_S64B0I0.xml", "Ringbearer_S64B0I0.xml", 0 };
//      const char *pages[] = { "Hobbiton Brushhand_S64B0I0_drop.xml", "Hobbiton Brushhand_S64B0I0_drop.xml", "Hobbiton Brushhand_S64B0I0_drop.xml", 0 };
//      const char *pages[] = { "Aniron_S64B0I0_drop.xml", "Aniron_S64B0I0_drop.xml", "Aniron_S64B0I0_drop.xml", 0 };
//      const char *pages[] = { "Font0_Drop.xml", "Font0_Drop.xml", "Font0_Drop.xml", 0 };
        const char *pages[] = { "OptimusPrincepsSemiBold_S64B1I0.xml", "OptimusPrincepsSemiBold_S64B1I0.xml", "OptimusPrincepsSemiBold_S64B1I0.xml", 0 };
        ui          = new GameUI(pages);
        
        audio = new Audio;
        
        audio->Load     ("battle013.wav",   "explosion");
        audio->Load     ("cardoor.caf",     "bump");        
        audio->Load     ("xylophone.caf",   "fanfare");            
        audio->Load     ("indiandrums.caf", "unlock");        
        audio->Load     ("ping.wav",        "pickup");      

        audio->SetSource("logo",            "logo");
        audio->SetSource("explosion",       "explosion");
        audio->SetSource("bump",            "bump");
        audio->SetSource("tap",             "tap");
        audio->SetSource("fanfare",         "fanfare");
        audio->SetSource("pickup",          "pickup");
        audio->SetSource("unlock",          "unlock");
        
        audio->Play("logo", 0.2f);

        mInterpreter = LuaInterpreter::GetInstance();
extern const char *_g_MainLuaFile;
        const char *luaFile = _g_MainLuaFile;//"Game.lua";
        mInterpreter->SetGlobalData("GameState",    this);
        mInterpreter->SetGlobalData("UIController", &uicontroller);
        if (!mInterpreter->RunFile(globalTranslatePath(luaFile)))
            printf("Execution of '%s' failed!\n", luaFile);
        mGameThread = mInterpreter->CreateThread(this);
    }
}

void GameState::PlayUISound()
{
    audio->Play("bump");    
}

void GameState::Start()
{
    levels->LoadNext();
    StartNewLife();
    input->enabled = true;
}

//#ifndef WIN32
extern Fader accelerometerFader;
//#endif

void GameState::ShowMainMenu()
{
//  SetPaused(true);
//#ifndef WIN32
    accelerometerFader.Set(1.0f);
//#endif
    uicontroller->ClearStack();
    uicontroller->SetVisible(UIController::MAIN_MENU);
}

void GameState::Restart()
{
    levels->ReloadCurrent();
    StartNewLife();
}
/*
void GameState::AddCharacter(Character *ch)
{
    levels->GetLevel()->AddCharacter(ch);
}
*/

void GameState::StartNewLife()
{   
    Map *map = GetLevel()->GetMap();

    Vector3 position(map->GetPosition() + map->GetStartPoint());
    Vector3 pos(position.x, position.y, 0.f);    

	if (player)
	{
        player->SetDynamicMap(0);
	    delete player;
	}

    mGameThread->Execute("GameInitialize");
}

FrameRateTracker  leveldrawlogic  ("level draw logic",         0),
                  leveldrawsubmit ("level draw submit",        0),
                  uidraw          ("ui draw",                  0),
                  gamestatedraw   ("game state draw (total)",  0),
                  total           ("total",                   40),
                  update          ("update",                   0);

void GameState::SetPlayer(Character *p)
{
    player = p;
    if (GetLevel() && GetLevel()->GetCharacterSync())
        GetLevel()->GetCharacterSync()->SetPlayer(player);
}

bool GameState::Update(const GameTime &time)
{
    multiplayer.Update();
    update.StartFrame();
    frames++;
    input->Update(time);
    RenderContext rc(*(services->device), *camera);
    levels->Update(rc, time, IsPaused());
    ui->Update(time);
    eventTimer.Update(time.elapsed);
    mGameThread->Execute("UpdateGame");
    if (!IsPaused())
    {
        levels->RunEvents(GetPlayer()->GetPosition(), *this);
    }
    update.EndFrame();
    return true;
}

bool GameState::Notify(const char *e)
{
    eventDispatcher.Fire(e);
    return true;
}

void GameState::RenderEffects(const GameTime &time)
{
 // quads->RenderBackground(background, effects.background.GetValue());
    quads->RenderOverlay(background, Vector4(0.0f, 0.0f, 0.0f, effects.black.GetValue()), 2.0f);
    quads->RenderOverlay(background, Vector4(1.0f, 1.0f, 1.0f, effects.white.GetValue()), 2.0f);
}

FrameRateTracker track("track", 40);

extern void SetDefaultLightingType(int type);
extern void PushLightingType();
extern void PopLightingType();

#include "xml/XmlReadContext.h"
#include "gamecore/IGameObject.h"
#include "gamecore/IRenderable.h"

void GameState::Draw(const GameTime &time)
{
    total.EndFrame();
    total.StartFrame();
    
    static Material material;

    ParticleFxDrawable::SetGameTime(time);
    RenderContext renderContext(*(services->device), *camera);

    SetGlobalRenderContext(renderContext);
    
    if (!anyRendered) // $TODO is this necessary?
    {
      // any first frame set up right here
        effects.UnDarken(3000);
        anyRendered = true;
    }

	camera->SetLookAt(GetPlayer()->GetPosition());
    services->device->SetProjection(camera->GetProjection());

    services->device->SetMaterial(material);

    track.StartFrame();
    levels->Draw(renderContext, time);
    track.EndFrame();        

    leveldrawsubmit.StartFrame();
    RenderSet::GetInstance()->Render("main", renderContext);
    leveldrawsubmit.EndFrame();

    RenderSet::GetInstance()->Render("selector", renderContext);
    
    profiler->StartTime("messages_hud");        
    uidraw.StartFrame();
    if (!uicontroller->IsAnyVisible())
        DrawHud(renderContext);
    ui->Draw(renderContext);    
    profiler->EndTime("messages_hud");

    ParticleEffects::GetInstance()->Render(renderContext, time);
    quads->RenderQuads();

    profiler->StartTime("fx");
    PushLightingType();
    SetDefaultLightingType(2);
    RenderEffects(time);
    profiler->EndTime("fx");

    //profiler->StartTime("ui_draw");    
    //uicontroller->Draw(renderContext, *ui);
    //profiler->EndTime("ui_draw");
    mUiInstance->UpdateAndRender(time, renderContext);
    PopLightingType();
    uidraw.EndFrame();
}

void GameState::DrawHud(RenderContext &context)
{
    if (uicontroller->IsAnyVisible())
        return;

    // $TODO ..

    return;
}