#ifndef _AUDIO_H
#define _AUDIO_H

#include "core/core.h"

#include <map>
#include <set>
#include <string>

class Audio
{
public:
        
    Audio();
    ~Audio();    
        
    bool Load(const char *filename, const char *soundname);
    bool IsLoaded(const char *filename) const;
    void Unload(const char *soundname);
    
    void SetListenerPosition(const Vector3 &pos);

    bool SetSource(const char *soundname, const char *sourcename);
    bool SetSourcePosition(const char *sourcename, const Vector3 &pos);
    void SetSourceSecondsOffset(const char *sourcename, float seconds);
    bool IsSourceDone(const char *sourcename) const;
    void ReleaseSource(const char *sourcename);
    void Vibrate();
    
    bool Play(const char *sourcename, float gain = 0.4f, bool loop = false);
  //bool Play(const char *sourcename, int ms, bool loop = false);
    bool Stop(const char *sourcename);

private:

    void  *device;  // void * = ALCdevice *
    void  *context; // void * = ALCcontext *
    
    typedef std::map<String, unsigned> map_t; // unsigned = ALuint
    
    map_t                        mBuffers;
    map_t                        mSources;
    std::set<String>             mLoaded;
};

#endif // _AUDIO_H