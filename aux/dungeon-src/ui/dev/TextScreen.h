/*
 *  TextScreen.h
 *  GLGravity
 *
 *  Created by Paul Senzee on 1/9/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _TEXTSCREEN_H
#define _TEXTSCREEN_H

#include <string>

#include "core/aabox.h"
#include "core/xna.h"

#include "GameClock.h"
#include "IScreen.h"

class OverheadCamera;
class GameUI;
class GameInput;

class TextScreen : public IScreen
{
public:
    
    typedef void (*function_t)(void *context);    
    
    TextScreen(const char *title, const char *text);
    
    void Set(GameUI &ui);
    bool HandleTouch(const core::Point &point, const OverheadCamera &camera);

    bool IsVisible() const  { return visible; }
    void SetVisible(bool v);
    
    void SetExitFunction(function_t f, void *context) { exitfn = f; this->context = context; }
    
private:

    void DrawTitle(GameUI &ui, const char *s, float alpha);
    void DrawText(GameUI &ui, const char *s, float alpha);    

    function_t   exitfn;
    std::string  title,
                 text;
    bool         visible;
    Fader        fader;
    void        *context;
};

#endif // _TEXTSCREEN_H