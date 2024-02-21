#ifndef _OPENALSUPPORT_H
#define _OPENALSUPPORT_H

#include <stdio.h>
#include <stdlib.h>

#ifdef WIN32
#include <al.h>
#include <alc.h>
#else
#import <OpenAL/al.h>
#import <OpenAL/alc.h>
#import <AudioToolbox/AudioToolbox.h>
#endif

typedef ALvoid /*AL_APIENTRY */(*alBufferDataStaticProcPtr) (const ALint bid, ALenum format, ALvoid* data, ALsizei size, ALsizei freq);

ALvoid alBufferDataStaticProc(const ALint bid, ALenum format, ALvoid* data, ALsizei size, ALsizei freq);

void *GetOpenALAudioData(const char *filename, ALsizei *outDataSize, ALenum *outDataFormat, ALsizei *outSampleRate);

void ShutdownOpenAL();

#endif // _OPENALSUPPORT_H
