/*
 *  Leaderboard.h
 *  GLGravity
 *
 *  Created by Paul Senzee on 1/9/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _LEADERBOARD_H
#define _LEADERBOARD_H

#include <string>

#include "core/aabox.h"
#include "core/xna.h"

#include "GameClock.h"
#include "IScreen.h"

class OverheadCamera;
class GameUI;
class GameInput;
class HighScores;

class Leaderboard : public IScreen
{
public:
    
    typedef void (*function_t)(void *context);    
    
    Leaderboard(const char *scoreTitle, const char *levelScoreTitle, HighScores &scores);
    
    void Set(GameUI &ui);
    bool HandleTouch(const core::Point &point, const OverheadCamera &camera);

    bool IsVisible() const  { return visible; }
    void SetVisible(bool v);
    
    void SetExitFunction(function_t f, void *context) { exitfn = f; this->context = context; }
    
private:
    
    void WriteMainHighScoreLine(GameUI &ui, float start, float size, int index, int begin, float alpha);
    void WriteLevelHighScoreLine(GameUI &ui, float start, float size, int index, int begin, float alpha);
    bool IsLevelHighScorePageEmpty(int page);
    void DrawTitle(GameUI &ui, const char *s, float alpha);
    
    enum Mode { MAIN_HIGH_SCORES, LEVEL_HIGH_SCORES_0 };

    Mode         mode;
    function_t   exitfn;
    std::string  scoreTitle,
                 levelScoreTitle;
    bool         visible;
    Fader        fader;
    HighScores  &scores;
    void        *context;
};

#endif // _LEADERBOARD_H