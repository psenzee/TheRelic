/*
 *  GameState.cpp
 *  GLGravity
 *
 *  Created by Paul Senzee on 12/29/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include "GameState.h"
#include "Player.h"
#include "BallCharacter.h"
#include "Level.h"

#include "input/GameInput.h"
#include "text/Text.h"
#include "map/TileMap.h"
#include "map/Map.h"
#include "render/QuadRenderer.h"
#include "render/LocalGameServices.h"
#include "render/Models.h"
#include "render/OverheadCamera.h"
#include "render/DeviceTexture.h"
#include "ui/GameUI.h"
#include "text/Glyph.h"
#include "render/DrawList.h"
#include "render/Material.h"
#include "render/GraphicsDevice.h"
#include "objects/Explosions.h"
#include "game/challenges/MutualDestruction.h"
#include "game/challenges/BumpTutorial.h"
#include "audio/Audio.h"
#include "time/Timer.h"

#include "core/global.h"
#include "core/strs.h"

#include "time/FrameRateTracker.h"

#include <string.h>

DeviceTexture *brick = 0;

GameState::GameState(const core::Size &framesz) :
    services(0), background(0), title(0), camera(0), input(0), quads(0),
    frames(0), levels(0), ui(0), score(0), levelScore(0),
    score2x(false), score3x(false), reachedExit(false), trappedTutorial(true),
    paused(false), timeRemaining(0), timeExpired(false), exitReachable(0),
    renderables(0), anyRendered(false), state(INTRO), uicontroller(this), challenge(0), audio(0), scoreContext(&scores, LevelScore()), mode(INDIGO_OCHO)
{
    Initialize(framesz);
}

Level *GameState::GetLevel()
{
    return levels->GetLevel();        
}

Level *GameState::GetPreviousLevel()
{
    return levels->GetPreviousLevel();        
}

BallCharacter *GameState::GetPlayer()
{
    return actors.GetPlayer();
}

static Vector2 GetPositionVector2(const Vector3 &player, const Vector3 &map)
{
    return Vector2(player.x - map.x, player.y - map.y);
}

static core::Point GetPositionPoint(const Vector3 &player, const Vector3 &map)
{
    return core::Point(int(player.x - map.x), int(player.y - map.y));
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
            
void GameState::ClearPickup()
{
    Map *map = GetLevel()->GetMap();    
    map->ClearPickup(GetPositionVector2(GetPlayer()->GetPosition(), map->GetPosition()));
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
        if (state->state == GameState::INTRO)
        {
            state->state = GameState::GAME;
            state->effects.FadeBackgroundToGame();
            state->ShowMainMenu();            
        }
        else if (state->uicontroller.HandleTouch(touch, *(state->camera)))
        {
            //..
        }
        else if (state->state == GameState::GAME)
        {
            if (!state->IsPaused())
            {
                if (state->mode == GameState::MAZES_ONLY)
                    state->uicontroller.SetVisible(UIController::MAZE_PAUSE_MENU);
                else 
                    state->uicontroller.SetVisible(UIController::PAUSE_MENU);
            }
        }
        toggle->SetState(false);
    }    
}

bool GameState::IsPaused() const
{
    return uicontroller.IsAnyVisible();
}

void GameState::SetPaused(bool v)
{
    paused = v;
}

int _g_ignoreBall = -1; // $hack for now

bool GameState::SpaceOpen(const core::Point &p, int ignore_ball)
{
    Map *map = GetLevel()->GetMap();
    if (Map::IsCollidableTile(map->GetMapModel()->GetTileId(p)))
        return false;
    for (size_t i = 0, sz = actors.list.size(); i < sz; i++)
    {
        if ((int)i == ignore_ball)
            continue; 
        BallCharacter *ball = actors.list[i];
        core::Point    bp   = map->GetTileLocationAt(v2(ball->GetPosition() - Vector3(0.0f, 16.0f, 0.0f)));
        if (bp.x == p.x && bp.y == p.y)
            return false;
    }
    return true;
}

static bool SpaceOpenCallback(ITileMapModel *, const core::Point &p, void *context)
{
    GameState *state = (GameState *)context;
    return state->SpaceOpen(p, _g_ignoreBall);
}

void GameState::UncollideBall(int index)
{
    Map           *map  = GetLevel()->GetMap();    
    BallCharacter *ball = actors.list[index];
    core::Point    p    = map->GetTileLocationAt(v2(ball->GetPosition() - Vector3(0.0f, 16.0f, 0.0f))),
                   at;
    _g_ignoreBall = index;
    TileMaps::FindClosestTile(map->GetMapModel(), p, at, SpaceOpenCallback, this);
    _g_ignoreBall = -1;
    ball->SetPosition(v2(map->GetPointInWorld(at) + Vector3(0.0f, 16.0f, 0.0f)));
}

void GameState::UncollideBalls()
{
    for (int i = 0, sz = (int)actors.list.size(); i < sz; i++)
        UncollideBall(i);
}

struct ExplodeState
{
    GameState *state;
    Vector3    at;
    Level     *level;
    int        stage;
};

void ExplodeInStages(void *context)
{
    ExplodeState *es     = (ExplodeState *)context;
    Map          *map    = es->level->GetMap();    
    core::Size    sz     = map->GetMapModel()->GetMapSizeInPixels();
    switch (es->stage)
    {
    case 0:
        map->Explode(es->at, sz.width * 0.6f, false);
        es->stage = 1;
        es->state->eventTimer.Add(ExplodeInStages, es, 200);
        break;
    case 1:
        map->Explode(es->at, sz.width * 0.4f, true);
        es->stage = 2;
        es->state->eventTimer.Add(ExplodeInStages, es, 200);
        break;
    case 2:
        map->Explode(es->at, sz.width * 0.3f, true);
        es->stage = 3;
        es->state->eventTimer.Add(ExplodeInStages, es, 200);            
        break;
    case 3:
        {
            // check reachability
            GameState *state = es->state;
            memset(es, 0, sizeof(ExplodeState)); // just in case..
            delete es;
            state->UncollideBalls();
            if (!state->IsExitReachable())
                state->Event_OnTrapped();        
        }
        break;
    }
}

void GameState::Explode(const Vector3 &at, float blastradius)
{
    effects.Flash(explosions->GetDuration());
    explosions->Explode(at);
    audio->Play("explosion", 1.0f);
    ExplodeState *es = new ExplodeState;
    es->state = this;
    es->at = at;
    es->stage = 0;
    es->level = GetLevel();
    eventTimer.Add(ExplodeInStages, es, 200);
}

void GameState::Initialize(const core::Size &framesz)
{
    if (!services)
    {
        effects.Darken(1);
        effects.FadeGameToBackground(1);
        services    = new LocalGameServices(framesz, 0, new ContentCache);
        
        input       = new GameInput;
        input->start.SetStateChangedFunction(StartChanged, this);
        
        renderables = ReadXmlConfigText(globalReadTextFile("general.xml"));        
        background  = new DeviceTexture("background54_small");
        title       = new DeviceTexture("i8_4");
        camera      = new OverheadCamera(services, 45.0f);    
        camera->SetDepthScale(0.8f);
        quads       = new QuadRenderer(*camera, *(services->device));
        levels      = new LevelSequence(this);        
        ui          = new GameUI(this, "text.xml");
        scores.SetFile(globalTranslateReadWritePath("scores33.bin"));
        save.SetFile(globalTranslateReadWritePath("autosave.bin"));
        mode = INDIGO_OCHO;
//      SaveGame::LoadMostRecentGame(save);
        
        explosions  = new Explosions(services, renderables, 2000);        
        brick = new DeviceTexture("Brick");
        
        audio = new Audio;
        
//        audio->Load      ("logo.wav",       "logo");
        audio->Load      ("battle013.wav", "explosion");
//        audio->Load      ("cling_1.wav",       "bump");
        audio->Load      ("cardoor.caf",       "bump");        
    //    audio->Load      ("flashbulb.wav",  "bump");        
    //    audio->Load      ("light-switch-1.wav", "tap");
    //    audio->Load      ("xylophone.wav",  "fanfare");
        //audio->Load("indiandrums.caf", "fanfare");    
        audio->Load("xylophone.caf", "fanfare");            
        audio->Load("indiandrums.caf", "unlock");        
         audio->Load      ("ping.wav",       "pickup");
//         audio->Load      ("cling_1.wav",    "bump");        
    //     audio->Load      ("flicklighter.wav", "bump");        
       //audio->Load      ("cling_1.wav",       "pickup");            
      //   audio->Load      ("cling_1.wav",    "fanfare");
        

        audio->SetSource ("logo",           "logo");
        audio->SetSource ("explosion",      "explosion");
        audio->SetSource ("bump",           "bump");
        audio->SetSource ("tap",            "tap");
        audio->SetSource ("fanfare",        "fanfare");
        audio->SetSource ("pickup",         "pickup");
        audio->SetSource ("unlock",         "unlock");        
        
        audio->Play("logo", 0.2f);
    }
}

std::vector<AABox> _debug_quads;

void GameState::_DebugRenderBoundsQuad(const AABox &box)
{
    quads->RenderScreenAlignedQuad(brick, Vector4(1.0f, 1.0f, 1.0f, 1.0f), box.minimum, box.maximum);
}

void _DebugRenderBoundsQuad(const AABox &box)
{
    extern GameState *state;
    //state->_DebugRenderBoundsQuad(box);
    _debug_quads.push_back(box);
}

void GameState::Save()
{
    if (GetLevel()->GetLevelNumber() > 34)
        return; // $TODO more elegantly
    time_t timespan;
    time(&timespan);
    LevelScore s(scoreContext.score.name, timespan, GetLevel()->GetLevelNumber(), score);
    save.Save(s);
}

void GameState::NewActiveGame()
{
    save.SetFile(globalTranslateReadWritePath("autosave.bin"));
    mode = GameState::INDIGO_OCHO;    
    score = 0;
    levelScore = 0;
    scoreContext.score.score = 0;
    scoreContext.score.level = 0;    
    Start(1);
    Save();    
}

void GameState::NewMazeOnlyGame()
{
    mode = GameState::MAZES_ONLY;
    score = 0;
    levelScore = 0;
 //   scoreContext.score.score = 0;
 //   scoreContext.score.level = 0;
    Start(1);
    Save();
}

void GameState::SaveActiveGame(int index)
{
    if (mode == INDIGO_OCHO) // can't save maze-only games .. yet
    {
        char filename[1024];
        SaveGame::GetSaveFileName(index, filename);
        save.SetFile(filename);
        Save();
        save.SetFile(globalTranslateReadWritePath("autosave.bin"));
    }
}

void GameState::LoadActiveGame(int index)
{
    mode = INDIGO_OCHO; // we can only load indigo ocho (not maze-only) games

    SaveGame::LoadGame(index, save);
    SavedStart();
    save.SetFile(globalTranslateReadWritePath("autosave.bin"));
}

void GameState::SavedStart()
{
    LevelScore score = save.GetScore();
    this->score = score.score;
    Start(score.level < 1 ? -1 : score.level);
}

static const int LEVEL_NORMALIZE_BALL_WEIGHT = 40;

static float _level_interpolate(float start, float end, int level)
{
    float t = (level - 1) / (float)LEVEL_NORMALIZE_BALL_WEIGHT;
    t = math::clamp(t, 0.0f, 1.0f);
    float d = end - start;
    return t * d + start;
}

float GameState::GetBumpMagnitude()
{
    return _level_interpolate(0.25f, 2.5f, GetLevel()->GetLevelNumber());
}

float GameState::GetBallVelocityScale()
{
    return _level_interpolate(20.0f, 200.0f, GetLevel()->GetLevelNumber());    
}

void GameState::ShowTutorial()
{
    if (state != GameState::GAME || uicontroller.IsAnyVisible())
        return;

    static const char *text[] =
    {
        "\nEach level of \\#6600ffIndigo Ocho\\#ffffff\n"
        "is a maze. The goal is\n"
        "simple - reach the exit\n"
        "in the time allowed.",
        
        "\n\nAnd with as many \\#ffff00points\\#ffffff\n"
        "as possible!",
        
        0
    };
    SetPaused(true);    
    uicontroller.ShowText("\\#6600ffMood Indigo", text);
}

void GameState::ShowMazeTutorial()
{
    if (state != GameState::GAME || uicontroller.IsAnyVisible())
        return;
    
    static const char *text[] =
    {
        "\nEach of these mazes\n"
        "is generated randomly.\n"
        "There are hundreds\n"
        "of them!",
        
        "\nUnlike in the main game,\n"
        "here there are no timers\n"
        "or exploding balls.\n",
        
        "\nEach time through\n"
        "is a different.\n"
        "experience.\n",    
        
        "\n\nHow far will \\#6600ffyou\\#ffffff go?",
        
        0
    };
    SetPaused(true);    
    uicontroller.ShowText("\\#ff0000I\\#ffff00n\\#00ff00d\\#0000ffi\\#ffff00g\\#ff0000o \\#00ff00Z\\#ff0000e\\#ffff00n", text);
}

void GameState::EndGame()
{
    if (state != GameState::GAME /*|| uicontroller.IsAnyVisible()*/)
        return;
    
    effects.FadeGameToBackground(2000);
    
    static const char *text[] =
    {
        "\\#ffff00`CONGRATULATIONS!\\#ffffff\n"
        "You've destroyed the\n"
        "ziggurat!\n\n"
        "The \\#808080Library of Babel\\#ffffff\n"
        "is no more..\n",
        
        "This is a bitter victory\n"
        "for \\#6600ffIndigo Ocho\\#ffffff.\n\n"
        "He destroyed his world to\n"
        "save another - \\#808080ours\\#ffffff.\n",
        
        "\n\nYet more adventures wait for\n"
        " \\#6600ffIndigo Ocho\\#ffffff..\n",
        
        "\n\n...\n",
        
        "\nVisit \\#8080ffwww.indigoocho.com\\#ffffff\n"
        "for more \\#6600ffIndigo Ocho\\#ffffff and\n"
        "other great new games.\n",
        
        "\\#6600ffIndigo Ocho\\#ffffff was developed\n"
        "by \\#808080AXOLOTL STUDIOS\\#ffffff.\n\n"
        "Come to\n"
        "\\#8080ffwww.axolotlstudios.com\\#ffffff\n"
        "for new games and updates.\n",
        
        "\n\n\\#6600ffIndigo Ocho\\#ffffff"
        "\n(c) 2009 AXOLOTL STUDIOS LLC",
        
        "\n\nThank you for playing!",
        
        0
    };
    SetPaused(true);    
    uicontroller.ShowText("\\#6600ff`Game Complete!", text, UIController::EXIT_END_GAME);
}

void GameState::ShowTrappedTutorial()
{
    if (state != GameState::GAME)
        return;
    
    static const char *text[] =
    {
        "\n\nYou've been trapped by\n"
        "the blast you detonated!\n\n",
        
        "\nThere is no path to the\n"
        "remaining balls and to the\n"
        "exit.  Select \\#6600ffRetry\\#ffffff to\n"
        "try again.",
        
        0
    };
    SetPaused(true);    
    uicontroller.ShowText("\\#ff0000`TRAPPED!", text, UIController::EXIT_GAME_OVER);
}

void GameState::PlayUISound()
{
    audio->Play("bump");    
}

void GameState::Start(int level)
{
    if (level == -1)
        levels->LoadNext();
    else
        levels->CompleteLoad(level);
    if (StaticModel::GlobalAnimator)
        delete StaticModel::GlobalAnimator; 
    StaticModel::GlobalAnimator = 0;    
    StartNewLife();
    if (GetLevel()->GetLevelNumber() == 2)
    {
        if (mode == MAZES_ONLY) ShowMazeTutorial();
        else                    ShowTutorial();
    }
    input->enabled = true;
}

extern Fader accelerometerFader;

void GameState::ShowMainMenu()
{
    SetPaused(true);
    accelerometerFader.Set(1.0f);    
    uicontroller.ClearStack();
    uicontroller.SetVisible(UIController::MAIN_MENU);
}

void GameState::Restart()
{
    levels->ReloadCurrent();
    if (StaticModel::GlobalAnimator)
        delete StaticModel::GlobalAnimator; 
    StaticModel::GlobalAnimator = 0;    
    StartNewLife();
}

void GameState::SendPointsMessage(unsigned score, int delay)
{
    char scorestr[256], message[256];
    sprintf(scorestr, "+%u", score);
    Text::MakePartyText(scorestr, message);
    ui->SendBallMessage(message, delay);
}

void GameState::StartNewLife()
{
    timeExpired = false;
    
    Map *map = GetLevel()->GetMap();
    
    if (GetPreviousLevel())
        GetPreviousLevel()->GetMap()->ClearEndArea();

    Vector3 position(map->GetPosition() + map->GetStartPoint());
    Vector2 pos(position.x, position.y + 16.0f);    

    actors.Clear();

    Player *player = new Player(this, GetLevel()->GetOptions()->configuration, pos);
    player->SetMap(map);
    player->TiltBias = GetLevel()->GetOptions()->tilt;
    actors.Add(player);
    actors.SetPlayerIndex(0);    

    if (challenge)
        delete challenge;
    challenge = 0;
    
    if (mode == INDIGO_OCHO)
    {
        if (GetLevel()->GetLevelNumber() > 7)
        {
            int pairs = GetLevel()->GetLevelNumber() / 10 + 1;
            challenge = new MutualDestruction(this, pairs, GetLevel()->GetLevelNumber() == 8);
        }
        else if (GetLevel()->GetLevelNumber() > 5)
            challenge = new BumpTutorial(this, GetLevel()->GetLevelNumber() == 6);
    }
        
    reachedExit              = false;
    score3x = score2x        = false;
    levelScore               = 0;
    
    if (mode == INDIGO_OCHO && GetLevel()->GetLevelNumber() >= 36)
    {
        EndGame();
    }
    
    if (mode == INDIGO_OCHO)
        timeRemaining = (GetLevel()->GetOptions()->time + levels->GetLevelNumber() * 2) * 1000;
    else
        timeRemaining = 1000; // we never run out in maze mode
}

FrameRateTracker  leveldrawlogic  ("level draw logic",         0),
                  leveldrawsubmit ("level draw submit",        0),
                  uidraw          ("ui draw",                  0),
                  gamestatedraw   ("game state draw (total)",  0),
                  total           ("total",                   40),
                  update          ("update",                   0);

bool GameState::Update(const GameTime &time)
{
    if (time.elapsed < 16)
        return false;
    update.StartFrame();
    frames++;
    input->Update(time);
    if (state == GAME)
        levels->Update(time);
    explosions->Update(time);    
    ui->Update(time);
    StaticModel::GlobalUpdate(time);
    eventTimer.Update(time.elapsed);
    if (state == GAME)
    {
        if (mode == INDIGO_OCHO && timeRemaining <= 0)
        {
            timeRemaining = 0;
            Event_OnTimeExpired();
        }
        else if (!IsPaused())
        {
            timeRemaining -= time.elapsed;
            actors.Update(time);
            if (challenge)
                challenge->Update(time);
            if (!reachedExit)
                levels->RunEvents();
        }
    }
    update.EndFrame();
    return true;
}

void GameState::RunEvent(const char *e)
{
    // we'll eventually do this in a less retarded way
    if      (starts(e, "ExitLevel"))    Event_OnExitReached();
    else if (starts(e, "PlayerKilled"))    Event_OnPlayerKilled();
    else if (starts(e, "TimeExpired"))    Event_OnTimeExpired();
    else if (starts(e, "Swirl"))        Event_OnSwirl();
    else if (starts(e, "Pickup2x"))        Event_OnPickup2x();
    else if (starts(e, "Pickup3x"))        Event_OnPickup3x();
}
 
void GameState::Narration()
{
    static int count = 0;
    if (this->state == INTRO)
    {
        /*
        if (count < 10)
            count++;
        else
        {
            this->state = GAME;
            effects.FadeBackgroundToGame();            
            ShowMainMenu();
        }
        return;
        */
    }
    
    const float START = -250.0f, END = 700.0f;
    static float titlex = 200.0f;
    static float y     = START;
    static int   state = 0;
    
    static Fader titleshow(0.0f);
    static Fader titlefade(0.0f);
    static Fader titlescale(0.0f);    
    
#define NARRATE_COLOR  "\\#ffffff"
#define NARRATE_BLACK  "\\#808080"    
#define NARRATE_INDIGO "\\#6600ff"
#define NARRATE_YELLOW "\\#ffff00"    
    
    const char *pages[] = 
    {
        0, 0,
        
        NARRATE_COLOR "In the pinnacle of the\n"
        NARRATE_BLACK "Library of Babel" NARRATE_COLOR ", sage and\n"
        "mystic 8 balls come to be.\n\n"
        
        "Eight-balls are " NARRATE_BLACK "black.\n" NARRATE_COLOR
        "But we live in a world of\n"
        "exception and so does\n"
        NARRATE_INDIGO "Indigo Ocho.\n\n"        

        NARRATE_COLOR "Something is amiss here.\n"
        "The studies quake and the\n"
        "gardens burn.\n\n"
        
        NARRATE_COLOR "An evil unfolds that threatens\n"
        "the human world.\n\n"
        
        NARRATE_COLOR "One thing is clear.\n\n"        
        "The ziggurat must come\ndown.\n\n"
        NARRATE_BLACK "`ALL 35 LEVELS!\n",

        0
    };
    
#undef NARRATE_COLOR    
#undef NARRATE_BLACK
#undef NARRATE_INDIGO
#undef NARRATE_YELLOW    
    
    static unsigned lastTime = 0;
    
    if (state == 0)
    {
        titleshow.Fade(1.0f, 1/*2000*/);
        state = 1;
    }
    else if (state == 1)
    {
        if (titleshow.GetValue() >= 1.0f)
        {
            effects.Flash();
            titlefade.Fade(1.0f, 3000);
            titlescale.Fade(1.0f, 2500);
            state = 2;
            lastTime = GetAppTimeMs();
        }
    }
    else if (pages[state])
    {
        unsigned time = GetAppTimeMs();
        
        if (y < END)
            ui->DrawTextCentered(pages[state], Vector2(0.0f, y), 1.0f, 26.0f);
        else
        {
            y = START;
            state++;
        }
        float delta = (time - lastTime) * 0.035f;
        y += delta;
        if (fabsf(y) <= 2.0f)
        {
            // title fade out
            titlefade.Fade(0.0f, 2000);    
        }
        lastTime = time;
    }
    else
    {        
        // move on to the game
        this->state = GAME;
        effects.FadeBackgroundToGame();
        ShowMainMenu();        
    }
    
  //quads->RenderOverlay(title, Vector4(1.0f, 1.0f, 1.0f, titlefade.GetValue()), (1.0f - titlefade.GetValue()) * 0.2f + 0.8f);        
//  quads->RenderTitle(title, titlefade.GetValue(), (1.0f - titlescale.GetValue()) * 2.0f + 0.25f);    
    quads->RenderTitle(title, titlefade.GetValue(), (1.0f - titlescale.GetValue()) * 2.0f + 0.25f);            
}

void GameState::RenderEffects(const GameTime &time)
{
    quads->RenderBackground(background, effects.background.GetValue());
    if (state == INTRO)
    {
        Narration();
    }        
    quads->RenderOverlay(0, Vector4(0.0f, 0.0f, 0.0f, effects.black.GetValue()), 2.0f);    
    quads->RenderOverlay(0, Vector4(1.0f, 1.0f, 1.0f, effects.white.GetValue()), 2.0f);    
}

FrameRateTracker track("track", 40);

#include "core/Profiler.h"

extern Profiler profiler;

void GameState::Draw(const GameTime &time)
{
    total.EndFrame();
    total.StartFrame();
    
    profiler.StartTime("gs_draw");    
    
    static Material material;
    
    if (!anyRendered)
    {
        // any first frame set up right here
        //fader.Fade(1.0f, 3000);
        effects.UnDarken(3000);
        anyRendered = true;
    }

    camera->SetLookAt(actors.GetLookAt());
    services->device->SetProjection(camera->GetProjection());

    quads->RenderBackground(background, 1.0f);
    
    material.Set(*(services->device));        

    gamestatedraw.StartFrame();

    if (state == GAME)
    {
        // clear framebuffer here?
        profiler.StartTime("balls");
        actors.Draw(time);
        profiler.EndTime("balls");
         DrawList::GetDrawList()->Clear();    
        leveldrawlogic.StartFrame();
        track.StartFrame();
        profiler.StartTime("level_draw");        
        levels->Draw(time);
        profiler.EndTime("level_draw");        
        track.EndFrame();        
        leveldrawlogic.EndFrame();
        leveldrawsubmit.StartFrame();
        profiler.StartTime("dl_render");
        DrawList::GetDrawList()->Render(*(services->device), *camera);
        profiler.EndTime("dl_render");        
        leveldrawsubmit.EndFrame();
        actors.DrawShadow();
    }
    
    profiler.StartTime("messages_hud");        
    uidraw.StartFrame();
    DrawHud();
    ui->Draw();    
    profiler.EndTime("messages_hud");
    
    profiler.StartTime("fx_explosion_narration");        
    explosions->Render(*camera);
    RenderEffects(time);
    profiler.EndTime("fx_explosion_narration");    

    profiler.StartTime("ui_draw");    
    uicontroller.Draw(*ui);
    profiler.EndTime("ui_draw");

    if (state == INTRO)
    {
    //    Narration();
    }        
    
    for (std::vector<AABox>::const_iterator i = _debug_quads.begin(), e = _debug_quads.end(); i != e; ++i)
        _DebugRenderBoundsQuad(*i);
    _debug_quads.clear();
    
    uidraw.EndFrame();    
    
    gamestatedraw.EndFrame();
    
    profiler.EndTime("gs_draw");    
}

void GameState::DrawHud()
{
    if (uicontroller.IsAnyVisible())
        return;
    
    const char *color = "\\#ffffff";
    if (timeRemaining <= WARNING_TIME && !reachedExit && (int)(timeRemaining / 500) % 2 == 0)
        color = "\\#ff0000";

    int seconds = timeRemaining / 1000;
    
    char msg[2048];
    if (mode == MAZES_ONLY)
        sprintf(msg, "L%u", GetLevel()->GetLevelNumber());
    else
        sprintf(msg, "%s%02u:%02u\nL%u %u", color, seconds / 60, seconds % 60, GetLevel()->GetLevelNumber(), score);
    ui->DrawString(msg, Vector2(180.0f, 100.0f));

    /*
    sprintf(msg, "%u", GetLevelScore());    
    ui->DrawString(msg, Vector2(0.0f, 100.0f));    
    */
    
    if (challenge)
        ui->DrawString(challenge->GetMessage(), Vector2(-100.0f, 100.0f));                
}

void GameState::Event_OnPlayerKilled()
{
    reachedExit = false;
    //player->OnKilled();
    ui->SendBallMessage("\\#2800ff`You Died!");
}

unsigned GameState::GetLevelScore() const
{
    unsigned ls = levelScore + unsigned(timeRemaining / 1000.0f * POINTS_PER_SECOND);
    if (score2x) ls *= 2;
    if (score3x) ls *= 3;
    return ls;
}
            
void GameState::Event_OnExitReached()
{
    if (!reachedExit)
    {
        if (actors.GetPlayerIndex() != 0 || GetLevel()->IsLocked())
            return;
        
        reachedExit = true;
        effects.Flash();
        audio->Play("fanfare", 0.3f);
        score += GetLevelScore();
        int level = GetLevel()->GetLevelNumber();
        time_t timespan;
        time(&timespan);
        LevelScore tscore = LevelScore("<no name>", timespan, level, score),
                   lscore = LevelScore("<no name>", timespan, level, levelScore);
        if (mode == INDIGO_OCHO && level < 34)
            save.AutoSave(tscore);
        ui->SendBallMessage((mode == INDIGO_OCHO && scores.IsLevelHighScore(lscore)) ? 
                            "\\#ffff00`Level High Score!" : "\\#ffff00Level Complete");
        int rank = scores.GetHighScoreIndex(tscore);
        if (rank >= 0 && rank < 10)
            ui->SendBallMessage("\\#ffffff`New High Score!", 40);
        scoreContext.score.score = score;
        scoreContext.score.level = level;
        if (mode == INDIGO_OCHO && (level < 35 || strcmp(scoreContext.score.name, "<no name>") != 0)) // if they haven't entered a name til the end, they're out of luck, because the end of game stuff gets ruined by the name popup
            scoreContext.UpdateScores();
        if (mode == INDIGO_OCHO)
            SendPointsMessage(GetLevelScore(), 10);
        levelScore = 0;
        score2x = score3x = false;        
        Start();
    }
}
            
void GameState::Event_OnTimeExpired()
{
    if (mode == INDIGO_OCHO && !timeExpired)
    {
        ui->SendBallMessage("\\#ffffffOut of Time");        
        ui->SendBallMessage("\\#6600ffOut of Time");
        GameOver();
    }
}

void GameState::Event_OnTrapped()
{
    if (!timeExpired)
    {
        ui->SendBallMessage("\\#ff0000`TRAPPED!");        
        if (!trappedTutorial)
            GameOver();
        else
        {
            SetPaused(true);
            ShowTrappedTutorial();
            printf("TRAPPED!\n");
            //trappedTutorial = false; // go ahead and always show this
        }
    }
}

void GameState::GameOver()
{
    timeExpired      = true;
    reachedExit      = false;
    //waitingForPlayer = true;
    uicontroller.ClearStack();
    uicontroller.SetVisible(UIController::GAME_OVER);    
}
        
void GameState::Event_OnSwirlEnd(IAnimator *animator)
{
    StaticModel::GlobalAnimator = 0;
}
            
void GameState::Event_OnSwirl()
{
    const int SwirlPoints = 100;

    audio->Play("pickup");
    TimedAnimator *animator = new TimedAnimator(new RotateAnimator(Vector3(0.0f, 0.0f, 0.1f)), 1500, false);
//    animator.SetCompleteEvent(new AnimationComplete(Event_OnSwirlEnd));
    if (StaticModel::GlobalAnimator)
        delete StaticModel::GlobalAnimator;
    StaticModel::GlobalAnimator = animator;
    ClearPickup();
    levelScore += SwirlPoints;
    if (mode == INDIGO_OCHO)    
        SendPointsMessage(SwirlPoints, 0);
    else
        ui->SendBallMessage("\\#808080`\\#ffffff!");
}
            
void GameState::Event_OnPickup2x()
{
    if (!score2x)
    {
        score2x = true;
        if (mode == INDIGO_OCHO)        
            ui->SendBallMessage("\\#808080x\\#ffffff2");
        else
            ui->SendBallMessage("\\#808080`\\#ffffff!");
        audio->Play("pickup");    
        ClearPickup();
    }
}
            
void GameState::Event_OnPickup3x()
{
    if (!score3x)
    {
        score3x = true;
        if (mode == INDIGO_OCHO)
            ui->SendBallMessage("\\#808080x\\#ffffff3");
        else
            ui->SendBallMessage("\\#808080`\\#ffffff!");
        audio->Play("pickup");        
        ClearPickup();
    }
}
