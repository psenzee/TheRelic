/*
 *  UIController.cpp
 *  GLGravity
 *
 *  Created by Paul Senzee on 1/10/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#include "UIController.h"

#include "core/global.h"

// $BAD_DEPENDENCYs!!
#include "game/GameState.h"
#include "game/Level.h"
#include "game/Player.h"
#include "game/saves/SaveGame.h"
#include "game/challenges/MutualDestruction.h"

#include "render/GameEffects.h"
#include "render/OverheadCamera.h"

void Darken(UIContext &context)
{
    GameState *state = (GameState *)context.context;
    state->effects.Darken();
}

void UnDarken(UIContext &context)
{
    GameState *state = (GameState *)context.context;
    state->effects.UnDarken();
}

void MenuItem_Pause(const MenuItem &item)
{
    UIContext *context = (UIContext *)item.context;
    context->controller->ClearStack();    
    context->controller->SetVisible(UIController::PAUSE_MENU);
}

void MenuItem_NewGame(const MenuItem &item)
{
    UIContext *context = (UIContext *)item.context;
    GameState *state   = (GameState *)context->context;
    if (state) state->NewActiveGame();
    context->controller->SetVisible(UIController::NO_SCREEN);
}

void MenuItem_Play(const MenuItem &item)
{
    UIContext *context = (UIContext *)item.context;
    GameState *state   = (GameState *)context->context;
    if (state->mode != GameState::INDIGO_OCHO)
    {
        state->mode = GameState::INDIGO_OCHO;
        if (state)
            state->SavedStart();
    }
    context->controller->SetVisible(UIController::NO_SCREEN);
}

void MenuItem_MazesOnly(const MenuItem &item)
{
    UIContext *context = (UIContext *)item.context;
    GameState *state   = (GameState *)context->context;
    state->mode = GameState::MAZES_ONLY;
    if (state) state->NewMazeOnlyGame();    
    context->controller->SetVisible(UIController::NO_SCREEN);
}

void MenuItem_RestartLevel(const MenuItem &item)
{
    UIContext *context = (UIContext *)item.context;
    GameState *state   = (GameState *)context->context;
    context->controller->SetVisible(UIController::NO_SCREEN);    
    state->Restart();    
}

void MenuItem_ResumeAutoSaveGame(const MenuItem &item)
{
    UIContext *context = (UIContext *)item.context;
    GameState *state   = (GameState *)context->context;
    if (state) state->SavedStart();
    context->controller->SetVisible(UIController::NO_SCREEN);
}

void MenuItem_NextLevel(const MenuItem &item)
{
    UIContext *context = (UIContext *)item.context;
    GameState *state   = (GameState *)context->context;
    if (state) state->Start();
    context->controller->SetVisible(UIController::NO_SCREEN);
}

void MenuItem_PreviousLevel(const MenuItem &item)
{
    UIContext *context = (UIContext *)item.context;
    GameState *state   = (GameState *)context->context;
    if (state)
    {
        int level = state->GetLevel()->GetLevelNumber();
        if (level > 1)
            state->Start(level - 1);
    }
    context->controller->SetVisible(UIController::NO_SCREEN);
}

typedef void (*text_callback)(const char *text, void *context);
extern "C" void GetTextFromKeyboard(const char *message, text_callback cb, void *context);

struct closure_t
{
    text_callback  callback;
    void          *context;
};

extern GameState *state;

void HighScoreNameCallback(const char *message, void *context)
{
    closure_t *cl = (closure_t *)context;
    if (cl)
        (cl->callback)(message, cl->context);
    if (state)
    {
        state->SetPaused(false);
        state->uicontroller.SetVisible(UIController::NO_SCREEN);        
    }
}

void GetHighScoreName(const char *message, text_callback cb, void *context)
{
    closure_t *cl = new closure_t;
    cl->callback = cb;
    cl->context  = context;
    if (state)
    {
        state->SetPaused(true);
        state->uicontroller.SetVisible(UIController::ENTER_HIGH_SCORE);
    }
    GetTextFromKeyboard(message, HighScoreNameCallback, cl);
}

void MenuItem_NewLoadGame(const MenuItem &item)
{
    UIContext *context = (UIContext *)item.context;
    context->controller->Push();
    context->controller->SetVisible(UIController::NEW_LOAD_MENU);
}

void MenuItem_LoadGame(const MenuItem &item)
{
    UIContext *context = (UIContext *)item.context;
    context->controller->Push();    
    context->controller->UpdateLoadGameMenu();
    context->controller->SetVisible(UIController::LOAD_GAME_MENU);        
}

void MenuItem_SaveGame(const MenuItem &item)
{
    UIContext *context = (UIContext *)item.context;
    context->controller->Push();    
    context->controller->UpdateSaveGameMenu();
    context->controller->SetVisible(UIController::SAVE_GAME_MENU);
}

void MenuItem_HighScores(const MenuItem &item)
{
    UIContext *context = (UIContext *)item.context;
    context->controller->Push();
    context->controller->SetVisible(UIController::SCORE_MENU);        
}

void MenuItem_ResumePausedGame(const MenuItem &item)
{
    UIContext *context = (UIContext *)item.context;
    GameState *state   = (GameState *)context->context;
    context->controller->SetVisible(UIController::NO_SCREEN);    
}

void MenuItem_ResumeFailedGame(const MenuItem &item)
{
    UIContext *context = (UIContext *)item.context;
    GameState *state   = (GameState *)context->context;
    context->controller->SetVisible(UIController::NO_SCREEN);
    state->Restart();
}

void MenuItem_Settings(const MenuItem &item)
{
    UIContext *context = (UIContext *)item.context;
    // ..
    context->controller->SetVisible(UIController::NO_SCREEN);
}

extern "C" void appStoreLink();

void MenuItem_GetFullVersion(const MenuItem &item)
{
    UIContext *context = (UIContext *)item.context;
    // ..
    context->controller->SetVisible(UIController::NO_SCREEN);
    appStoreLink();
}

void MenuItem_MainMenu(const MenuItem &item)
{
    UIContext *context = (UIContext *)item.context;
    // ..
    context->controller->ClearStack();
    context->controller->SetVisible(UIController::MAIN_MENU);
}

typedef void (*GameSlot_fn_t)(GameState *state, int index);

void GameSlot_Save(GameState *state, int index) { state->SaveActiveGame(index); }
void GameSlot_Load(GameState *state, int index) { state->LoadActiveGame(index); }

void MenuItem_GameSlot(const MenuItem &item, GameSlot_fn_t function)
{
    UIContext *context = (UIContext *)item.context;
    GameState *state   = (GameState *)context->context;
    
    std::vector<SaveGameInfo> info;
    SaveGame::LoadGameInfo(info);
    SaveGame::PrintGameInfo(info);
    if (!info.empty())
    {
        char name[128];
        for (std::vector<SaveGameInfo>::iterator i = info.begin(), e = info.end(); i != e; ++i)
        {
            (*i).data.GetDisplayName(name);
            if (strcmp(item.text.c_str(), name) == 0)
            {
                function(state, (*i).index);
                break;
            }
        }
    }
    
    context->controller->SetVisible(UIController::NO_SCREEN);
}

void MenuItem_SelectNewSave(const MenuItem &item)
{
    UIContext *context = (UIContext *)item.context;
    GameState *state   = (GameState *)context->context;
    
    std::vector<SaveGameInfo> info;
    SaveGame::LoadGameInfo(info);
    SaveGame::PrintGameInfo(info);
    int max = -1;
    for (std::vector<SaveGameInfo>::iterator i = info.begin(), e = info.end(); i != e; ++i)
        if ((*i).index > max)
            max = (*i).index;
    state->SaveActiveGame(max + 1);
    context->controller->SetVisible(UIController::NO_SCREEN);
}

void MenuItem_LoadGameSlot(const MenuItem &item) { MenuItem_GameSlot(item, GameSlot_Load); }
void MenuItem_SaveGameSlot(const MenuItem &item) { MenuItem_GameSlot(item, GameSlot_Save); }

void MenuItem_SelectLevel(const MenuItem &item)
{
    UIContext *context = (UIContext *)item.context;
    context->controller->Push();
    context->controller->SetVisible(UIController::LEVEL_SELECT);    
}

void MenuItem_LocalHighScores(const MenuItem &item)
{
    UIContext *context = (UIContext *)item.context;
    context->controller->Push();
    context->controller->SetVisible(UIController::LOCAL_LEADERBOARD);    
}

void MenuItem_WorldHighScores(const MenuItem &item)
{
    UIContext *context = (UIContext *)item.context;
    context->controller->Push();
    context->controller->SetVisible(UIController::WORLD_LEADERBOARD);    
}

void MenuItem_MoreGames(const MenuItem &item)
{
    UIContext *context = (UIContext *)item.context;
    context->controller->Push();
    context->controller->SetVisible(UIController::MORE_GAMES);    
}

void MenuItem_DebugMenu(const MenuItem &item)
{
    UIContext *context = (UIContext *)item.context;
    context->controller->Push();
    context->controller->SetVisible(UIController::DEBUG_MENU);    
}
/*
void MenuItem_AbandonProgress(const MenuItem &item)
{
    UIContext *context = (UIContext *)item.context;
    context->controller->Push();
    context->controller->SetVisible(UIController::ABANDON_PROGRESS);
}
*/
void MenuItem_Return(const MenuItem &item)
{
    UIContext *context = (UIContext *)item.context;
    context->controller->Pop();
}

void PlayUISound(void *_context)
{
    UIContext *context = (UIContext *)_context;
    GameState *state   = (GameState *)context->context;    
    state->PlayUISound();
}

void Screen_Exit(void *_context)
{
    UIContext *context = (UIContext *)_context;
    GameState *state   = (GameState *)context->context;    
    context->controller->Pop();
    if (!context->controller->IsAnyVisible())
        state->SetPaused(false);
}

void Screen_Exit_EndGame(void *_context)
{
    UIContext *context = (UIContext *)_context;
    GameState *state   = (GameState *)context->context;    
    context->controller->Pop();
    state->effects.FadeBackgroundToGame(2000);    
    state->NewActiveGame();
    context->controller->SetVisible(UIController::MAIN_MENU);
}

void Screen_Exit_GameOver(void *_context)
{
    UIContext *context = (UIContext *)_context;
    GameState *state   = (GameState *)context->context;    
    context->controller->Pop();
    state->GameOver();
}

void Screen_Exit_AppStore(void *_context)
{
    UIContext *context = (UIContext *)_context;
    GameState *state   = (GameState *)context->context;    
    context->controller->Pop();
    appStoreLink();
}

void MenuItem_DebugCompleteLevel(const MenuItem &item)
{
    UIContext *context = (UIContext *)item.context;
    GameState *state   = (GameState *)context->context;
    context->controller->SetVisible(UIController::NO_SCREEN);    
    if (state)
    {
        state->actors.SetPlayerIndex(0);
        state->GetLevel()->UnlockLevelExit();
        state->Event_OnExitReached();
    }
}

void MenuItem_DebugExplode(const MenuItem &item)
{
    UIContext *context = (UIContext *)item.context;
    GameState *state   = (GameState *)context->context;
    if (state)
        state->Explode(state->camera->GetLookAt(), 64.0f);
    context->controller->SetVisible(UIController::NO_SCREEN);
}

void MenuItem_DebugMinusBallDirection(const MenuItem &item)
{
    UIContext *context = (UIContext *)item.context;
    GameState *state   = (GameState *)context->context;
    MutualDestruction::FLIP_MINUS_DIRECTION ^= true;
    context->controller->SetVisible(UIController::NO_SCREEN);
}


void MenuItem_DebugClearHighScores(const MenuItem &item)
{
    UIContext *context = (UIContext *)item.context;
    GameState *state   = (GameState *)context->context;
    state->scores.Clear();
    context->controller->SetVisible(UIController::NO_SCREEN);
}

extern float _g_inputSensitivity;

const float SENSITIVITY_DELTA = 1.0f;

void MenuItem_DebugSensitivityDown(const MenuItem &item)
{
    UIContext *context = (UIContext *)item.context;
    _g_inputSensitivity -= SENSITIVITY_DELTA;
    if (_g_inputSensitivity == 0.0f)
        _g_inputSensitivity = -SENSITIVITY_DELTA;        
    context->controller->UpdateDebugMenu();
}

void MenuItem_DebugSensitivityUp(const MenuItem &item)
{
    UIContext *context = (UIContext *)item.context;
    _g_inputSensitivity += SENSITIVITY_DELTA;
    if (_g_inputSensitivity == 0.0f)
        _g_inputSensitivity = SENSITIVITY_DELTA;
    context->controller->UpdateDebugMenu();    
}

static void Add(Menu *menu, UIContext &context, const char *text, MenuItem::function_t function)
{
    if (text && *text)
    {
        MenuItem item;
        item.text     = text;
        item.function = function;
        item.context  = &context;
        menu->Add(item);
    }
}

static void Add(IScreen *screen, UIContext &context, const char *text, MenuItem::function_t function)
{
    Add((Menu *)screen, context, text, function);
}

void UIController::SetVisible(Screen screen)
{
    bool anyvisible = IsAnyVisible();
    if      ( anyvisible && screen == NO_SCREEN) UnDarken(context);
    else if (!anyvisible && screen != NO_SCREEN) Darken(context);
    for (int i = (int)NO_SCREEN; i < MAX_SCREENS; i++)
        if (screens[i])
            screens[i]->SetVisible(screen == i); 
    if (screen == NO_SCREEN)
        ClearStack();
}

void UIController::UpdateLoadGameMenu()
{
    if (screens[LOAD_GAME_MENU])
        delete screens[LOAD_GAME_MENU];
    
    screens[LOAD_GAME_MENU] = new Menu("\\#6000c0Load Game", 30);    
    
    std::vector<SaveGameInfo> info;
    SaveGame::LoadGameInfo(info);
    SaveGame::PrintGameInfo(info);
    char name[1024];    
    if (info.size() == 0)
        Add(screens[LOAD_GAME_MENU], context, "\\#808080No Saved Games",  0);
    else
    {
        for (std::vector<SaveGameInfo>::iterator i = info.begin(), e = info.end(); i != e; ++i)
        {
            (*i).data.GetDisplayName(name);        
            Add(screens[LOAD_GAME_MENU], context, name,  MenuItem_LoadGameSlot);
        }
    }
    Add(screens[LOAD_GAME_MENU], context, "\\#808080< Return", MenuItem_Return);    
}

void UIController::UpdateSaveGameMenu()
{
    if (screens[SAVE_GAME_MENU])
        delete screens[SAVE_GAME_MENU];
    
    screens[SAVE_GAME_MENU] = new Menu("\\#6600ffSave Game", 30);    
    
    std::vector<SaveGameInfo> info;
    SaveGame::LoadGameInfo(info);
    SaveGame::PrintGameInfo(info);
    char name[1024];    
    if (info.size() < 5)
        Add(screens[SAVE_GAME_MENU], context, "New Save", MenuItem_SelectNewSave);
    for (std::vector<SaveGameInfo>::iterator i = info.begin(), e = info.end(); i != e; ++i)
    {
        (*i).data.GetDisplayName(name);        
        Add(screens[SAVE_GAME_MENU], context, name,  MenuItem_SaveGameSlot);
    }
    Add(screens[SAVE_GAME_MENU], context, "\\#808080< Return", MenuItem_Return);
}

void UIController::UpdateDebugMenu()
{
    Menu *menu = (Menu *)screens[DEBUG_MENU];
    char message[1024];
    sprintf(message, "Sensitivity [%.1f]", _g_inputSensitivity);
    menu->RemoveLast();
    Add(menu, context, message, 0);
}

void UIController::ShowText(const char *title, const char **text, ExitTextOption option)
{
    TextScreen  *screen = new TextScreen(title, text);
    if (screens[GENERAL_TEXT])
        delete screens[GENERAL_TEXT];
    TextScreen::function_t f = 0;
    switch (option)
    {
    default:
    case UIController::EXIT_RETURN_GAME:  f = Screen_Exit;          break;
    case UIController::EXIT_GAME_OVER:    f = Screen_Exit_GameOver; break;
    case UIController::EXIT_APPSTORE:     f = Screen_Exit_AppStore; break;            
    case UIController::EXIT_END_GAME:     f = Screen_Exit_EndGame;  break;
    }
    screen->SetExitFunction(f, &context);    
    screens[GENERAL_TEXT] = screen;
    SetVisible(GENERAL_TEXT);
}

UIController::UIController(void *usercontext) : context(this, usercontext)
{
    memset(screens, 0, sizeof(screens));
    
    GameState   *state       = (GameState *)usercontext;    
    Leaderboard *leaderboard = new Leaderboard  ("\\#6600ffMy High Scores",
                                                 "\\#6600ffMy Level High Scores", state->scores);    
#ifdef LITE_VERSION
    screens[MAIN_MENU]          = new Menu      ("\\#ff0000I\\#ffff00n\\#00ff00d\\#0000ffi\\#ffff00g\\#ff0000o \\#00ff00O\\#ff0000c\\#ffff00h\\#0000ffo \\#ffffffLite", 30);
#else
    screens[MAIN_MENU]          = new Menu      ("\\#ff0000I\\#ffff00n\\#00ff00d\\#0000ffi\\#ffff00g\\#ff0000o \\#00ff00O\\#ff0000c\\#ffff00h\\#0000ffo", 30);
#endif
    screens[PAUSE_MENU]         = new Menu      ("\\#6600ffGame Paused",       30);
    screens[MAZE_PAUSE_MENU]    = new Menu      ("\\#6600ffGame Paused",       30);    
    screens[SCORE_MENU]         = new Menu      ("\\#6600ffHigh Scores",       30);
    screens[GAME_OVER]          = new Menu      ("\\#6600ffGame Over",         30);
    screens[LEVEL_SELECT]       = new Menu      ("\\#6600ffSelect Level",      30);
    screens[NEW_LOAD_MENU]      = new Menu      ("\\#6600ffNew/Load Game",     30);
    screens[WORLD_LEADERBOARD]  = new Menu      ("\\#6600ffWorld High Scores", 30);
//  screens[ABANDON_PROGRESS]   = new Menu      ("\\#6600ffAbandon Progress?", 30);
    screens[ENTER_HIGH_SCORE]   = new Menu      ("\\#6600ffNew High Score!",   30);

    screens[MORE_GAMES]         = new Menu      ("\\#6600ffMore Games!",       30);    
    
    screens[LOCAL_LEADERBOARD]  = leaderboard;
    
    screens[GENERAL_TEXT]       = 0;
    
    screens[DEBUG_MENU]         = new Menu      ("\\#808080DEBUG",             30);
    
    leaderboard->SetExitFunction(Screen_Exit, &context);
    
    Add(screens[MAIN_MENU],         context, "\\#6600ffPlay!",               MenuItem_Play);
#ifndef LITE_VERSION    
    Add(screens[MAIN_MENU],         context, "\\#8080ffRun the Gauntlet!",   MenuItem_MazesOnly);
    Add(screens[MAIN_MENU],         context, "\\#c0c0c0New/Load Game",       MenuItem_NewLoadGame);
    Add(screens[MAIN_MENU],         context, "\\#c0c0c0High Scores",         MenuItem_LocalHighScores);    
#else
    Add(screens[MAIN_MENU],         context, "\\#ffff00More Games!",         MenuItem_MoreGames);
#endif
    
//  Add(screens[ABANDON_PROGRESS],  context, "Yes",                          MenuItem_MainMenu);
//  Add(screens[ABANDON_PROGRESS],  context, "No",                           MenuItem_Return);    
    
    Add(screens[PAUSE_MENU],        context, "Resume",                       MenuItem_ResumePausedGame);
   Add(screens[PAUSE_MENU],        context, "Restart Level",                 MenuItem_RestartLevel);
#ifndef LITE_VERSION
    Add(screens[PAUSE_MENU],        context, "Save Game",                    MenuItem_SaveGame);
//  Add(screens[PAUSE_MENU],        context, "High Scores",                  MenuItem_HighScores);
    Add(screens[PAUSE_MENU],        context, "High Scores",                  MenuItem_LocalHighScores);
#else
    Add(screens[PAUSE_MENU],        context, "\\#ffff00More Games!",         MenuItem_MoreGames);
#endif    

#ifdef _DEVELOPMENT_    
    Add(screens[PAUSE_MENU],        context, "\\#808080[Select Level]",      MenuItem_SelectLevel);
#endif
    Add(screens[PAUSE_MENU],        context, "\\#808080< Quit",              MenuItem_MainMenu);
    
    Add(screens[MORE_GAMES],        context, "\\#808080Indigo Ocho Lite",    0);
   Add(screens[MORE_GAMES],        context, "\\#ffffffIndigo Ocho",          MenuItem_GetFullVersion);
    Add(screens[MORE_GAMES],        context, "\\#808080More Coming Soon!",   0);
    Add(screens[MORE_GAMES],        context, "\\#ffffff< Return",            MenuItem_Return);
    
    Add(screens[MAZE_PAUSE_MENU],   context, "Resume",                       MenuItem_ResumePausedGame);
#ifdef _DEVELOPMENT_    
    Add(screens[MAZE_PAUSE_MENU],   context, "\\#808080[Select Level]",      MenuItem_SelectLevel);
#endif
//  Add(screens[MAZE_PAUSE_MENU],   context, "High Scores",                  MenuItem_HighScores);
    Add(screens[MAZE_PAUSE_MENU],   context, "High Scores",                  MenuItem_LocalHighScores);
    Add(screens[MAZE_PAUSE_MENU],   context, "\\#808080< Quit",              MenuItem_MainMenu);
    
    Add(screens[LEVEL_SELECT],      context, "Next Level",                   MenuItem_NextLevel);
    Add(screens[LEVEL_SELECT],      context, "Previous Level",               MenuItem_PreviousLevel);
    Add(screens[LEVEL_SELECT],      context, "\\#808080< Return",            MenuItem_Return);
    Add(screens[LEVEL_SELECT],      context, "",                             0);
    Add(screens[LEVEL_SELECT],      context, "\\#808080[DEBUG MENU]",        MenuItem_DebugMenu);    
    
    Add(screens[DEBUG_MENU],        context, "\\#808080Complete Level",       MenuItem_DebugCompleteLevel);    
    Add(screens[DEBUG_MENU],        context, "\\#808080Clear High Scores",    MenuItem_DebugClearHighScores);        
//  Add(screens[DEBUG_MENU],        context, "\\#808080Explode",              MenuItem_DebugExplode);
//  Add(screens[DEBUG_MENU],        context, "\\#808080Anti-Balls",           MenuItem_DebugAntiBalls);
//  Add(screens[DEBUG_MENU],        context, "\\#808080Minus Ball Direction", MenuItem_DebugMinusBallDirection);
    Add(screens[DEBUG_MENU],        context, "\\#808080Sensitivity -",        MenuItem_DebugSensitivityUp);
    Add(screens[DEBUG_MENU],        context, "\\#808080Sensitivity +",        MenuItem_DebugSensitivityDown);
    Add(screens[DEBUG_MENU],        context, "\\#808080< Return",                      MenuItem_Return);    
    Add(screens[DEBUG_MENU],        context, "$placeholder",                  0);    
    
    UpdateDebugMenu();
    
    Add(screens[SCORE_MENU],        context, "My High Scores",            MenuItem_LocalHighScores);
    Add(screens[SCORE_MENU],        context, "World High Scores",         MenuItem_WorldHighScores);
    Add(screens[SCORE_MENU],        context, "\\#808080< Return",                  MenuItem_Return);
    
    Add(screens[GAME_OVER],         context, "Retry",                     MenuItem_ResumeFailedGame);
    Add(screens[GAME_OVER],         context, "\\#808080< Quit",                    MenuItem_MainMenu/*MenuItem_AbandonProgress*/);
    
    Add(screens[NEW_LOAD_MENU],     context, "New Game",                  MenuItem_NewGame);
    Add(screens[NEW_LOAD_MENU],     context, "Load Game",                 MenuItem_LoadGame);
    Add(screens[NEW_LOAD_MENU],     context, "\\#808080< Return",                  MenuItem_Return);
    
    Add(screens[WORLD_LEADERBOARD], context, "\\#808080Online World High Scores", 0);
    Add(screens[WORLD_LEADERBOARD], context, "\\#808080Coming Soon!",             0);
    Add(screens[WORLD_LEADERBOARD], context, "\\#808080Check for Updates",        0);
    Add(screens[WORLD_LEADERBOARD], context, " ",                                 0);
    Add(screens[WORLD_LEADERBOARD], context, "\\#808080< Return",                  MenuItem_Return);    
}

UIController::~UIController()
{
    for (int i = (int)NO_SCREEN; i < MAX_SCREENS; i++)
        if (screens[i])
            delete screens[i];
    memset(screens, 0, sizeof(screens));
}

void UIController::Draw(GameUI &ui)
{
    for (int i = (int)NO_SCREEN; i < MAX_SCREENS; i++)
        if (screens[i])
            screens[i]->Set(ui);
}

bool UIController::IsVisible(Screen type) const
{
    return screens[type] && screens[type]->IsVisible();
}

bool UIController::IsAnyVisible() const
{
    for (int i = (int)NO_SCREEN; i < MAX_SCREENS; i++)
        if (screens[i] && screens[i]->IsVisible())
            return true;
    return false;
}

UIController::Screen UIController::GetVisible() const
{
    for (int i = (int)NO_SCREEN; i < MAX_SCREENS; i++)
        if (screens[i] && screens[i]->IsVisible())
            return (Screen)i;
    return NO_SCREEN;
}

bool UIController::HandleTouch(const core::Point &point, const OverheadCamera &camera)
{
    for (int i = (int)NO_SCREEN; i < MAX_SCREENS; i++)
    {
        if (screens[i] && screens[i]->HandleTouch(point, camera))
        {
            PlayUISound(&context);
            return true;
        }
    }
    return false;
}