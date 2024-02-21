/*
 *  Audio.h
 *  GLGravity
 *
 *  Created by Paul Senzee on 2/1/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _AUDIO_H
#define _AUDIO_H

#include <map>
#include <string>

#include "time/GameClock.h"

class Audio
{
public:
        
    Audio();
    ~Audio();    
        
    bool Load(const char *filename, const char *soundname);
    void Unload(const char *soundname);
    
    bool SetSource(const char *soundname, const char *sourcename);
    void ReleaseSource(const char *sourcename);
    
    bool Play(const char *sourcename, float gain = 0.4f, bool loop = false);
  //bool Play(const char *sourcename, int ms, bool loop = false);
    bool Stop(const char *sourcename);

private:

    void  *device;  // void * = ALCdevice *
    void  *context; // void * = ALCcontext *
    
    typedef std::map<std::string, unsigned> map_t; // unsigned = ALuint
    
    map_t                        buffers;
    map_t                        sources;
    std::map<std::string, Fader> faders;
};

#endif // _AUDIO_H