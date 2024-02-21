#ifndef _UICONTROLLER_H
#define _UICONTROLLER_H

#include "core/core.h"
#include "Menu.h"

#include <map>
#include <string>

class OverheadCamera;
class GameUI;
class RenderContext;
class UIController;

class UIContext
{
public:
    UIController *controller;
    void         *context;
        
    inline UIContext(UIController *controller, void *context) : controller(controller), context(context) {}
};

#include <vector>

class UIController
{
public:
    
    enum Screen { NO_SCREEN, GENERAL_TEXT, MAIN_MENU, PAUSE_MENU, MAX_SCREENS };
    enum ExitTextOption { EXIT_RETURN_GAME, EXIT_GAME_OVER, EXIT_END_GAME, EXIT_APPSTORE };
    
    IScreen             *screens[MAX_SCREENS];
    UIContext            context;
    
    std::vector<Screen>  stack;
    
    inline void Push()                 { stack.push_back(GetVisible()); }
    inline void Pop()                  { SetVisible(PopTop()); } 
    inline void ClearStack()           { stack.clear(); }
       
    void   Draw(RenderContext &context, GameUI &ui);
    bool   HandleTouch(const core::Point &point, const OverheadCamera &camera);
    void   SetVisible(Screen screen);
    Screen GetVisible() const;
    bool   IsVisible(Screen screen) const;
    bool   IsAnyVisible() const;

    void        AddScreen(const char *name, IScreen *screen);
    void        SetVisibleScreen(const char *name);
    const char *GetVisibleScreen();
    bool        IsVisibleScreen(const char *name) const;
    
    void   ShowText(const char *title, const char **text, ExitTextOption option = EXIT_RETURN_GAME);

    static void          CreateInstance(void *context) { if (!mInstance) mInstance = new UIController(context); }
    static UIController *GetInstance()                 { return mInstance; }
    static void          DestroyInstance()             { if (mInstance) delete mInstance; mInstance = 0; }
    
private:

    std::map<std::string, IScreen *> mScreens;

    UIController(void *context);
    ~UIController();
    
    static UIController *mInstance;

    inline Screen PopTop()  { if (stack.empty()) return NO_SCREEN; Screen s = stack.back(); stack.pop_back(); return s; }
};

#endif // _UICONTROLLER_H