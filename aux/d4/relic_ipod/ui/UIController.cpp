#include "UIController.h"

#include "core/global.h"

// $BAD_DEPENDENCYs!!
#include "game/GameState.h"
#include "level/Level.h"

#include "render/GameEffects.h"
#include "render/OverheadCamera.h"

#include "platform/GLUtils.h"
#include "platform/GLIncludes.h"

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
        UIController::GetInstance()->SetVisible(UIController::NO_SCREEN);
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
        UIController::GetInstance()->SetVisible(UIController::NO_SCREEN/*UIController::ENTER_HIGH_SCORE*/);
    }
    GetTextFromKeyboard(message, HighScoreNameCallback, cl);
}

void MenuItem_ResumePausedGame(const MenuItem &item)
{
    UIContext *context = (UIContext *)item.context;
    GameState *state   = (GameState *)context->context;
    context->controller->SetVisible(UIController::NO_SCREEN);    
}

void MenuItem_MainMenu(const MenuItem &item)
{
    UIContext *context = (UIContext *)item.context;
    // ..
    context->controller->ClearStack();
    context->controller->SetVisible(UIController::MAIN_MENU);
}

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

void Screen_Exit_GameOver(void *_context)
{
    UIContext *context = (UIContext *)_context;
    GameState *state   = (GameState *)context->context;    
    context->controller->Pop();
    state->Notify("GameOver");
}

static void Add(Menu *menu, UIContext &context, const char *text, MenuItem::function_t function, bool animate = true)
{
    if (text && *text)
    {
        MenuItem item;
        item.text     = text;
        item.function = function;
        item.context  = &context;
        item.animate  = animate;
        menu->Add(item);
    }
}

static void Add(IScreen *screen, UIContext &context, const char *text, MenuItem::function_t function, bool animate = true)
{
    Add((Menu *)screen, context, text, function, animate);
}

void UIController::SetVisible(Screen screen)
{
    bool anyvisible = IsAnyVisible();
    if      ( anyvisible && screen == NO_SCREEN) UnDarken(context);
    else if (!anyvisible && screen != NO_SCREEN) Darken(context);
    for (int i = (int)NO_SCREEN; i < MAX_SCREENS; i++)
    {
        if (screens[i])
        {
            screens[i]->SetVisible(screen == i); 
            if (screen == i)
                screens[i]->Reset();
        }
    }
    if (screen == NO_SCREEN)
        ClearStack();
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
//  case UIController::EXIT_APPSTORE:     f = Screen_Exit_AppStore; break;
//  case UIController::EXIT_END_GAME:     f = Screen_Exit_EndGame;  break;
    }
    screen->SetExitFunction(f, &context);    
    screens[GENERAL_TEXT] = screen;
    SetVisible(GENERAL_TEXT);
}

// FOR LUA INTEGRATION START

Menu *CreateMenu(const char *name, const char *title, float textSize)
{
    Menu *menu = new Menu(title, static_cast<int>(textSize));
    UIController::GetInstance()->AddScreen(name, menu);
    return menu;
}

void LuaUICallback(const MenuItem &item)
{
    // $TODO call into lua with menu information
}

void Menu_AddItem(Menu *menu, const char *text, const char *luaFunctionName, bool animate)
{
    UIContext &context = UIController::GetInstance()->context;
    MenuItem item;
    item.text        = (!text || !*text) ? "" : text;
    item.function    = LuaUICallback;
    item.context     = &context;
    item.animate     = animate;
    item.luaFunction = luaFunctionName;
    menu->Add(item);
}

// FOR LUA INTEGRATION END

UIController *UIController::mInstance = 0;

UIController::UIController(void *usercontext) : context(this, usercontext)
{
    memset(screens, 0, sizeof(screens));
    
    GameState   *state       = (GameState *)usercontext;    
//  Leaderboard *leaderboard = new Leaderboard  ("\\#6600ffMy High Scores",
//                                               "\\#6600ffMy Level High Scores", state->scores);    
#ifdef LITE_VERSION
    screens[MAIN_MENU]          = new Menu      ("\\#ff0000I\\#ffff00n\\#00ff00d\\#0000ffi\\#ffff00g\\#ff0000o \\#00ff00O\\#ff0000c\\#ffff00h\\#0000ffo \\#ffffffLite", 30);
#else
    screens[MAIN_MENU]          = new Menu      ("\\#ff0000I\\#ffff00n\\#00ff00d\\#0000ffi\\#ffff00g\\#ff0000o \\#00ff00O\\#ff0000c\\#ffff00h\\#0000ffo", 30);
#endif
    screens[PAUSE_MENU]         = new Menu      ("\\#6600ffGame Paused",       30);
    
    Add(screens[PAUSE_MENU],   context, "Resume",                       MenuItem_ResumePausedGame);
//  Add(screens[PAUSE_MENU],   context, "\\#808080< Quit",              MenuItem_MainMenu);
}

UIController::~UIController()
{
    for (int i = (int)NO_SCREEN; i < MAX_SCREENS; i++)
        if (screens[i])
            delete screens[i];
    memset(screens, 0, sizeof(screens));
}

void UIController::Draw(RenderContext &context, GameUI &ui)
{
    for (int i = (int)NO_SCREEN; i < MAX_SCREENS; i++)
        if (screens[i])
            screens[i]->Set(context, ui);
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

void UIController::AddScreen(const char *name, IScreen *screen)
{
    mScreens[name] = screen;
}