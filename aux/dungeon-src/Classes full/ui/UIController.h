/*
 *  UIController.h
 *  GLGravity
 *
 *  Created by Paul Senzee on 1/10/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */
#ifndef _UICONTROLLER_H
#define _UICONTROLLER_H

#include "core/xna.h"
#include "Menu.h"

class OverheadCamera;
class GameUI;

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
    
    enum Screen { NO_SCREEN, GENERAL_TEXT, MAIN_MENU, PAUSE_MENU, MAZE_PAUSE_MENU,
                  NEW_GAME_MENU, LOAD_GAME_MENU, NEW_LOAD_MENU, SAVE_GAME_MENU, SCORE_MENU, LEVEL_SELECT,
                  /*ABANDON_PROGRESS, */GAME_OVER, ENTER_HIGH_SCORE, LOCAL_LEADERBOARD, WORLD_LEADERBOARD, DEBUG_MENU, MAX_SCREENS };
    enum ExitTextOption { EXIT_RETURN_GAME, EXIT_GAME_OVER, EXIT_END_GAME };
    
    IScreen             *screens[MAX_SCREENS];
    UIContext            context;
    
    std::vector<Screen>  stack;
    
    inline void Push()                 { stack.push_back(GetVisible()); }
    inline void Pop()                  { SetVisible(PopTop()); } 
    inline void ClearStack()           { stack.clear(); }
    
    UIController(void *context);
    ~UIController();
    
    void   Draw(GameUI &ui);
    bool   HandleTouch(const core::Point &point, const OverheadCamera &camera);
    void   SetVisible(Screen screen);
    Screen GetVisible() const;
    bool   IsVisible(Screen screen) const;
    bool   IsAnyVisible() const;
    
    void   ShowText(const char *title, const char **text, ExitTextOption option = EXIT_RETURN_GAME);
      
    void   UpdateLoadGameMenu();
    void   UpdateSaveGameMenu();    
    void   UpdateDebugMenu();
    
private:
    
    inline Screen PopTop()  { if (stack.empty()) return NO_SCREEN; Screen s = stack.back(); stack.pop_back(); return s; }
};

#endif // _UICONTROLLER_H