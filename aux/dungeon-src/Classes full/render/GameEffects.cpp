/*
 *  GameEffects.cpp
 *  GLGravity
 *
 *  Created by Paul Senzee on 1/10/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#include "GameEffects.h"

static const float FADED_ALPHA       = 0.5f;
static const int   QUICKFADE_TIME    = 250;
static const int   GAMEFADE_TIME     = 2000;
static const int   FLASH_TIME        = 1000;

static float flash(float v)
{
    return (v <= 0.5f ? v : (1.0f - v)) * 2.0f;
}

void GameEffects::Darken(int time)
{
    black.Fade(FADED_ALPHA, time < 0 ? QUICKFADE_TIME : time);
}

void GameEffects::UnDarken(int time)
{
    black.Fade(0.0f, time < 0 ? QUICKFADE_TIME : time);
}

void GameEffects::Lighten(int time)
{
    white.Fade(FADED_ALPHA, time < 0 ? QUICKFADE_TIME : time);
}

void GameEffects::UnLighten(int time)
{
    white.Fade(0.0f, time < 0 ? QUICKFADE_TIME : time);
}

void GameEffects::Flash(int time)
{
    white.Fade(flash, time < 0 ? FLASH_TIME : time);
}

void GameEffects::FadeGameToBackground(int time)
{
    background.Fade(1.0f, time < 0 ? GAMEFADE_TIME : time);
}

void GameEffects::FadeBackgroundToGame(int time)
{
    background.Fade(0.0f, time < 0 ? GAMEFADE_TIME : time);
}