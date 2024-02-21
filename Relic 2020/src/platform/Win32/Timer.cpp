/*
 *  Timer.cpp
 *  GLGravity
 *
 *  Created by Paul Senzee on 12/30/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include "time/Timer.h"

#include <windows.h>
#include <mmsystem.h>
#include <stdio.h>

typedef unsigned __int64 uint64_t;

unsigned int GetCurrentTimeMs()
{
    return (unsigned int)::timeGetTime();
}

uint64_t _AppTime = 0, _LastTime = 0;

void UpdateAppTime()
{
    uint64_t time    = ::timeGetTime(),
             elapsed = time - _LastTime; 
    _LastTime = time;
    _AppTime += elapsed;
}

unsigned int GetAppTimeMs()
{
    return (unsigned int)(_AppTime);
}