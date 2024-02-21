/*
 *  Menu.cpp
 *  GLGravity
 *
 *  Created by Paul Senzee on 1/9/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#include "Menu.h"
#include "GameUI.h"

#include "core/strs.h"
#include "render/OverheadCamera.h"
#include "render/RenderContext.h"
#include "render/GraphicsDevice.h"
    
Menu::Menu(const char *title, int rowheight, float titlesz, bool thicktitle) : title(title ? title : ""), rowheight(rowheight), titlesz(titlesz), visible(false), fader(0.0f), thicktitle(thicktitle)
{
}
        
void Menu::Add(const MenuItem &item)
{
    items.push_back(item);
}

void Menu::RemoveLast()
{
    items.pop_back();
}

void Menu::SetVisible(bool v)
{ 
    if (visible == v)
        return;
    visible = v;
    fader.Fade(visible ? 1.0f : 0.0f, 1000);
    for (std::vector<MenuItem>::iterator i = items.begin(), e = items.end(); i != e; ++i)
        if ((*i).animate) (*i).touched.Fade(0.0f, 1);
}

void Menu::Reset()
{
    for (std::vector<MenuItem>::iterator i = items.begin(), e = items.end(); i != e; ++i)
        if ((*i).animate) (*i).touched.Set(0.0f);
}

void Menu::Set(RenderContext &context, GameUI &ui)
{
    float alpha = fader.GetValue();
    if (alpha < 0.01f)
        return;
    if (!title.empty())
    {
        const char *s = title.c_str();
        if (!thicktitle)
            ui.DrawStringCentered(context, s, Vector2(0.0f, 80.0f), alpha, titlesz);
        else
            ui.DrawThickStringCentered(context, s, Vector2(0.0f, 80.0f), alpha, titlesz);
    }
    float start = 0.0f, size = 26.0f;
    int   count = 0;
    // we have a line about which it should be centered
    float centerline = -45.0f;
    start = centerline + items.size() * rowheight * 0.5f;
    for (std::vector<MenuItem>::iterator i = items.begin(), e = items.end(); i != e; ++i)
    {
        MenuItem &item = *i;
        if (!item.text.empty())
        {
            Vector2 at(0.0f, start - count * rowheight);
            AABox &box = item.bounds;
            const char *s = item.text.c_str();
            if (!box.valid()) // uninitialized bounds
            {
                box = ui.GetCenteredHitTestBox(s, at, size);
                box.expand(Vector3(size * 2.0f, math::max(rowheight - size, 0.0f), 0.0f));
            }
            
            float sz = size, animated = 0.f;
            if (item.animate)
            {
                animated = item.touched.GetValue();
            if (animated < 1.0f)
                sz += sz * animated * 3.0f;
//            else
//                item.touched.Fade(0.0f, 500);
            }
            ui.DrawThickStringCentered(context, s, at, alpha * (1.0f - animated), sz);
        }
        count++;
    }
}

bool Menu::HandleTouch(const core::Point &point, const OverheadCamera &camera)
{
    if (!IsVisible())
        return false;
    
    static const Vector4 TEXT_UI_PLANE(0.0f, 0.0f, 1.0f, 0.0f);
    Vector3 pick, lookAt(camera.GetLookAt());
    camera.PickPlanePoint(TEXT_UI_PLANE, Vector2(-(point.x / 160.0f - 1), point.y / 240.0f - 1), pick);
    pick.x = lookAt.x - pick.x;
    pick.y = lookAt.y - pick.y;
    for (std::vector<MenuItem>::iterator i = items.begin(), e = items.end(); i != e; ++i)
    {
        MenuItem &item = *i;
        if (item.function && 
            pick.x >= item.bounds.minimum.x && pick.x <= item.bounds.maximum.x &&
            pick.y >= item.bounds.minimum.y && pick.y <= item.bounds.maximum.y)
        {
            (item.function)(item);
            item.touched.Fade(1.0f, 250);
            return true;
        }
    }
    return false;
}
/*
Leaderboard::Leaderboard(const char *scoreTitle, const char *levelScoreTitle, HighScores &scores) 
    : scoreTitle(scoreTitle ? scoreTitle : ""), levelScoreTitle(levelScoreTitle ? levelScoreTitle : ""), 
      visible(false), fader(0.0f), scores(scores), mode(MAIN_HIGH_SCORES), exitfn(0), context(0)
{
}

void Leaderboard::SetVisible(bool v)
{ 
    if (visible == v)
        return;
    visible = v;
    fader.Fade(visible ? 1.0f : 0.0f, 1000);
    if (visible)
        mode = MAIN_HIGH_SCORES;
}

void Leaderboard::WriteMainHighScoreLine(RenderContext &context, GameUI &ui, float start, float size, int index, int begin, float alpha)
{
    const LevelScore &score = scores.GetScore(index + begin);
    if (score.score && *score.name && score.level)
    {    
        char s[128]; 
        float at = start - index * size;
        sprintf(s, "%s", score.name);
        ui.DrawString(context, s, Vector2(150.0f, at), alpha, size);
        sprintf(s, "L%u", score.level);
        ui.DrawString(context, s, Vector2(-30.0f, at), alpha, size);        
        sprintf(s, "%u", score.score);
        ui.DrawString(context, s, Vector2(-90.0f, at), alpha, size);
    }
}

void Leaderboard::WriteLevelHighScoreLine(RenderContext &context, GameUI &ui, float start, float size, int index, int begin, float alpha)
{
    const LevelScore &score = scores.GetLevelScore(index + begin);
    if (score.score && *score.name && score.level)
    {
        char s[128]; 
        float at = start - index * size;
        sprintf(s, "%s", score.name);
        ui.DrawString(context, s, Vector2(150.0f, at), alpha, size);
        sprintf(s, "L%u", score.level);
        ui.DrawString(context, s, Vector2(-30.0f, at), alpha, size);        
        sprintf(s, "%u", score.score);
        ui.DrawString(context, s, Vector2(-90.0f, at), alpha, size);
    }
}

void Leaderboard::DrawTitle(RenderContext &context, GameUI &ui, const char *s, float alpha)
{
    if (s && *s)
        ui.DrawStringCentered(context, s, Vector2(0.0f, 100.0f), alpha, 32.0f);
}

void Leaderboard::Set(RenderContext &context, GameUI &ui)
{
    float alpha = fader.GetValue();
    if (alpha < 0.01f)
        return;
    DrawTitle(context, ui, (mode == MAIN_HIGH_SCORES) ? scoreTitle.c_str() : levelScoreTitle.c_str(), alpha);
    float start = 70.0f, size = 20.0f;
    for (int i = 0; i < 10; i ++)
    {
        if (mode == MAIN_HIGH_SCORES)
            WriteMainHighScoreLine(context, ui, start, size, i, 0, alpha);
        else
            WriteLevelHighScoreLine(context, ui, start, size, i, (mode - LEVEL_HIGH_SCORES_0) * 10 + 1, alpha);
    }
}

bool Leaderboard::IsLevelHighScorePageEmpty(int page)
{
    for (int i = 0; i < 10; i++)
        if (scores.GetLevelScore(i + page * 10 + 1).score != 0)
            return false;
    return true;
}

bool Leaderboard::HandleTouch(const core::Point &point, const OverheadCamera &camera)
{
    if (!IsVisible())
        return false;
    if (mode == MAIN_HIGH_SCORES)
    {
        mode = LEVEL_HIGH_SCORES_0;
        return true;
    }
    else if (!IsLevelHighScorePageEmpty(mode - LEVEL_HIGH_SCORES_0 + 1))
    {
        mode = Mode(mode + 1);
        return true;
    }
    else if (exitfn)
    {
        exitfn(context);
        return true;
    }
    return false;
}
*/

TextScreen::TextScreen(const char *title, const char **screentext) : screen(0), title(title ? title : ""), visible(false), fader(0.0f), exitfn(0), context(0)
{
    while (*screentext)
    {
        text.push_back(*screentext);
        screentext++;
    }
}

void TextScreen::SetVisible(bool v)
{ 
    if (visible == v)
        return;
    visible = v;
    fader.Fade(visible ? 1.0f : 0.0f, 1000);
    if (visible)
        screen = 0;
}

void TextScreen::DrawText(RenderContext &context, GameUI &ui, const char *s, float start, float size, float alpha)
{
    //ui.DrawTextCentered(context, s, Vector2(0.0f, start), alpha, size);
    ui.DrawThickTextCentered(context, s, Vector2(0.0f, start), alpha, size);
}

void TextScreen::DrawTitle(RenderContext &context, GameUI &ui, const char *s, float alpha)
{
    if (s && *s)
        //ui.DrawStringCentered(context, s, Vector2(0.0f, 100.0f), alpha, 32.0f);
        ui.DrawThickStringCentered(context, s, Vector2(0.0f, 100.0f), alpha, 32.0f);
}

void TextScreen::Set(RenderContext &context, GameUI &ui)
{
    float alpha = fader.GetValue();
    if (alpha < 0.01f)
        return;
    DrawTitle(context, ui, title.c_str(), alpha);
    float start = 50.0f, size = 26.0f;
    if (!text.empty() && screen < (int)text.size())
        DrawText(context, ui, text[screen].c_str(), start, size, alpha);
    if (screen < (int)(text.size() - 1))
    {
        ui.DrawStringCentered(context, "\\#808080>>", Vector2(0.0f, -110.0f), alpha, 32.0f);
        if (exitfn)
            ui.DrawString(context, "\\%c0c0c060~", Vector2(-150.0f, -105.0f), alpha, 26.0f);
    }
}

bool TextScreen::HandleTouch(const core::Point &point, const OverheadCamera &camera)
{
    if (!IsVisible())
        return false;
    if (point.x < 60 && point.y > 420)
    {
        // this should be our X - close screen..
        // skip whatever remains
        if (exitfn)
        {
            exitfn(context);
            return true;
        }
    }
    if (screen < (int)(text.size() - 1))
    {
        screen++;
        return true;
    }
    else if (exitfn)
    {
        exitfn(context);
        return true;
    }
    return false;
}