#include "SoundManager.h"
#include "OpenALSupport.h"
#include "Audio.h"

#include "core/random.h"
#include "math/Filters.h"

#ifdef __APPLE__
#include "platform/Apple/SoundEngine.h"
#endif

extern "C" void SoundManager_SetAmbient(const char *filename, int index)
{
    printf("LOAD AMBIENT '%s'\n", filename);
    SoundManager *sm = SoundManager::GetInstance();
    if (!sm)
    {
        printf("Error: SoundManager not created\n");
        return;
    }
    sm->LoadAmbient(filename, index);
}

extern "C" void SoundManager_SetAmbientVolume(float volume)
{
    printf("SET AMBIENT VOLUME '%.2f'\n", volume);
    SoundManager *sm = SoundManager::GetInstance();
    if (!sm)
    {
        printf("Error: SoundManager not created\n");
        return;
    }
    sm->SetAmbientVolume(volume);
}

extern "C" void SoundManager_SetThemeVolume(float volume)
{
    printf("SET THEME VOLUME '%.2f'\n", volume);
    SoundManager *sm = SoundManager::GetInstance();
    if (!sm)
    {
        printf("Error: SoundManager not created\n");
        return;
    }
    sm->SetThemeVolume(volume);
}

extern "C" void SoundManager_StopTheme()
{
    printf("STOP THEME\n");
    SoundManager *sm = SoundManager::GetInstance();
    if (!sm)
    {
        printf("Error: SoundManager not created\n");
        return;
    }
    sm->StopTheme();
}

extern "C" void SoundManager_StartAmbient()
{
    printf("STARTING AMBIENT\n");
    SoundManager *sm = SoundManager::GetInstance();
    if (!sm)
    {
        printf("Error: SoundManager not created\n");
        return;
    }
    sm->StartAmbient();
}

extern "C" void SoundManager_PlaySound(const char *sound, float gain, float x, float y, float z)
{
    printf("PLAYING '%s'\n", sound);
    SoundManager *sm = SoundManager::GetInstance();
    if (!sm)
    {
        printf("Error: SoundManager not created\n");
        return;
    }
    sm->PlayAt(sound, Tuple3f(x, y, z), gain);
}

extern "C" void SoundManager_PlaySoundAtOrigin(const char *sound, float gain)
{
    SoundManager_PlaySound(sound, gain, 0.0, 0.0, 0.0);
}

extern "C" void SoundManager_PlayTheme(const char *filename, float gain)
{
    printf("PLAYING '%s'\n", filename);
    SoundManager *sm = SoundManager::GetInstance();
    if (!sm)
    {
        printf("Error: SoundManager not created\n");
        return;
    }
    sm->PlayTheme(filename, gain);
}

extern "C" void SoundManager_Update()
{
    SoundManager *sm = SoundManager::GetInstance();
    if (sm)
        sm->Update();
}

SoundManager *SoundManager::sInstance = 0;

// http://www.devmaster.net/articles/openal-tutorials/lesson1.php
// http://www.edenwaith.com/products/pige/tutorials/openal.php
// http://connect.creativelabs.com/openal/default.aspx
/*
extern "C" const char *globalTranslatePath(const char *path)
{
    return path;
}
*/

ScatterNoise::ScatterNoise(Audio &audio, float distance) 
    : mAudio(audio), mDistance(distance), mRepositionFrame(0), mRandom(++sSeedValue), mPosition(0.05f)
{
}

void ScatterNoise::Update()
{
    if (mRepositionFrame <= 0)
    {
        mTargetPosition = Vector3(0.1f * ((mRandom.realf() - 0.5f) * 2.0f * mDistance) + mListener.x,
                                  0.1f * ((mRandom.realf() - 0.5f) * 2.0f * mDistance) + mListener.y,
                                    mListener.z);
        mRepositionFrame = mRandom.integer() % 100;
    }
    mAudio.SetSourcePosition(mSourceName.c_str(), mPosition.Get());
    mPosition.Input(mTargetPosition);
    mRepositionFrame--;
}

int ScatterNoise::sSeedValue;

SoundManager::SoundManager() : mAudio(), mAmbientNoise1(mAudio, 100.f), mAmbientNoise2(mAudio, 100.f), mAmbientVolume(1.f),
                               mThemePlaying(false), mAmbientSet1(false), mAmbientSet2(false), mAmbientStarted(false)
{
#ifdef __APPLE__
//	SoundEngine_Initialize(0.f);
#endif
}

void SoundManager::Load()
{
    Load("drips11k8m.wav",                        "Drips");
    Load("creature_footstep_large_var_01_01.wav", "LargeFootstep");
    Load("body_fall_dirt_01.wav",                 "Fall1");
    Load("body_hit_big_03.wav",                   "Hit1");
    Load("airwhoosh03.wav",                       "Whoosh1");
    Load("bone_break11.wav",                      "BoneBreak");

    mAudio.SetSource("Drips", "Ambient1");
    mAudio.SetSource("Drips", "Ambient2");
}

void SoundManager::Load(const char *filename, const char *soundname)
{
    mAudio.Load(filename, soundname);
}

void SoundManager::LoadAmbient(const char *filename, int index /* 0 or 1 */)
{
    char source[32];
    mAmbientSet1 |= (index == 0);
    mAmbientSet2 |= (index == 1);
    sprintf(source, "Ambient%d", index + 1);
    Load(filename, filename);
    mAudio.SetSource(filename, source);
}

void SoundManager::SetAmbientVolume(float value)
{
    mAmbientVolume = value;
    if (mAmbientStarted)
    {
        if (mAmbientSet1)
            mAudio.SetSourceVolume("Ambient1", value);
        if (mAmbientSet2)
            mAudio.SetSourceVolume("Ambient2", value);
    }
}

void SoundManager::Play(const char *sound, const char *source, float gain, bool loop)
{
    if (strcmp(sound, "#vibrate") == 0)
    {
        //mAudio.Vibrate();
        return;
    }
    mAudio.SetSource(sound, source);
    mAudio.Play(source, gain, loop);
}

void SoundManager::PlayAt(const char *sound, const Vector3 &position, float gain)
{
    if (strcmp(sound, "#vibrate") == 0)
    {
        //mAudio.Vibrate();
        return;
    }	
    const char *available = GetAvailableSourceName();
    mAudio.SetSource(sound, available);
    mAudio.SetSourcePosition(available, position);
    mAudio.Play(available, gain, false);
}

void SoundManager::PlayTheme(const char *filename, float gain) // this will possibly have a radically different implementation for iPhone & PC
{
#ifndef __APPLE__
    const char *source = "theme";
    // the pc implementation
    if (!mAudio.IsLoaded(filename))
    {
        mAudio.Load(filename, filename); // use the filename as the sound name
    }
    mAudio.SetSource(filename, source); // use the sound name as the source name
    mAudio.Play(source, gain, true);
#else // iPhone
    StopTheme();
    SoundEngine_LoadBackgroundMusicTrack(filename, true, false);
    SoundEngine_SetBackgroundMusicVolume(gain);	
    SoundEngine_StartBackgroundMusic();
    mThemePlaying = true;
#endif
}

void SoundManager::SetThemeVolume(float volume)
{
    SoundEngine_SetBackgroundMusicVolume(volume);
}

void SoundManager::StopTheme()
{
#ifndef __APPLE__
    // .. implement for PC
#else
    if (mThemePlaying)
    {
        SoundEngine_StopBackgroundMusic(false);
        SoundEngine_UnloadBackgroundMusicTrack();
    }
	mThemePlaying = false;
#endif
}

void SoundManager::SetListenerPosition(const Vector3 &v)
{
    mAudio.SetListenerPosition(v);
    mAmbientNoise1.SetListenerPosition(v);
    mAmbientNoise2.SetListenerPosition(v);
}

void SoundManager::StartAmbient()
{
    if (!mAmbientStarted)
    {
        if (mAmbientSet1)
        {
            mAudio.SetSourceSecondsOffset("Ambient1", 3.f);
            mAmbientNoise1.SetSourceName("Ambient1");
            mAudio.Play("Ambient1", mAmbientVolume, true);
        }
        if (mAmbientSet2)
        {
            mAmbientNoise2.SetSourceName("Ambient2");
            mAudio.Play("Ambient2", mAmbientVolume, true);
        }
        mAmbientStarted = true;
    }
}

void SoundManager::StopAmbient()
{
    if (mAmbientStarted)
    {
        mAmbientStarted = false;
        if (mAmbientSet1)
            mAudio.Stop("Ambient1");
        if (mAmbientSet2)
            mAudio.Stop("Ambient2");
    }
}

void SoundManager::Update()
{
    if (mAmbientStarted)
    {
        if (mAmbientSet1)
            mAmbientNoise1.Update();
        if (mAmbientSet2)
            mAmbientNoise2.Update();
    }
}

const char *SoundManager::GetAvailableSourceName() const
{
    static char name[8];
    static unsigned times[8]; // doing these as function static is a hack
    static int count = 0;
    if (count == 0)
        memset(times, 0xff, sizeof(times));
    for (int i = 0; i < 8; i++)
    {
        sprintf(name, "#%d", i);
        if (mAudio.IsSourceDone(name))
        {
            times[i] = count++;
            return name;
        }
    }
    memset(name, 0, sizeof(name));
    int earliest = ~0u, earlyIndex = 0;
    for (int i = 0; i < 8; i++)
    {
        if (times[i] < earliest)
        {
            earliest = times[i];
            earlyIndex = i;
        }
    }
    sprintf(name, "#%d", earlyIndex);
    times[earlyIndex] = count++;
    return name;
}

inline float MapToRange(float t, float lo, float hi)
{
    float d = hi - lo;
    return t * d + lo;
}

Vector3 GetRandomVector3(core::Random &r, const Vector3 &lo, const Vector3 &hi)
{
    return Vector3(MapToRange(r.realf(), lo.x, hi.x),
                   MapToRange(r.realf(), lo.y, hi.y),
                   MapToRange(r.realf(), lo.z, hi.z));
}
