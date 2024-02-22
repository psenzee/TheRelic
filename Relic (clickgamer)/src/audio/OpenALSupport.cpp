#include "core/global.h"

#ifdef OPENAL

#include "OpenALSupport.h"

#include <stdio.h>
#include <stdlib.h>

#ifdef WIN32
#include <al.h>
#include <alc.h>
#include <alut.h>
#else
#import <OpenAL/al.h>
#import <OpenAL/alc.h>
#import <AudioToolbox/AudioToolbox.h>
#endif

ALvoid alBufferDataStaticProc(const ALint bid, ALenum format, ALvoid *data, ALsizei size, ALsizei freq)
{
    static alBufferDataStaticProcPtr proc = NULL;
    
    if (!proc)
        proc = (alBufferDataStaticProcPtr) alcGetProcAddress(NULL, (const ALCchar*) "alBufferDataStatic");
    
    if (proc)
        proc(bid, format, data, size, freq);
    
    return;
}

#ifdef WIN32
void *GetOpenALAudioData(const char *filename, ALsizei *outDataSize, ALenum *outDataFormat, ALsizei *outSampleRate)
{
    void *data = 0;
    ALboolean loop = false;
    alutLoadWAVFile((ALbyte *)filename, outDataFormat, &data, outDataSize, outSampleRate, &loop);
    return data;
}
#else
void *GetOpenALAudioData(const char *filename, ALsizei *outDataSize, ALenum *outDataFormat, ALsizei *outSampleRate)
{
    OSStatus                        err = noErr;    
    UInt64                            fileDataSize = 0;
    AudioStreamBasicDescription        theFileFormat;
    UInt32                            thePropertySize = sizeof(theFileFormat);
    AudioFileID                        afid = 0;
    void*                            theData = NULL;
    
    UInt32        dataSize = 0;
    
    // Open a file with ExtAudioFileOpen()
    CFURLRef inFileURL = CFURLCreateFromFileSystemRepresentation(0, (const UInt8 *)filename, strlen(filename) + 1, false);
    err = AudioFileOpenURL(inFileURL, kAudioFileReadPermission, 0, &afid);
    if(err) { printf("GetOpenALAudioData: AudioFileOpenURL FAILED '%s', Error = %ld\n", filename, err); goto Exit; }
    
    // Get the audio data format
    err = AudioFileGetProperty(afid, kAudioFilePropertyDataFormat, &thePropertySize, &theFileFormat);
    if(err) { printf("GetOpenALAudioData: AudioFileGetProperty(kAudioFileProperty_DataFormat) FAILED, Error = %ld\n", err); goto Exit; }
    
    if (theFileFormat.mChannelsPerFrame > 2)  { 
        printf("GetOpenALAudioData - Unsupported Format, channel count is greater than stereo\n"); goto Exit;
    }
    
    if ((theFileFormat.mFormatID != kAudioFormatLinearPCM) || (!TestAudioFormatNativeEndian(theFileFormat))) { 
        printf("GetOpenALAudioData - Unsupported Format, must be little-endian PCM\n"); goto Exit;
    }
    
    if ((theFileFormat.mBitsPerChannel != 8) && (theFileFormat.mBitsPerChannel != 16)) { 
        printf("GetOpenALAudioData - Unsupported Format, must be 8 or 16 bit PCM\n"); goto Exit;
    }
    
    thePropertySize = sizeof(fileDataSize);
    err = AudioFileGetProperty(afid, kAudioFilePropertyAudioDataByteCount, &thePropertySize, &fileDataSize);
    if(err) { printf("GetOpenALAudioData: AudioFileGetProperty(kAudioFilePropertyAudioDataByteCount) FAILED, Error = %ld\n", err); goto Exit; }
    
    // Read all the data into memory
    dataSize = fileDataSize;
    theData = malloc(dataSize);
    if (theData)
    {
        AudioFileReadBytes(afid, false, 0, &dataSize, theData);
        if(err == noErr)
        {
            // success
            *outDataSize = (ALsizei)dataSize;
            *outDataFormat = (theFileFormat.mChannelsPerFrame > 1) ? AL_FORMAT_STEREO16 : AL_FORMAT_MONO16;
            *outSampleRate = (ALsizei)theFileFormat.mSampleRate;
        }
        else 
        { 
            // failure
            free (theData);
            theData = NULL; // make sure to return NULL
            printf("GetOpenALAudioData: ExtAudioFileRead FAILED, Error = %ld\n", err); goto Exit;
        }    
    }
    
Exit:

    CFRelease(inFileURL);
    // Dispose the ExtAudioFileRef, it is no longer needed
    if (afid) AudioFileClose(afid);
    return theData;
}
#endif

void ShutdownOpenAL()
{
    ALCcontext    *context = NULL;
    ALCdevice    *device = NULL;
    ALuint        returnedName;
    
    // Delete the Sources
    alDeleteSources(1, &returnedName);
    // Delete the Buffers
    alDeleteBuffers(1, &returnedName);
    
    //Get active context
    context = alcGetCurrentContext();
    //Get device for active context
    device = alcGetContextsDevice(context);
    //Release context
    alcDestroyContext(context);
    //Close device
    alcCloseDevice(device);
}

#endif