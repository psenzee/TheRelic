#ifdef __cplusplus
extern "C" {
#endif
    
void SoundManager_PlaySound(const char *sound, float gain, float x, float y, float z);
void SoundManager_PlaySoundAtOrigin(const char *sound, float gain);
    
void SoundManager_PlayTheme(const char *filename, float gain);
void SoundManager_SetThemeVolume(float volume);
void SoundManager_StopTheme();

void SoundManager_Update();

void SoundManager_SetAmbient(const char *filename, int index);
void SoundManager_SetAmbientVolume(float volume);
void SoundManager_StartAmbient();

#ifdef __cplusplus
}

#ifndef _SOUNDMANAGER_H
#define _SOUNDMANAGER_H

#include "Audio.h"

#include "core/core.h"
#include "core/random.h"
#include "math/Filters.h"

class ScatterNoise
{
public:

    ScatterNoise(Audio &audio, float distance);

    inline void SetSourceName(const char *source)
    {
        mSourceName = source;
    }
    inline void SetListenerPosition(const Vector3 &pos)
    { 
        mListener = pos;
    }

    void Update();

public:

    static int    sSeedValue;

    core::Random  mRandom;
    Audio        &mAudio;
    FilterValue3  mPosition;
    Vector3       mTargetPosition;
    Vector3       mListener;
    int           mRepositionFrame;
    float         mDistance;
    String        mSourceName;
};

class SoundManager
{
public:

    static void           CreateInstance()   { if (!sInstance) sInstance = new SoundManager(); }
    static SoundManager  *GetInstance()      { return sInstance; }
    static void           DestroyInstance()  { if (sInstance) delete sInstance; sInstance = 0; }

    void Load();
    void Load(const char *filename, const char *soundname);

    void Play(const char *sound, const char *source, float gain, bool loop);
    void PlayAt(const char *sound, const Vector3 &position, float gain);
    void SetListenerPosition(const Vector3 &v);

    void LoadAmbient(const char *filename, int index /* 0 or 1 */);
    void SetAmbientVolume(float value);
    void StartAmbient();
    void StopAmbient();

    void PlayTheme(const char *filename, float gain);
    void SetThemeVolume(float volume);
    void StopTheme();

    void Update();

private:

    static SoundManager *sInstance;

    SoundManager();
    ~SoundManager() { /* $TODO!$ */ }

    const char *GetAvailableSourceName() const;

    Audio        mAudio;
    ScatterNoise mAmbientNoise1,
                 mAmbientNoise2;
    float        mAmbientVolume;
    bool         mThemePlaying;
    bool         mAmbientSet1;
    bool         mAmbientSet2;
    bool         mAmbientStarted;
};

#endif // _SOUNDMANAGER_H
#endif // __cplusplus
