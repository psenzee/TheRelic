#ifndef _ANIMATION_H
#define _ANIMATION_H

#include "core/core.h"
#include "core/quaternion.h"
#include "time/GameClock.h"

class XmlElement;
class RenderContext;
class IGameObject;
class ICollidable;
class XmlReadContext;

class Animation
{
public:
    
    typedef bool (*TagCallback_fn)(void *user, const char *data);

    Animation();

    void         SetActive();

    void         Load(XmlReadContext *context, const char *type);
    void         Update(const GameTime &time);

    void         Render(RenderContext &context, const Matrix &transform);

    IGameObject *GetFrame();

    ICollidable *GetCollidable();

    inline void  Reset()                 { frame = 0; ended = false; }
    inline int   GetFrameNumber() const  { return frame; }
    inline int   GetFrameCount() const   { return GetFrameCount(frames); }

    void         SetFrame(int f);

    inline void  SetLoopMode(bool m)     { loop = m; }
    inline void  SetFrameTime(int ms)    { frameTimeMs = ms; }

    inline void  SetAlpha(float a)       { /* $TODO */ }

    inline void  SetTagCallback(TagCallback_fn callback, void *user)
    {
        mTagCallback     = callback;
        mTagCallbackUser = user;
    }
  
private:

    IGameObject        *GetFrame(IGameObject *list);
    bool                ProcessTag(const char *data);
    void                ProcessTags(IGameObject *object);

    void                Internal_SetFrame(int f);

    static int          GetFrameCount(IGameObject *list);

    static IGameObject *GetFrame(IGameObject *list, int frame);

    int                 frame;
    IGameObject        *frames;
    Matrix              transform;
    unsigned            elapsed;
    int                 frameTimeMs;
    bool                loop,
                        ended;

    TagCallback_fn      mTagCallback;
    void               *mTagCallbackUser;
};

int  GetTagIntArgument(const char *data, const char *name);
bool GetTagStringArgument(const char *data, const char *name, char *buffer);

#endif  // _ANIMATION_H