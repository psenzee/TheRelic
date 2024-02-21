/*
 *  GameClock.cpp
 *  GLGravity
 *
 *  Created by Paul Senzee on 1/1/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#include "GameClock.h"
#include "Timer.h"

GameClock::GameClock(unsigned time) 
    : time(time), start(0), suspend(0)
{
}

void GameClock::Restart()
{
    start = GetAppTimeMs();
}

void GameClock::Suspend()
{
    progress = Progress();
    suspend = GetAppTimeMs();
}

void GameClock::Resume()
{
    start += GetAppTimeMs() - suspend;
    suspend = 0;
}

float GameClock::Progress() const
{
    if (!start)
        return 0;
    if (suspend)
        return progress;
    int at = GetAppTimeMs() - start;
    float value = at / (float)time;
    if (time == 0.0f)
        value = 1.0f;
    if (value > 1.0f)
        value = 1.0f;
    return value;
}

bool GameClock::Expired() const
{
    return !suspend && (GetAppTimeMs() - start) >= time;
}