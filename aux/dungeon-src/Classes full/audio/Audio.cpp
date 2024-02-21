/*
 *  Audio.cpp
 *  GLGravity
 *
 *  Created by Paul Senzee on 2/1/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#include <OpenAL/al.h>
#include <OpenAL/alc.h>

#include "Audio.h"
#include "core/global.h"
#include "OpenALSupport.h"

#include <AudioToolbox/AudioToolbox.h>

Audio::Audio() : device(0), context(0)
{
    // Create a new OpenAL Device
    // Pass NULL to specify the system’s default output device
    device = alcOpenDevice(NULL);
    if (device)
    {
        // Create a new OpenAL Context
        // The new context will render to the OpenAL Device just created 
        context = alcCreateContext((ALCdevice *)device, 0);
        if (context)
        {
            // Make the new context the Current OpenAL Context
            alcMakeContextCurrent((ALCcontext *)context);
        }
    }
            
    alGetError();
}

bool Audio::Play(const char *sourcename, float gain, bool loop)
{
    float GLOBAL_GAIN = 4.0f;
    
    if (!sourcename)
    {
        printf("[Audio::Play] Null audio source name specified!\n");
        return false;
    }
    std::string sn(sourcename);
    map_t::iterator i = sources.find(sn);
    if (i == sources.end())
    {
        printf("[Audio::Play] Audio source '%s' not found!\n", sourcename);
        return false;
    }
    printf("[Audio::Play] Start '%s'!\n", sourcename);
    ALenum error = AL_NO_ERROR;
    ALuint source = (*i).second;
    // Turn Looping ON
    alSourcei(source, AL_LOOPING, loop ? AL_TRUE : AL_FALSE);    
    // Begin playing our source file
    alSourcef(source, AL_GAIN, gain * GLOBAL_GAIN);
    alSourcePlay(source);
    if ((error = alGetError()) != AL_NO_ERROR)
    {
        printf("[Audio::Play] Error starting audio source: '%s' %u (ERROR %x)\n", sourcename, source, error);
        return false;
    }
    return true;
}

bool Audio::Stop(const char *sourcename)
{
    if (!sourcename)
    {
        printf("[Audio::Stop] Null audio source name specified!\n");
        return false;
    }
    std::string sn(sourcename);
    map_t::iterator i = sources.find(sn);
    if (i == sources.end())
    {
        printf("[Audio::Stop] Audio source '%s' not found!\n", sourcename);
        return false;
    }
    printf("[Audio::Stop] Stop Playing'%s'\n", sourcename);
    ALenum error = AL_NO_ERROR;        
    ALuint source = (*i).second;
    // Stop playing our source file    
    alSourceStop(source);
    if ((error = alGetError()) != AL_NO_ERROR)
    {
        printf("[Audio::Stop] Error stopping source: '%s' (ERROR %x)\n", sourcename, error);
        return false;
    }
    return true;
}

Audio::~Audio()
{
    ShutdownOpenAL();
}

/*
- (CGPoint)sourcePos
{
    return _sourcePos;
}

- (void)setSourcePos:(CGPoint)SOURCEPOS
{
    _sourcePos = SOURCEPOS;
    float sourcePosAL[] = {_sourcePos.x, _sourcePos.y, kDefaultDistance};
    // Move our audio source coordinates
    alSourcefv(_source, AL_POSITION, sourcePosAL);
}

- (CGPoint)listenerPos
{
    return _listenerPos;
}

- (void)setListenerPos:(CGPoint)LISTENERPOS
{
    _listenerPos = LISTENERPOS;
    float listenerPosAL[] = {_listenerPos.x, _listenerPos.y, 0.};
    // Move our listener coordinates
    alListenerfv(AL_POSITION, listenerPosAL);
}

- (CGFloat)listenerRotation
{
    return _listenerRotation;
}

- (void)setListenerRotation:(CGFloat)radians
{
    _listenerRotation = radians;
    float ori[] = {cos(radians + M_PI_2), sin(radians + M_PI_2), 0., 0., 0., 1.};
    // Set our listener orientation (rotation)
    alListenerfv(AL_ORIENTATION, ori);
}
 */
        
bool Audio::Load(const char *filename, const char *soundname)
{
    ALenum  error = AL_NO_ERROR;
    ALenum  format;
    ALvoid *data  = 0;
    ALsizei size  = 0;
    ALsizei freq  = 0;
            
    if (!filename || !soundname)
    {
        printf("[Audio::Load] Filename and sound name must be specified!\n");
        return false;
    }
            
    data = GetOpenALAudioData(globalTranslatePath(filename), &size, &format, &freq);
            
    if ((error = alGetError()) != AL_NO_ERROR)
    {
        printf("[Audio::Load] Error loading sound: '%s'->'%s' (ERROR %x)\n", filename, soundname, error);
        return false;
    }
    
    ALuint buffer = 0;
    
    // Create some OpenAL Buffer Objects
    alGenBuffers(1, &buffer);
    if ((error = alGetError()) != AL_NO_ERROR)
    {
        printf("[Audio::Load] Error generating buffer: '%s' %x", soundname, error);
        return false;
    }    

    // use the static buffer data API
    alBufferDataStaticProc(buffer, format, data, size, freq);
        
    if ((error = alGetError()) != AL_NO_ERROR)
    {
        printf("[Audio::Load] Error attaching audio to buffer: '%s' %x\n", soundname, error);
        return false;
    }
    
    buffers[std::string(soundname)] = buffer;
    
    return true;
}    
        
bool Audio::SetSource(const char *soundname, const char *sourcename)
{
    if (!soundname || !sourcename)
    {
        printf("[Audio::SetSource] Sound name ('%s') and source name ('%s') must be specified!\n", soundname, sourcename);
        return false;
    }
    
    map_t::iterator i = buffers.find(soundname);
    if (i == buffers.end())
    {
        printf("[Audio::SetSource] Unknown sound name '%s' for source '%s'\n", soundname, sourcename);
        return false;
    }

    ALenum error = AL_NO_ERROR;    
    
    ALuint source = 0, buffer = (*i).second;
    
    // Create some OpenAL Source Objects
    alGenSources(1, &source);
    if (alGetError() != AL_NO_ERROR) 
    {
        printf("[Audio::SetSource] Error generating source! '%s' %x\n", sourcename, error);
        return false;
    }    
    
    alGetError(); // Clear the error
                    
    // Set Source Position
    float sourcePosAL[] = {0.0f, 0.0f, 20.0f};
    alSourcefv(source, AL_POSITION, sourcePosAL);
            
    // Set Source Reference Distance
    alSourcef(source, AL_REFERENCE_DISTANCE, 50.0f);
    
    // attach OpenAL Buffer to OpenAL Source
    alSourcei(source, AL_BUFFER, buffer);
            
    if ((error = alGetError()) != AL_NO_ERROR)
    {
        printf("[Audio::SetSource] Error attaching buffer to source: '%s' %x\n", sourcename, error);
        return false;
    }
    
    sources[sourcename] = source;
    
    return true;
}