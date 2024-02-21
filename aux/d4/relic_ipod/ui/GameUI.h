#ifndef _GAMEUI_H
#define _GAMEUI_H

#include "core/core.h"
#include "core/aabox.h"

class GameState;
class Text;
class MessageRenderer;
class GlyphWriter;
class RenderContext;

class GameUI
{
public:
        
    MessageRenderer  *messager;
    GlyphWriter      *writer;
    Vector2           scale;
        
    enum { GLYPH_PAGES = 3 }; // two pages right now
        
    GameUI(const char **filename);

    void    SetGlyphPage(int index) { writer = mWriters[index % GLYPH_PAGES]; }
        
    void    SendBallMessage(const char *text, int delay = 0);
    
    void    DrawString(RenderContext &context, const char *s, const Vector2 &at, float alpha = 0.5f, float size = 26.0f);
    void    DrawStringCentered(RenderContext &context, const char *s, const Vector2 &at, float alpha = 0.5f, float size = 26.0f);
    void    DrawThickStringCentered(RenderContext &context, const char *s, const Vector2 &at, float alpha = 0.5f, float size = 26.0f);
    void    DrawTextCentered(RenderContext &context, const char *s, const Vector2 &at, float alpha, float size = 26.0f);
    void    DrawThickTextCentered(RenderContext &context, const char *s, const Vector2 &at, float alpha, float size = 26.0f);

    void    SetOverrideColor(const Vector4 &color);
    void    ClearOverrideColor();

    void    Update(const GameTime &time);
    void    Draw(RenderContext &context);
    
    Vector2 GetSize(const char *s);
    AABox   GetCenteredHitTestBox(const char *s, const Vector2 &at, float scaler = 26.0f);
        
private:

    void  SendBallMessage(Text *text, const Vector3 &offset, int delay);
    Text *GetText(const char *s, float speed);    

    GlyphWriter *mWriters[GLYPH_PAGES];
};

#endif  // _GAMEUI_H