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
#include "render/OverheadCamera.h"
#include "render/DeviceTexture.h"
#include "ui/GameUI.h"
#include "ui/UiCore.h"
#include "text/Glyph.h"
#include "render/DrawList.h"
#include "render/Material.h"
#include "render/GraphicsDevice.h"
#include "render/RenderContext.h"
#include "render/ParticleEffects.h"
#include "render/QuadListRenderer.h"
#include "render/RenderSet.h"
#include "render/ContentLoader.h"
#include "time/Timer.h"

#include "luautil/LuaEvents.h"
#include "luautil/LuaDefer.h"
#include "luautil/LocationEvents.h"

#include "Glyphs.h"

#include "LuaFunctions.h"

#include "game/UiInstance.h"
#include "game/Game.h"

#include "core/global.h"
#include "core/strs.h"

#include "time/FrameRateTracker.h"
#include "core/Profiler.h"

#include "render/ContentLoader.h"

#include "luautil/LuaThread.h"

#include "core/Dictionary.h"
#include "serialize/LuaSerialize.h"
#include "render/ParticleFxDrawable.h"

#include "level/PauseState.h"

#include "GLUtils.h"
#include <OpenGLES/ES1/glext.h>

extern "C"
{
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}

FrameRateTracker track("track", 40);

extern void SetDefaultLightingType(int type);
extern void PushLightingType();
extern void PopLightingType();

#include "xml/XmlReadContext.h"
#include "gamecore/IGameObject.h"
#include "gamecore/IRenderable.h"
#include "audio/SoundManager.h"

#include <string.h>

Vector2 _globalTouchPoint;

RenderContext *_globalRenderContext = 0;

GameState *GameState::mInstance = 0;

extern "C" bool IsHiResDevice();

extern "C" void SetScreenTouchBegan(int index, int x, int y)
{
    if (GameState::HasInstance()) {
        GameState::GetInstance()->GetInput().ScreenTouchBegan(index, core::Point(x, y));
    }
}

extern "C" void SetScreenTouchMoved(int index, int x, int y)
{
    if (GameState::HasInstance()) {
        GameState::GetInstance()->GetInput().ScreenTouchMoved(index, core::Point(x, y));
    }
}

extern "C" void SetScreenTouchStationary(int index, int x, int y)
{
    if (GameState::HasInstance()) {
        GameState::GetInstance()->GetInput().ScreenTouchStationary(index, core::Point(x, y));
    }
}

extern "C" void SetScreenTouchEnded(int index, int x, int y)
{
    if (GameState::HasInstance()) {
        GameState::GetInstance()->GetInput().ScreenTouchEnded(index, core::Point(x, y));
    }
}

void SetGlobalRenderContext(const RenderContext &rc)
{
    if (_globalRenderContext) {
        delete _globalRenderContext;
    }
    _globalRenderContext = new RenderContext(rc);
}

RenderContext &GetGlobalRenderContext()
{
    return *_globalRenderContext;
}

GameState *GameState::GetInstance()
{
    if (!GameState::mInstance) {
        printf("\nERROR GameState instance not set!  Call CreateInstance()\n");
        exit(-1);
    }
    return GameState::mInstance;
}

bool GameState::HasInstance()
{
    return GameState::mInstance != 0;
}

void GameState::DestroyInstance()
{
    if (GameState::mInstance) {
        delete GameState::mInstance;
        GameState::mInstance = 0;
    }
}

GameState *GameState::CreateInstance(const GameDimensions &dimensions)
{
    if (GameState::mInstance) {
        printf("\nERROR GameState instance ALREADY set!  Call GetInstance(), not CreateInstance()\n");
        exit(-1);
    }
    GameState::mInstance = new GameState(dimensions);
    return GetInstance();
}

Game *GetGlobalGame()
{
    GameState *state = GameState::GetInstance();
    core_assert(state);
	if (!state)
	    return 0;
    Game *game = state->GetGame();
    core_assert(game);
    return game;
}

OverheadCamera *GetGlobalCamera()
{
    GameState *state = GameState::GetInstance();
    core_assert(state);
    return state->GetCamera();
}

GameInput *GetGlobalInput()
{
    GameState *state = GameState::GetInstance();
    core_assert(state);
    return &state->GetInput();
}

/*
EventDispatcher *GetGlobalEventDispatcher()   { return GameState::GetInstance()->GetEventDispatcher(); }
EventDispatcher *GetGlobalUiEventDispatcher() { return GameState::GetInstance()->GetUiEventDispatcher(); }
EventTimer      *GetGlobalEventTimer()        { return GameState::GetInstance()->GetEventTimer(); }
*/

GameState::GameState(const GameDimensions &dimensions) :
    mBackground(0), mCamera(0), mInput(0), mQuads(0), 
    mGameUi(0), mGame(0),
    mRenderables(0), mGameThread(0), mUiCore(0), mFrames(0), mFpsFrames(0), mFpsTimeMs(1), 
    mFpsRunningAverage(30.0),
    mOverlayColor(Vector4(1.f, 1.f, 1.f, 0.f)),
    mDimensions(dimensions), mOverlay(0)
{
    if (!Serialize::GetDictionary()) {
        Dictionary *dictionary = new Dictionary;
        Serialize::SetDictionary(dictionary);
    }
    if (!LocationEvents::GetInstance()) {
        LocationEvents::CreateInstance(&mEventDispatcher);
    }
    if (!LuaInterpreter::GetInstance()) {
        LuaInterpreter::CreateInstance();
    }
    if (!ParticleEffects::GetInstance()) {
        ParticleEffects::CreateInstance();
    }
    if (!ContentLoader::GetInstance()) {
        ContentLoader::CreateInstance();
    }
    if (!Glyphs::GetInstance()) {
        Glyphs::CreateInstance();
    }
    mUiInstance = new UiInstance;
    Initialize(dimensions);
}

GameState::~GameState()
{
    if (mGame) {
        delete mGame;
    }
    mGame = 0;

    const Dictionary *dictionary = Serialize::GetDictionary();
    if (dictionary) {
        Serialize::SetDictionary(0);
        delete dictionary;
    }

    if (LuaInterpreter::GetInstance()) {
        LuaInterpreter::DestroyInstance();
    }
    if (ParticleEffects::GetInstance()) {
        ParticleEffects::DestroyInstance();
    }
    delete mUiInstance;
    // $TODO a lot more to destroy here
}

extern XmlReadContext *ReadXmlConfigText(const char *);

void GameState::StartAudio()
{
    if (!SoundManager::GetInstance()) {
        SoundManager::CreateInstance();
    }
    SoundManager *sounds = SoundManager::GetInstance();
    sounds->Load();
}

void GameState::PrintLoadedTextures()
{
    if (ContentLoader::GetInstance()) {
		ContentLoader::GetInstance()->PrintLoadedTextures();
    }
}

UiCore *GameState::GetUiCore()
{
    return mUiCore;
}

extern const char *_g_MainLuaFile;

void GameState::Initialize(const GameDimensions &dimensions)
{
    const char *pagesSmall[] = { "OptimusPrincepsSemiBold_S64B1I0_512.xml",
                              /*   "OptimusPrincepsSemiBold_S64B1I0_512.xml", 
                                 "OptimusPrincepsSemiBold_S64B1I0_512.xml", */ 0, 0, 0 };
    const char *pagesLarge[] = { "OptimusPrincepsSemiBold_S64B1I0_1024.xml",
                              /*   "OptimusPrincepsSemiBold_S64B1I0_1024.xml", 
                                 "OptimusPrincepsSemiBold_S64B1I0_1024.xml", */ 0, 0, 0 };
    const char **pages = pagesSmall;
	
    if (IsHiResDevice()) {
        pages = pagesLarge;
    }
	
	printf("TEXT PAGES: %s\n", pages[0]);
	  
    mGameUi = new GameUI(pages);
    mUiCore = new UiCore(dimensions, *mGameUi, *mQuads, mUiEventDispatcher);

    Vector2 v = dimensions.GetViewportSize();
    core::Size sz = core::Size(int(v.x), int(v.y));
    GraphicsDevice::CreateInstance(sz);
    GraphicsDevice *device = GraphicsDevice::GetInstance();

    mEffects.Darken(1);
    mEffects.FadeGameToBackground(1);
    
    mInput = new GameInput;
    mInput->SetTransform(mDimensions.GetTouchTransform());
    
    mBackground  = new DeviceTexture("Black");
    mOverlay = new DeviceTexture("white");
    mCamera = new OverheadCamera(*device, 45.0f);
    mCamera->SetDepthScale(0.8f);

    Matrix ma, mb;
    ma.scale(Vector3(1.f, -1.f, 1.f));
    mb.translate(Vector3(0.f, 1.f, 0.f));
    ma = ma * mb;
    device->SetBaseUvTransform(ma);

    mQuads = new QuadRenderer();

    StartAudio();

    LuaInterpreter *interpreter = LuaInterpreter::GetInstance();

    /*
    const char *luaFile = _g_MainLuaFile;//"Game.lua";
    interpreter->SetGlobalData("GameState", this);
    if (!interpreter->RunFile(globalTranslatePath(luaFile)))
        core_abort(("Execution of '%s' failed!\n", luaFile));
    */
    mGameThread = interpreter->CreateThread(this);
    mGame = new Game(mUiCore, mGameThread, &mMultiplayer);
    // first run lua after game?
    const char *luaFile = _g_MainLuaFile;//"Game.lua";
    interpreter->SetGlobalData("GameState", this);
    if (!interpreter->RunFile(globalTranslatePath(luaFile)))
        core_abort(("Execution of '%s' failed!\n", luaFile));
}

void GameState::LoadRenderables()
{
    mRenderables = ReadXmlConfigText(globalReadTextFile("general.xml"));
    mUiCore->SetRenderables(mRenderables);
}

// $TODO much of this should be owned by LevelManager
void GameState::StartGameState()
{   
    mGameThread->Execute("GameStateInitialize");
}

FrameRateTracker  leveldrawlogic  ("level draw logic",         0),
                  leveldrawsubmit ("level draw submit",        0),
                  uidraw          ("ui draw",                  0),
                  huddraw         ("hud draw",                 0),
                  gamestatedraw   ("game state draw (total)",  0),
                  total           ("total",                   40),
                  update          ("update",                   0);

extern EventDispatcher *GetGlobalUiEventDispatcher();

float GameState::GetFpsAverage() const
{
//	return (float)(((double)mFpsFrames / (double)mFpsTimeMs) * 1000.0);
    return (float)mFpsRunningAverage;
}

void GameState::ResetFpsAverage()
{
	mFpsRunningAverage = 30.0; // assume this
	mFpsFrames = 0;
	mFpsTimeMs = 1;
}

void GameState::UpdateFpsAverage(unsigned framesToAdd, unsigned elapsed)
{
	if (elapsed > 500) // if it's a crazy hitch, throw it out
		return;
	mFpsRunningAverage = (double(framesToAdd) / double(elapsed) * 1000.0) * 0.2 + mFpsRunningAverage * 0.8;
	mFpsFrames += framesToAdd;
	mFpsTimeMs += elapsed;
}

bool GameState::Update(const GameTime &time)
{
    GraphicsDevice *device = GraphicsDevice::GetInstance();
    update.StartFrame();
    mInput->Update(time);
    mGameThread->Execute("UpdateGameState");
    mInput->Update(time);
    RenderContext rc(*device, *mCamera);
    mGame->Update(time);
    GetGlobalUiEventDispatcher()->Update();
    LuaFunctionsDeferredUpdate();
    update.EndFrame();
    LuaDeferUpdate(mGameThread->GetLuaState());
    mFrames++;
	UpdateFpsAverage(1, time.elapsed);
    return true;
}

bool GameState::Notify(const char *e, const char *info)
{
    mEventDispatcher.Fire(e, info);
    return true;
}

void GameState::RenderEffects(const GameTime &time)
{
//  mQuads->RenderBackground(background, effects.background.GetValue());
//  mQuads->RenderOverlay(mBackground, Vector4(0.0f, 0.0f, 0.0f, mEffects.black.GetValue()), 2.0f);
//  mQuads->RenderOverlay(mBackground, Vector4(1.0f, 1.0f, 1.0f, mEffects.white.GetValue()), 2.0f);
}

void GameState::UnloadAll()
{
	Game *game = GetGame();
	if (game)
	{
		game->EndGame();
	}
	ObjectTileSetLoader::RemoveAllTileSets();	
	ContentLoader::GetInstance()->UnloadAllTextures();
}

Character *GameState::GetPlayer()
{
    return mGame ? mGame->GetPlayer() : 0;
}

PauseState GameState::GetPauseState() const
{
    if (!mGame) {
        return PauseState(true, true, true);
    }
    return PauseState(mGame->IsRenderPaused(), mGame->IsPaused(), mGame->IsCharacterPaused());
}

void GameState::LookAt(const Vector3 &p)
{
    GetGlobalCamera()->SetLookAt(p);
    GraphicsDevice *device = GraphicsDevice::GetInstance();
    device->SetProjection(GetGlobalCamera()->GetProjection());
}

void GameState::LookAt(Movable *movable)
{
    LookAt(movable ? movable->GetPosition() : Vector3(0.f, 0.f, 0.f));
}

void GameState::DrawParticles(const GameTime &time)
{
    GraphicsDevice *device = GraphicsDevice::GetInstance();
    SetDefaultLightingType(2);
    //GetLights(PASS_PARTICLES).Apply(*device);
    ParticleFxDrawable::SetGameTime(time);
    profiler->StartTime("particles");
    //PushLightingType();

    device->SetColor(Vector4(1.f, 1.f, 1.f, 1.f));
    PauseState paused(GetPauseState());
    if (mGame && !paused.render) {
        ParticleEffects::GetInstance()->Render(GetGlobalRenderContext(), time);
        mQuads->RenderQuads();
    }
    RenderEffects(time); // this doesn't currently do anything
    //PopLightingType();
    profiler->EndTime("particles");
}

void GameState::DrawUi(const GameTime &time)
{
    GraphicsDevice *device = GraphicsDevice::GetInstance();
    uidraw.StartFrame();
    SetDefaultLightingType(1);
    GetLights(PASS_UI).Apply(*device, *mCamera);
    mUiInstance->UpdateAndRender(time, GetGlobalRenderContext());
    uidraw.EndFrame();
}

void GameState::DrawHud(const GameTime &time)
{
    GraphicsDevice *device = GraphicsDevice::GetInstance();
    SetDefaultLightingType(0);
    GetLights(PASS_UI).Apply(*device, *mCamera);
    huddraw.StartFrame();
    device->SetColor(Vector4(1.f, 1.f, 1.f, 1.f));
    profiler->StartTime("messages_hud");
    mUiCore->GetGameUi().Draw(GetGlobalRenderContext());
    huddraw.EndFrame();
    profiler->EndTime("messages_hud");
}

void GameState::DrawOverlay(const GameTime &time)
{
    GraphicsDevice *device = GraphicsDevice::GetInstance();
    if (mOverlayColor.w >= 0.05f)
    {
        SetDefaultLightingType(2);
        GetLights(PASS_OVERLAY).Apply(*device, *mCamera);
        // terrible hack, but it works..
        OverheadCamera c(*device, 45.f);
        // I wish I had commented what I am trying to accomplish here at the time I wrote it
        // but I believe this is setting up the projection and view matrices for the colored overlay
        Matrix m;
        m.data[0]  =   1.61f;
        m.data[5]  =   2.41f;
        m.data[10] =  -1.00f;
        m.data[11] =  -1.00f;
        m.data[14] =  -10.01f;
        c._SetProjection(m);
        device->SetProjection(m);

        m = Matrix();
        m.data[0]  =  -1.00f;
        m.data[5]  =   0.61f;
        m.data[6]  =  -0.79f;
        m.data[9]  =  -0.79f;
        m.data[10] =  -0.61f;
        m.data[13] =  -0.00f;
        m.data[14] =  -309.02f;
        m.data[15] =   1.f;
        c._SetView(m);

        RenderContext rc(*device, c);
        SetGlobalRenderContext(rc);

        Vector3 size(1000.f, 1000.f);
        float distance = -150.f;
        device->EnableDepthTest(false);
        QuadRenderer::RenderScreenAlignedQuad(mOverlay, mOverlayColor,
                                              Vector3(size.x, size.y, distance), Vector3(-size.x, -size.y, distance),
                                              Vector2(0.f, 1.f), Vector2(1.f, 0.f));
        device->EnableDepthTest(true);
    }
}

void GameState::DrawLevel(const GameTime &time)
{
    static Material material;
    
    GraphicsDevice *device = GraphicsDevice::GetInstance();
    RenderContext renderContext(*device, *GetGlobalCamera());
    SetGlobalRenderContext(renderContext);

    LookAt(GetPlayer());

    PauseState paused(GetPauseState());

    device->SetMaterial(material);
    //glEnable(GL_COLOR_MATERIAL);
    if (mGame && !paused.render) {
        GetLights(PASS_LEVEL).Apply(*device, *mCamera);
        leveldrawlogic.StartFrame();
        mGame->GetLevels()->Draw(renderContext, time, paused);
        leveldrawlogic.EndFrame();

        leveldrawsubmit.StartFrame();
        RenderSet::GetInstance()->Render("level", renderContext);
        RenderSet::GetInstance()->Render("level-floor", renderContext);
        RenderSet::GetInstance()->Render("level-floor-overlay", renderContext);
        RenderSet::GetInstance()->Render("fx", renderContext);
        RenderSet::GetInstance()->Render("main",  renderContext);
        leveldrawsubmit.EndFrame();

        RenderSet::GetInstance()->Render("selector", renderContext);
        QuadRenderer::RenderDeferred();
    }
}

void GameState::Draw(const GameTime &time)
{
    GraphicsDevice *device = GraphicsDevice::GetInstance();
    device->StartFrame();
    
    total.EndFrame();
    total.StartFrame();

    static Material material;
    device->EnableColorMaterial(false);
    device->SetMaterial(material);

    RenderContext renderContext(*device, *mCamera);
    SetGlobalRenderContext(renderContext);

    DrawLevel(time);
    DrawParticles(time);
    DrawHud(time);
    DrawUi(time);
    DrawOverlay(time);

    //GLSetDepthWrite(false);
    //GLDepthTest(false);
    //QuadListRenderer<CommonVertex> &r = Glyphs::GetInstance()->GetQuadRenderer();
    Matrix4f m;
    m.scale(50.1);
    renderContext.transform = m;
    Glyphs::GetInstance()->Render(renderContext, time);

    device->EndFrame();
}
