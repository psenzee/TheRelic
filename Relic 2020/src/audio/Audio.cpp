#include "Audio.h"
#include "core/global.h"
#include "core/strs.h"

#ifndef OPENAL

// winmm.lib opengl32.lib glu32.lib glaux.lib  "C:\Program Files\OpenAL 1.1 SDK\libs\Win32\OpenAL32.lib" .\freealut-1.1.0-bin\lib\alut.lib

void Audio::Vibrate() {}
bool Audio::Play(const char *sourcename, float gain, bool loop) { return true; }
bool Audio::Stop(const char *sourcename) { return true; }
Audio::Audio() {}
Audio::~Audio() {}
bool Audio::IsLoaded(const char *filename) const { return true; }
bool Audio::Load(const char *filename, const char *soundname) { return true; }
void Audio::SetListenerPosition(const Vector3 &pos) {}
bool Audio::SetSourcePosition(const char *sourcename, const Vector3 &pos) { return true; }
bool Audio::SetSource(const char *soundname, const char *sourcename) { return true; }
void Audio::SetSourceSecondsOffset(const char *sourcename, float seconds) {}
bool Audio::IsSourceDone(const char *sourcename) const { return true; }
void Audio::ReleaseSource(const char *sourcename) {}

#else

#ifdef WIN32
#include <al.h>
#include <alc.h>
#include <alut.h>
static void Vibrate()
{
}
#else
#import <OpenAL/al.h>
#import <OpenAL/alc.h>
#import <AudioToolbox/AudioToolbox.h>
static void Vibrate()
{
    AudioServicesPlaySystemSound(kSystemSoundID_Vibrate);
}
#endif

#include "OpenALSupport.h"

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
        else
        {
            printf("[Audio::Audio] Unable to create new OpenAL context!\n");
        }
    }
        
    ALenum error = alGetError();
    if (error != AL_NO_ERROR)
    {
        printf("[Audio::Audio] An error occurred in alcMakeContextCurrent!\n");
    }
}

void Audio::Vibrate()
{
    ::Vibrate();
}

bool Audio::Play(const char *sourcename, float gain, bool loop)
{
    float GLOBAL_GAIN = 1.0f;
    
    if (!sourcename || !*sourcename)
    {
        printf("[Audio::Play] Null audio source name specified!\n");
        return false;
    }
    String sn(sourcename);
    map_t::iterator i = mSources.find(sn);
    if (i == mSources.end())
    {
        printf("[Audio::Play] Audio source '%s' not found!\n", sourcename);
        return false;
    }
//  printf("[Audio::Play] Start '%s'!\n", sourcename);
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
    if (!sourcename || !*sourcename)
    {
        printf("[Audio::Stop] Null audio source name specified!\n");
        return false;
    }
    String sn(sourcename);
    map_t::iterator i = mSources.find(sn);
    if (i == mSources.end())
    {
        printf("[Audio::Stop] Audio source '%s' not found!\n", sourcename);
        return false;
    }
//  printf("[Audio::Stop] Stop Playing'%s'\n", sourcename);
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

bool Audio::IsLoaded(const char *filename) const
{
    return mLoaded.find(filename) != mLoaded.end();
}

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
            
	const char *toloadfile = filename;
#ifndef WIN32	
    // we don't run wavs directly on an iPhone
    char caffile[1024];
    if (ends(filename, ".wav", true))
        sprintf(caffile, "%s.caf", filename);
    else
        strcpy(caffile, filename);
    toloadfile = caffile;	
#endif	
            
    data = GetOpenALAudioData(globalTranslatePath(toloadfile), &size, &format, &freq);
            
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
#ifdef WIN32
    alBufferData(buffer, format, data, size, freq);
#else
    alBufferDataStaticProc(buffer, format, data, size, freq);
#endif
        
    if ((error = alGetError()) != AL_NO_ERROR)
    {
        printf("[Audio::Load] Error attaching audio to buffer: '%s' %x\n", soundname, error);
        return false;
    }
    
    mBuffers[soundname] = buffer;
    mLoaded.insert(filename);
    
    return true;
}    

void Audio::SetListenerPosition(const Vector3 &pos)
{
    alListenerfv(AL_POSITION, (ALfloat *)&pos);
}

bool Audio::SetSourcePosition(const char *sourcename, const Vector3 &pos)
{
    if (!sourcename)
    {
        printf("[Audio::SetSourcePosition] Null audio source name specified!\n");
        return false;
    }
    String sn(sourcename);
    map_t::iterator i = mSources.find(sn);
    if (i == mSources.end())
    {
        printf("[Audio::SetSourcePosition] Audio source '%s' not found!\n", sourcename);
        return false;
    }
    ALenum error = AL_NO_ERROR;        
    ALuint source = (*i).second;
    alSourcefv(source, AL_POSITION, (ALfloat *)&pos);
    return true;
}
        
bool Audio::SetSource(const char *soundname, const char *sourcename)
{
    if (!soundname || !sourcename)
    {
        printf("[Audio::SetSource] Sound name ('%s') and source name ('%s') must be specified!\n", soundname, sourcename);
        return false;
    }
    
    map_t::iterator i = mBuffers.find(soundname);
    if (i == mBuffers.end())
    {
        printf("[Audio::SetSource] Unknown sound name '%s' for source '%s'\n", soundname, sourcename);
        return false;
    }

    ReleaseSource(sourcename);

    ALenum error = AL_NO_ERROR;    
    
    ALuint source = 0, buffer = (*i).second;
    
    // Create some OpenAL Source Objects
    alGenSources(1, &source);
    if ((error = alGetError()) != AL_NO_ERROR) 
    {
        printf("[Audio::SetSource] Error generating source! '%s' %x\n", sourcename, error);
        return false;
    }    
    
    alGetError(); // Clear the error
                    
    // Set Source Position
    float sourcePosAL[] = {0.0f, 0.0f, 0.0f};
    Vector3 velocity(0.f, 0.f, 0.f);
    alSourcefv(source, AL_POSITION, sourcePosAL);
    alSourcef (source, AL_PITCH,    1.0f     );
    alSourcef (source, AL_GAIN,     1.0f     );
    alSourcefv(source, AL_VELOCITY, (ALfloat *)&velocity);
    //alSourcefv(source, AL_VELOCITY, SourceVel);
    //alSourcei (source, AL_LOOPING,  loop     );
    ///*/
         
    alSourcef(source, AL_MAX_DISTANCE, 500.0f);
    // Set Source Reference Distance
    alSourcef(source, AL_REFERENCE_DISTANCE, 100.0f);
    
    // attach OpenAL Buffer to OpenAL Source
    alSourcei(source, AL_BUFFER, buffer);
            
    if ((error = alGetError()) != AL_NO_ERROR)
    {
        printf("[Audio::SetSource] Error attaching buffer to source: '%s' %x\n", sourcename, error);
        return false;
    }
    
    mSources[sourcename] = source;
    
    return true;
}

void Audio::SetSourceSecondsOffset(const char *sourcename, float seconds)
{
    if (!sourcename)
    {
        printf("[Audio::SetSourceSecondsOffset] Null audio source name specified!\n");
    }
    String sn(sourcename);
    map_t::iterator i = mSources.find(sn);
    if (i != mSources.end())
    {
        alSourcef((*i).second, AL_SEC_OFFSET, (ALfloat)seconds);
    }
}
void Audio::SetSourceVolume(const char *sourcename, float volume)
{
    if (!sourcename)
    {
        printf("[Audio::SetSourceVolume] Null audio source name specified!\n");
    }
    String sn(sourcename);
    map_t::iterator i = mSources.find(sn);
    if (i != mSources.end())
    {
        alSourcef((*i).second, AL_GAIN, (ALfloat)volume);
    }
}

bool Audio::IsSourceDone(const char *sourcename) const
{
    if (!sourcename)
    {
        printf("[Audio::IsSourceDone] Null audio source name specified!\n");
    }
    String sn(sourcename);
    map_t::const_iterator i = mSources.find(sn);
    if (i == mSources.end())
        return true;

    ALenum error = AL_NO_ERROR;
    ALuint source = (*i).second;
    ALint  state = AL_PLAYING;
    alGetSourcei(source, AL_SOURCE_STATE, &state);
    return state != AL_PLAYING;
}

void Audio::ReleaseSource(const char *sourcename)
{  
    if (!sourcename)
    {
        printf("[Audio::ReleaseSource] Null audio source name specified!\n");
    }
    String sn(sourcename);
    map_t::iterator i = mSources.find(sn);
    if (i != mSources.end())
    {
        ALenum error = AL_NO_ERROR;
        ALuint source = (*i).second;
        alSourcei(source, AL_BUFFER, 0),
        alDeleteSources(1, &source);
        mSources.erase(sourcename);
    }
}

#endif