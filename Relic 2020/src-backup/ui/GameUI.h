#ifndef _GAMEUI_H
#define _GAMEUI_H

#include "core/core.h"
#include "core/aabox.h"
#include "text/Glyph.h"

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

    enum { TEXT_DROP_SHADOW = 1, TEXT_GLOW = 2 };
        
    GameUI(const char **filename);

    void    SetGlyphPage(int index) { writer = mWriters[index % GLYPH_PAGES]; }
        
    void    SendBallMessage(const char *text, int delay = 0);
    
    void    DrawString(RenderContext &context, const char *s, const Vector2 &at, const Vector4 &color = Vector4(1.0f, 1.0f, 1.0f, 0.5f), float size = 26.0f, bool useInlineColor = true, GlyphWriter::TextJustify justify = GlyphWriter::TEXT_JUSTIFY_LEFT, int flags = TEXT_DROP_SHADOW);

    void    SetOverrideColor(const Vector4 &color);
    void    ClearOverrideColor();

    void    Update(const GameTime &time);
    void    Draw(RenderContext &context);
    
    Vector2 GetSize(const char *s);
    Vector2 GetSize(const char *s, const Vector2 &scale);
    AABox   GetCenteredHitTestBox(const char *s, const Vector2 &at, float scaler = 26.0f);
        
private:

    void  SendBallMessage(Text *text, const Vector3 &offset, int delay);
    void  SendBallMessage(Text *text, const Vector3 &offset, int delay, float scale);
    Text *GetText(const char *s, float speed);    

    GlyphWriter *mWriters[GLYPH_PAGES];
};

#endif  // _GAMEUI_H