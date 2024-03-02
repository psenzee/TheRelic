/*
 *  FrameRateTracker.h
 *  GLGravity
 *
 *  Created by Paul Senzee on 1/1/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _FRAMERATETRACKER_H
#define _FRAMERATETRACKER_H

class FrameRateTracker
{    
public:
        
    FrameRateTracker(const char *label, int logfrequency = 20);
        
    void StartFrame();
    void EndFrame();
    
    int  TotalLastFrameTime() const;
    int  FrameCount() const;
    
private:
    
    const char *label;
    int         sumtimes,
                sumtimescount,
                start,
                logfrequency,
                frames,
                lastframe;
};

#endif // _FRAMERATETRACKER_H