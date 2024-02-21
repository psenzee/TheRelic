/*
 *  Timer.cpp
 *  GLGravity
 *
 *  Created by Paul Senzee on 12/30/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include "time/Timer.h"

#include <mach/mach.h>
#include <mach/mach_time.h>

#include <stdio.h>

unsigned int GetCurrentTimeMs()
{
    uint64_t time = mach_absolute_time(); 
    mach_timebase_info_data_t info;
    mach_timebase_info(&info);
    double value = (double)info.numer / (double)info.denom / 1000000.0;
    return (unsigned int)(time * value);
//  return (unsigned int)(time / info.denom * info.numer / 1000000ull);    
}

uint64_t _AppTime = 0, _LastTime = 0;

void UpdateAppTime()
{
    uint64_t time    = mach_absolute_time(),
             elapsed = time - _LastTime; 
    mach_timebase_info_data_t info; 
    mach_timebase_info(&info);
    _LastTime = time;
    _AppTime += elapsed * ((double)info.numer / info.denom);
//  _AppTime += elapsed / info.denom * info.numer;    
}

unsigned int GetAppTimeMs()
{
    return (unsigned int)(_AppTime / 1000000ull);
}