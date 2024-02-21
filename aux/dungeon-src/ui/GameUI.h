/*
 *  GameUI.h
 *  GLGravity
 *
 *  Created by Paul Senzee on 12/31/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _GAMEUI_H
#define _GAMEUI_H

#include "core/xna.h"
#include "core/aabox.h"

class GameState;
class Text;
class MessageRenderer;
class GlyphWriter;

class GameUI
{
public:
        
    GameState        *state;
    MessageRenderer  *messager;
    GlyphWriter      *writer;
    Vector2           scale;
        
    GameUI(GameState *state, const char *filename);
        
    void    SendBallMessage(const char *text, int delay = 0);
    
    void    DrawString(const char *s, const Vector2 &at, float alpha = 0.5f, float size = 26.0f);
    void    DrawStringCentered(const char *s, const Vector2 &at, float alpha = 0.5f, float size = 26.0f);
    void    DrawTextCentered(const char *s, const Vector2 &at, float alpha, float size = 26.0f);
    void    Update(const GameTime &time);
    void    Draw();
    
    Vector2 GetSize(const char *s);
    AABox   GetCenteredHitTestBox(const char *s, const Vector2 &at, float scaler = 26.0f);
        
private:

    void  SendBallMessage(Text *text, const Vector3 &offset, int delay);
    Text *GetText(const char *s, float speed);    
};

#endif  // _GAMEUI_H