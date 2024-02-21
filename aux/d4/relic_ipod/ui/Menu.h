#ifndef _MENU_H
#define _MENU_H

#include <string>
#include <vector>

#include "core/aabox.h"
#include "core/core.h"

#include "time/GameClock.h"

class OverheadCamera;
class GameUI;
class GameInput;
class RenderContext;

class IScreen
{
public:
        
    virtual ~IScreen() {}
        
    virtual void Set(RenderContext &context, GameUI &ui)                                   = 0;
    virtual bool HandleTouch(const core::Point &point, const OverheadCamera &camera)       = 0;
    virtual void Reset()                                                                   = 0;
    virtual bool IsVisible()                                                         const = 0;
    virtual void SetVisible(bool v)                                                        = 0;
};

class MenuItem
{
public:
    
    typedef void (*function_t)(const MenuItem &item);
    
    std::string text;
    std::string luaFunction;
    function_t  function;
    void       *context;    
    AABox       bounds;
    Fader       touched;
    bool        animate;
    
    inline MenuItem() : function(0), context(0), touched(0.0f), animate(true) {}
};

class Menu : public IScreen
{
public:
    
    Menu(const char *title, int rowheight, float titlesz = 32.0f, bool thicktitle = true);
    
    void Add(const MenuItem &item);
    void RemoveLast();

    void Set(RenderContext &context, GameUI &ui);
    bool HandleTouch(const core::Point &point, const OverheadCamera &camera);
    
    void Reset();
    
    bool IsVisible() const  { return visible; }
    void SetVisible(bool v);
    
private:
    
    std::string           title;
    std::vector<MenuItem> items;
    int                   rowheight;
    bool                  visible;
    float                 titlesz;
    Fader                 fader;
    bool                  thicktitle;
};

/*
class Leaderboard : public IScreen
{
public:
    
    typedef void (*function_t)(void *context);    
    
    Leaderboard(const char *scoreTitle, const char *levelScoreTitle, HighScores &scores);
    
    void Set(RenderContext &context, GameUI &ui);
    bool HandleTouch(const core::Point &point, const OverheadCamera &camera);

    void Reset() {}
    
    bool IsVisible() const  { return visible; }
    void SetVisible(bool v);
    
    void SetExitFunction(function_t f, void *context) { exitfn = f; this->context = context; }
    
private:
    
    void WriteMainHighScoreLine(RenderContext &context, GameUI &ui, float start, float size, int index, int begin, float alpha);
    void WriteLevelHighScoreLine(RenderContext &context, GameUI &ui, float start, float size, int index, int begin, float alpha);
    bool IsLevelHighScorePageEmpty(int page);
    void DrawTitle(RenderContext &context, GameUI &ui, const char *s, float alpha);
    
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
*/

class TextScreen : public IScreen
{
public:
    
    typedef void (*function_t)(void *context);    
        
    TextScreen(const char *title, const char **text);
        
    void Set(RenderContext &context, GameUI &ui);
    bool HandleTouch(const core::Point &point, const OverheadCamera &camera);
        
    void Reset() {}    
        
    bool IsVisible() const  { return visible; }
    void SetVisible(bool v);
        
    void SetExitFunction(function_t f, void *context) { exitfn = f; this->context = context; }
        
private:
        
    void DrawTitle(RenderContext &context, GameUI &ui, const char *s, float alpha);
    void DrawText(RenderContext &context, GameUI &ui, const char *s, float start, float size, float alpha);
    
    int                       screen;
    function_t                exitfn;
    std::string               title;
    std::vector<std::string>  text;
    bool                      visible;
    Fader                     fader;
    void                     *context;
};

#endif // _MENU_H