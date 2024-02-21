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

#include "core/xna.h"
#include "text/Glyph.h"
#include "text/Text.h"

#include "game/GameState.h" // $BAD_DEPENDENCY
#include "render/OverheadCamera.h"
#include "render/Animator.h"
#include "render/LocalGameServices.h"
#include "render/GraphicsDevice.h"

#include "core/strs.h"

class TextAnimator;

GameUI::GameUI(GameState *state, const char *filename) : state(state), messager(0), writer(0), scale(-1.0f, -1.0f)
{
    writer      = new GlyphWriter(filename, state->services);
    messager    = new MessageRenderer(state->services);
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
    SendBallMessage(GetText(text, 0.15f), Vector3(v.x * 0.5f, 0.0f, 0.0f), delay);
}

void GameUI::SendBallMessage(Text *text, const Vector3 &offset, int delay)
{
    text->Reset();
    messager->Add(new TextMessage(text, *(state->camera), offset, Vector2(32.0f, 32.0f), delay, 100));
}

void GameUI::DrawString(const char *s, const Vector2 &at, float alpha, float size)
{
    if (!s || !*s)
        return;
/*    
    Vector3 lookAt(state->camera->GetLookAt());
    Text *text = new Text(writer, s,
                          Vector2(0.0f, 0.0f), scale,
                          Vector3(1.0f, 1.0f, 1.0f), 0);
    Matrix ms, m;
    ms.scale(Vector3(size, size, 1.0f));
    m.translate(Vector3(lookAt.x + at.x, lookAt.y + at.y, 0.0f));
    m = ms * m;
    glDisable(GL_DEPTH_TEST);
    text->Render(m, *(state->camera), Vector4(1.0f, 1.0f, 1.0f, alpha));
    glEnable(GL_DEPTH_TEST);
    delete text;
*/
    writer->DrawString(s, v2(state->camera->GetLookAt()), at, alpha, size, state->camera->GetView());
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

void GameUI::DrawStringCentered(const char *s, const Vector2 &at, float alpha, float size)
{
    if (!s || !*s)
        return;    
    
    float hs = size * 0.5f;    
    Vector2 sz(GetSize(s) * hs), point(sz.x + at.x, at.y);
    DrawString(s, point, alpha, size);
}

void GameUI::DrawTextCentered(const char *s, const Vector2 &at, float alpha, float size)
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
                DrawStringCentered(lines[i], Vector2(at.x, y), alpha, size);
            if (y >= window.y && y  <=  window.y + disappear)
                DrawStringCentered(lines[i], Vector2(at.x, y), 1.0f - ((y - window.y) / disappear), size);
            if (y <= -window.y && y >= -(window.y + disappear))
                DrawStringCentered(lines[i], Vector2(at.x, y), 1.0f - ((-y - window.y) / disappear), size);
        }
        destroy(lines);
    }
}

void GameUI::Update(const GameTime &time)
{
    messager->Update(time);
    writer->Update();
}
        
void GameUI::Draw()
{
    LocalGameServices *services = state->services;

    // $TODO why is this stuff in update - shouldn't it be in Draw??? $BUG?
    // NOTE THAT THIS WAS (WRONGLY) IN UPDATE BEFORE, SO IT'S POSSIBLE IT COULD CAUSE A BUG NOW
    services->device->EnableDepthTest(false);  // glDisable(GL_DEPTH_TEST); 
    services->device->EnableDepthWrite(false); // glDepthMask(GL_FALSE);
//    services->device->EnableLighting(false);   // glDisable(GL_LIGHTING);

    messager->Draw();

    services->device->EnableDepthTest(true);   // glEnable(GL_DEPTH_TEST);
    services->device->EnableDepthWrite(true);  // glDepthMask(GL_TRUE);    
//    services->device->EnableLighting(true);    // glEnable(GL_LIGHTING);
}
