/*
 *  GameClock.h
 *  GLGravity
 *
 *  Created by Paul Senzee on 1/1/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _GAMECLOCK_H
#define _GAMECLOCK_H

#include <math.h>

class GameClock
{    
public:
        
    GameClock(unsigned time);
        
    void  Restart();
    void  Suspend();
    void  Resume();
    bool  Expired()  const;
    float Progress() const;
    
private:
    
    unsigned time;
    unsigned start,
             suspend;
    float    progress;
};

class Fader
{
public:    
    
    typedef float (*function_t)(float);
        
    inline Fader(float from = 0.0f) : from(from), to(from), clock(1), function(0) {}
        
    inline void Fade(float to, int ms)
    {
        from = GetValue();
        this->to = to;
        function = 0;
        clock = GameClock(ms);
        clock.Restart();
    }
    inline void Fade(function_t fn, int ms)
    {
        function = fn;
        from = to = 0.0f;
        clock = GameClock(ms);
        clock.Restart();        
    }
    inline void Set(float v)
    {
        from = to = v;
    }
    inline float GetValue() const
    {
        float progress = clock.Progress();
        if (fabsf(progress - 1.0f) < 0.001)
        {
            Fader *mutable_fader = const_cast<Fader *>(this);
            mutable_fader->Set(to);
        }
        if (function)
            return function(progress);
        return progress * (to - from) + from;
    }
    
    
private:
    
    float      from, to;
    GameClock  clock;
    function_t function;
};

#endif // _GAMECLOCK_H