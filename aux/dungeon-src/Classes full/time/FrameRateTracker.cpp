/*
 *  FrameRateTracker.cpp
 *  GLGravity
 *
 *  Created by Paul Senzee on 1/1/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#include "FrameRateTracker.h"
#include "Timer.h"

#include <stdio.h>

FrameRateTracker::FrameRateTracker(const char *label, int logfrequency) 
    : label(label), sumtimes(0), sumtimescount(0), start(0), logfrequency(logfrequency), frames(0), lastframe(0)
{
    StartFrame();
}
        
void FrameRateTracker::StartFrame()
{
    int ms = GetCurrentTimeMs(); 
    lastframe = start ? ms - start : 0;
    start = ms;
}

int FrameRateTracker::TotalLastFrameTime() const
{
    return lastframe;
}

void FrameRateTracker::EndFrame()
{
    frames++;
    sumtimes += GetCurrentTimeMs() - start;
    sumtimescount++;
    if (logfrequency && sumtimescount == logfrequency)
    {
        double time = sumtimes / (double)sumtimescount;
        printf("[%-10s] fps %.2f (%.2fms/frame)\n", label, 1000.0f / time, time);
        sumtimes      = 0;
        sumtimescount = 0;
    }        
}

int FrameRateTracker::FrameCount() const
{
    return frames;
}