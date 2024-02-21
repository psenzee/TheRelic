/*
 
 File: OpenALSupport.h
 
 Abstract: OpenAL-related functions
 
 Version: 1.0
 
 Disclaimer: IMPORTANT:  This Apple software is supplied to you by
 Apple Inc. ("Apple") in consideration of your agreement to the
 following terms, and your use, installation, modification or
 redistribution of this Apple software constitutes acceptance of these
 terms.  If you do not agree with these terms, please do not use,
 install, modify or redistribute this Apple software.
 
 In consideration of your agreement to abide by the following terms, and
 subject to these terms, Apple grants you a personal, non-exclusive
 license, under Apple's copyrights in this original Apple software (the
 "Apple Software"), to use, reproduce, modify and redistribute the Apple
 Software, with or without modifications, in source and/or binary forms;
 provided that if you redistribute the Apple Software in its entirety and
 without modifications, you must retain this notice and the following
 text and disclaimers in all such redistributions of the Apple Software.
 Neither the name, trademarks, service marks or logos of Apple Inc.
 may be used to endorse or promote products derived from the Apple
 Software without specific prior written permission from Apple.  Except
 as expressly stated in this notice, no other rights or licenses, express
 or implied, are granted by Apple herein, including but not limited to
 any patent rights that may be infringed by your derivative works or by
 other works in which the Apple Software may be incorporated.
 
 The Apple Software is provided by Apple on an "AS IS" basis.  APPLE
 MAKES NO WARRANTIES, EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION
 THE IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY AND FITNESS
 FOR A PARTICULAR PURPOSE, REGARDING THE APPLE SOFTWARE OR ITS USE AND
 OPERATION ALONE OR IN COMBINATION WITH YOUR PRODUCTS.
 
 IN NO EVENT SHALL APPLE BE LIABLE FOR ANY SPECIAL, INDIRECT, INCIDENTAL
 OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 INTERRUPTION) ARISING IN ANY WAY OUT OF THE USE, REPRODUCTION,
 MODIFICATION AND/OR DISTRIBUTION OF THE APPLE SOFTWARE, HOWEVER CAUSED
 AND WHETHER UNDER THEORY OF CONTRACT, TORT (INCLUDING NEGLIGENCE),
 STRICT LIABILITY OR OTHERWISE, EVEN IF APPLE HAS BEEN ADVISED OF THE
 POSSIBILITY OF SUCH DAMAGE.
 
 Copyright (C) 2008 Apple Inc. All Rights Reserved.
 
 */

#include "OpenALSupport.h"

#import <OpenAL/al.h>
#import <OpenAL/alc.h>
#import <AudioToolbox/AudioToolbox.h>

ALvoid alBufferDataStaticProc(const ALint bid, ALenum format, ALvoid *data, ALsizei size, ALsizei freq)
{
    static alBufferDataStaticProcPtr proc = NULL;
    
    if (!proc)
        proc = (alBufferDataStaticProcPtr) alcGetProcAddress(NULL, (const ALCchar*) "alBufferDataStatic");
    
    if (proc)
        proc(bid, format, data, size, freq);
    
    return;
}

void *GetOpenALAudioData(const char *filename, ALsizei *outDataSize, ALenum *outDataFormat, ALsizei *outSampleRate)
{
    OSStatus                        err = noErr;    
    UInt64                            fileDataSize = 0;
    AudioStreamBasicDescription        theFileFormat;
    UInt32                            thePropertySize = sizeof(theFileFormat);
    AudioFileID                        afid = 0;
    void*                            theData = NULL;
    
    // Open a file with ExtAudioFileOpen()
    CFURLRef inFileURL = CFURLCreateFromFileSystemRepresentation(0, (const UInt8 *)filename, strlen(filename) + 1, false);
    err = AudioFileOpenURL(inFileURL, kAudioFileReadPermission, 0, &afid);
    if(err) { printf("GetOpenALAudioData: AudioFileOpenURL FAILED, Error = %ld\n", err); goto Exit; }
    
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
    UInt32        dataSize = fileDataSize;
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

