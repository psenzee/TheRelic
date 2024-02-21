/*
 *  GameUI.cpp
 *  GLGravity
 *
 *  Created by Paul Senzee on 12/31/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include "GameUI.h"
#include "MessageRenderer.h"

#include "core/core.h"
#include "text/Glyph.h"
#include "text/Text.h"

#include "render/OverheadCamera.h"
#include "render/Animator.h"
#include "render/RenderContext.h"
#include "render/GraphicsDevice.h"

#include "core/strs.h"

class TextAnimator;

GameUI::GameUI(const char **filenames) : messager(0), writer(0), scale(-0.75f, -0.75f)
{
    memset(mWriters, 0, sizeof(mWriters));
    for (int i = 0; i < GLYPH_PAGES; i++)
        if (filenames[i])
            mWriters[i] = new GlyphWriter(filenames[i]);
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
    Vector2 v(GetSize(text));
    SendBallMessage(GetText(text, 0.05f), Vector3(v.x * 0.5f, 0.0f, 0.0f), delay);
}

void GameUI::SendBallMessage(Text *text, const Vector3 &offset, int delay)
{
    text->Reset();
    messager->Add(new TextMessage(text, offset, Vector2(32.0f, 32.0f), delay, 100));
}

void GameUI::DrawString(RenderContext &context, const char *s, const Vector2 &at, float alpha, float size)
{
    if (!s || !*s)
        return;
    writer->DrawString(context, s, v2(context.camera.GetLookAt()), at, alpha, size, context.camera.GetView());
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
    Vector2 sz(GetSize(s) * hs);
    return AABox(Vector3(sz.x + at.x, at.y + hs, 0.0f), Vector3(-sz.x + hs + at.x, at.y - hs, 0.0f));
}

void GameUI::DrawStringCentered(RenderContext &context, const char *s, const Vector2 &at, float alpha, float size)
{
    if (!s || !*s)
        return;    
    
    float hs = size * 0.5f;    
    Vector2 sz(GetSize(s) * hs), point(sz.x + at.x, at.y);
    DrawString(context, s, point, alpha, size);
}

void GameUI::DrawTextCentered(RenderContext &context, const char *s, const Vector2 &at, float alpha, float size)
{
    const Vector2 window(240, 100);
    float         disappear = 20.0f;
    if (s && *s)
    {
        char **lines  = split_char_of_keep_empty(s, "\n");
        for (int i = 0; lines[i]; i++)
        {
            float y = at.y - size * i;
            if (y < window.y && y > -window.y)
                DrawStringCentered(context, lines[i], Vector2(at.x, y), alpha, size);
            if (y >= window.y && y  <=  window.y + disappear)
                DrawStringCentered(context, lines[i], Vector2(at.x, y), 1.0f - ((y - window.y) / disappear), size);
            if (y <= -window.y && y >= -(window.y + disappear))
                DrawStringCentered(context, lines[i], Vector2(at.x, y), 1.0f - ((-y - window.y) / disappear), size);
        }
        destroy(lines);
    }
}

void GameUI::DrawThickStringCentered(RenderContext &context, const char *s, const Vector2 &at, float alpha, float sz)
{
    SetGlyphPage(2);
    SetOverrideColor(Vector4(1.0f, 1.0f, 1.0f, alpha));
    float offset = 1.0f;
    /*
    DrawStringCentered(context, s, at + Vector2( offset,  offset), alpha, sz);
    DrawStringCentered(context, s, at + Vector2(-offset,  offset), alpha, sz);
    DrawStringCentered(context, s, at + Vector2( offset, -offset), alpha, sz);
    DrawStringCentered(context, s, at + Vector2(-offset, -offset), alpha, sz);
    */
    ClearOverrideColor();
    SetGlyphPage(1);
    DrawStringCentered(context, s, at, alpha, sz);
    SetGlyphPage(0);
}

void GameUI::DrawThickTextCentered(RenderContext &context, const char *s, const Vector2 &at, float alpha, float size)
{
    const Vector2 window(240, 100);
    float         disappear = 20.0f;
    if (s && *s)
    {
        char **lines  = split_char_of_keep_empty(s, "\n");
        for (int i = 0; lines[i]; i++)
        {
            float y = at.y - size * i;
            if (y < window.y && y > -window.y)
                DrawThickStringCentered(context, lines[i], Vector2(at.x, y), alpha, size);
            if (y >= window.y && y  <=  window.y + disappear)
                DrawThickStringCentered(context, lines[i], Vector2(at.x, y), 1.0f - ((y - window.y) / disappear), size);
            if (y <= -window.y && y >= -(window.y + disappear))
                DrawThickStringCentered(context, lines[i], Vector2(at.x, y), 1.0f - ((-y - window.y) / disappear), size);
        }
        destroy(lines);
    }
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
