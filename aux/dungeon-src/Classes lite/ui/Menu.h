/*
 *  Menu.h
 *  GLGravity
 *
 *  Created by Paul Senzee on 1/9/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _MENU_H
#define _MENU_H

#include <string>
#include <vector>

#include "core/aabox.h"
#include "core/xna.h"

#include "time/GameClock.h"
#include "game/saves/HighScores.h"

class OverheadCamera;
class GameUI;
class GameInput;

class IScreen
{
public:
        
    virtual ~IScreen() {}
        
    virtual void Set(GameUI &ui)                                                           = 0;
    virtual bool HandleTouch(const core::Point &point, const OverheadCamera &camera)       = 0;
    virtual bool IsVisible()                                                         const = 0;
    virtual void SetVisible(bool v)                                                        = 0;
};

class MenuItem
{
public:
    
    typedef void (*function_t)(const MenuItem &item);
    
    std::string text;
    function_t  function;
    void       *context;    
    AABox       bounds;
    Fader       touched;
    
    inline MenuItem() : function(0), context(0), touched(0.0f) {}
};

class Menu : public IScreen
{
public:
    
    Menu(const char *title, int rowheight);
    
    void Add(const MenuItem &item);
    void RemoveLast();

    void Set(GameUI &ui);
    bool HandleTouch(const core::Point &point, const OverheadCamera &camera);
    
    bool IsVisible() const  { return visible; }
    void SetVisible(bool v);
    
private:
    
    std::string           title;
    std::vector<MenuItem> items;
    int                   rowheight;
    bool                  visible;
    Fader                 fader;
};

class Leaderboard : public IScreen
{
public:
    
    typedef void (*function_t)(void *context);    
    
    Leaderboard(const char *scoreTitle, const char *levelScoreTitle, HighScores &scores);
    
    void Set(GameUI &ui);
    bool HandleTouch(const core::Point &point, const OverheadCamera &camera);

    bool IsVisible() const  { return visible; }
    void SetVisible(bool v);
    
    void SetExitFunction(function_t f, void *context) { exitfn = f; this->context = context; }
    
private:
    
    void WriteMainHighScoreLine(GameUI &ui, float start, float size, int index, int begin, float alpha);
    void WriteLevelHighScoreLine(GameUI &ui, float start, float size, int index, int begin, float alpha);
    bool IsLevelHighScorePageEmpty(int page);
    void DrawTitle(GameUI &ui, const char *s, float alpha);
    
    enum Mode { MAIN_HIGH_SCORES, LEVEL_HIGH_SCORES_0 };

    Mode         mode;
    function_t   exitfn;
    std::string  scoreTitle,
                 levelScoreTitle;
    bool         visible;
    Fader        fader;
    HighScores  &scores;
    void        *context;
};

class TextScreen : public IScreen
{
public:
    
    typedef void (*function_t)(void *context);    
        
    TextScreen(const char *title, const char **text);
        
    void Set(GameUI &ui);
    bool HandleTouch(const core::Point &point, const OverheadCamera &camera);
        
    bool IsVisible() const  { return visible; }
    void SetVisible(bool v);
        
    void SetExitFunction(function_t f, void *context) { exitfn = f; this->context = context; }
        
private:
        
    void DrawTitle(GameUI &ui, const char *s, float alpha);
    void DrawText(GameUI &ui, const char *s, float start, float size, float alpha);
    
    int                       screen;
    function_t                exitfn;
    std::string               title;
    std::vector<std::string>  text;
    bool                      visible;
    Fader                     fader;
    void                     *context;
};

#endif // _MENU_H