#include "GameUI.h"
#include "MessageRenderer.h"

#include "core/core.h"
#include "text/Glyph.h"
#include "text/GlyphWriter.h"
#include "text/Text.h"

#include "render/OverheadCamera.h"
#include "render/Animator.h"
#include "render/RenderContext.h"
#include "render/GraphicsDevice.h"

#include "core/strs.h"

class TextAnimator;

extern bool IsLowEndDevice();

GameUI::GameUI(const char **filenames) : messager(0), writer(0), scale(-1.0f, -1.0f)
{
    memset(mWriters, 0, sizeof(mWriters));
    for (int i = 0; i < GLYPH_PAGES && filenames[i]; i++) {
        printf("Glyph filename (%d) %s\n", i, filenames[i]);
        mWriters[i] = new GlyphWriter(filenames[i]);
    }
    messager = new MessageRenderer;
    SetGlyphPage(0);
}
        
Text *GameUI::GetText(const char *s, float speed)
{
    return new Text(writer, s,
                    Vector2(0.0f, 0.0f), scale,
                    Vector3(1.0f, 1.0f, 1.0f),
                    new TextAnimator(Vector3(speed, speed, speed)));
}

void GameUI::SendBallMessage(const char *text, int delay)
{
    float scale = 24.0f;
    if (text[0] == '@' && isdigit(text[1]) && isdigit(text[2])) // we're going to use a size multiplier
    {
        scale *= (text[1] - '0') + (text[2] - '0') * 0.1f;
        text += 3;
    }
    Vector2 v(GetSize(text));
    SendBallMessage(GetText(text, 0.05f), Vector3(v.x * 0.5f, 0.0f, 0.0f), delay, scale);
}

void GameUI::SendBallMessage(Text *text, const Vector3 &offset, int delay, float scale)
{
    text->Reset();
    messager->Add(new TextMessage(text, offset, Vector2(scale, scale), delay, 100));
}

void GameUI::SendBallMessage(Text *text, const Vector3 &offset, int delay)
{
    SendBallMessage(text, offset, delay, 24.0f);
}

void GameUI::DrawString(RenderContext &context, const char *s, const Vector2 &at, const Vector4 &color, float size, bool useInlineColor, GlyphWriter::TextJustify justify, int flags)
{
    if (!s || !*s)
        return;

    if (!IsLowEndDevice())
    {
        if (flags & TEXT_DROP_SHADOW)
        {
            writer->DrawString(context, s, v2(context.camera.GetLookAt()), at - Vector2(1.f, 1.f), Vector4(0.f, 0.f, 0.f, 1.f) * color, size, context.camera.GetView(), false, justify);
        }

        if (flags & TEXT_GLOW)
        {
            float glow = 0.15f;
            writer->DrawString(context, s, v2(context.camera.GetLookAt()), at - Vector2( 2.f,  2.f), Vector4(1.f, 1.f, 1.f, glow * color.w), size, context.camera.GetView(), false, justify);
            writer->DrawString(context, s, v2(context.camera.GetLookAt()), at - Vector2(-2.f, -2.f), Vector4(1.f, 1.f, 1.f, glow * color.w), size, context.camera.GetView(), false, justify);
            writer->DrawString(context, s, v2(context.camera.GetLookAt()), at - Vector2(-2.f,  2.f), Vector4(1.f, 1.f, 1.f, glow * color.w), size, context.camera.GetView(), false, justify);
            writer->DrawString(context, s, v2(context.camera.GetLookAt()), at - Vector2( 2.f, -2.f), Vector4(1.f, 1.f, 1.f, glow * color.w), size, context.camera.GetView(), false, justify);
        }
    }
    writer->DrawString(context, s, v2(context.camera.GetLookAt()), at, color, size, context.camera.GetView(), useInlineColor, justify);
}

void GameUI::SetOverrideColor(const Vector4 &color)
{
    writer->_hack_override_color = &color;
}

void GameUI::ClearOverrideColor()
{
    writer->_hack_override_color = 0;
}

Vector2 GameUI::GetSize(const char *s)
{
    return GetSize(s, scale);
}

Vector2 GameUI::GetSize(const char *s, const Vector2 &scale)
{
    size_t dsz = strlen(s);
    char *t = new char [dsz + 1];
    Text::StripColor(s, t);
    Vector2 sz = writer->GetSize(t, scale);
    delete [] t;
    return sz;
}

AABox GameUI::GetCenteredHitTestBox(const char *s, const Vector2 &at, float size)
{
    float hs = size * 0.5f;
    Vector2 sz(GetSize(s, Vector2(size, size)) * hs);
    return AABox(Vector3(sz.x + at.x, at.y + hs, 0.0f), Vector3(-sz.x + hs + at.x, at.y - hs, 0.0f));
}

void GameUI::Update(const GameTime &time)
{
    messager->Update(time);
    writer->Update();
}
        
void GameUI::Draw(RenderContext &context)
{
    context.device.EnableDepthTest(false);  // glDisable(GL_DEPTH_TEST); 
    context.device.EnableDepthWrite(false); // glDepthMask(GL_FALSE);

    messager->Draw(context);

    context.device.EnableDepthTest(true);   // glEnable(GL_DEPTH_TEST);
    context.device.EnableDepthWrite(true);  // glDepthMask(GL_TRUE);    
}
