/*
 *  Leaderboard.cpp
 *  GLGravity
 *
 *  Created by Paul Senzee on 1/9/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#include "Leaderboard.h"
#include "GameUI.h"
#include "OverheadCamera.h"
#include "HighScores.h"

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

void Leaderboard::WriteMainHighScoreLine(GameUI &ui, float start, float size, int index, int begin, float alpha)
{
    const LevelScore &score = scores.GetScore(index + begin);
    if (score.score && *score.name && score.level)
    {    
        char s[128]; 
        float at = start - index * size;
        sprintf(s, "%s", score.name);
        ui.DrawString(s, Vector2(150.0f, at), alpha, size, false);
        sprintf(s, "L%u", score.level);
        ui.DrawString(s, Vector2(-30.0f, at), alpha, size, false);        
        sprintf(s, "%u", score.score);
        ui.DrawString(s, Vector2(-90.0f, at), alpha, size, false);
    }
}

void Leaderboard::WriteLevelHighScoreLine(GameUI &ui, float start, float size, int index, int begin, float alpha)
{
    const LevelScore &score = scores.GetLevelScore(index + begin);
    if (score.score && *score.name && score.level)
    {
        char s[128]; 
        float at = start - index * size;
        sprintf(s, "%s", score.name);
        ui.DrawString(s, Vector2(150.0f, at), alpha, size, false);
        sprintf(s, "L%u", score.level);
        ui.DrawString(s, Vector2(-30.0f, at), alpha, size, false);        
        sprintf(s, "%u", score.score);
        ui.DrawString(s, Vector2(-90.0f, at), alpha, size, false);
    }
}

void Leaderboard::DrawTitle(GameUI &ui, const char *s, float alpha)
{
    if (s && *s)
        ui.DrawStringCentered(s, Vector2(0.0f, 100.0f), alpha, 32.0f, false);
}

void Leaderboard::Set(GameUI &ui)
{
    float alpha = fader.GetValue();
    if (alpha < 0.01f)
        return;
    DrawTitle(ui, (mode == MAIN_HIGH_SCORES) ? scoreTitle.c_str() : levelScoreTitle.c_str(), alpha);
    float start = 70.0f, size = 20.0f;
    for (int i = 0; i < 10; i ++)
    {
        if (mode == MAIN_HIGH_SCORES)
            WriteMainHighScoreLine(ui, start, size, i, 0, alpha);
        else
            WriteLevelHighScoreLine(ui, start, size, i, (mode - LEVEL_HIGH_SCORES_0) * 10 + 1, alpha);
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