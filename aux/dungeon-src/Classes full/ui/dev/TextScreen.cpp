/*
 *  TextScreen.cpp
 *  GLGravity
 *
 *  Created by Paul Senzee on 1/9/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#include "TextScreen.h"
#include "GameUI.h"
#include "OverheadCamera.h"

TextScreen::TextScreen(const char *title, const char *text) 
    : title(title ? title : ""), text(text ? text : ""), visible(false), fader(0.0f), exitfn(0), context(0)
{
}

void TextScreen::SetVisible(bool v)
{ 
    if (visible == v)
        return;
    visible = v;
    fader.Fade(visible ? 1.0f : 0.0f, 1000);
}

void TextScreen::DrawTitle(GameUI &ui, const char *s, float alpha)
{
    if (s && *s)
        ui.DrawStringCentered(s, Vector2(0.0f, 100.0f), alpha, 32.0f, false);
}

void TextScreen::DrawText(GameUI &ui, const char *s, float alpha)
{
    const Vector2 START(150.0f, 70.0f);
    const float   SIZE = 20.0f;    
    if (s && *s)
        ui.DrawString(s, START, alpha, SIZE, false);
}

void TextScreen::Set(GameUI &ui)
{
    float alpha = fader.GetValue();
    if (alpha < 0.01f)
        return;
    DrawTitle(ui, title.c_str(), alpha);
    DrawText(ui,  text.c_str(),  alpha);
}

bool TextScreen::HandleTouch(const core::Point &point, const OverheadCamera &camera)
{
    if (!IsVisible())
        return false;
    if (exitfn)
    {
        exitfn(context);
        return true;
    }
    return false;
}