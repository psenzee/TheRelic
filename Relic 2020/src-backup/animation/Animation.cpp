#include "Animation.h"

#include "xml/XmlReadContext.h"
#include "render/OverheadCamera.h"
#include "gamecore/IGameObject.h"
#include "gamecore/IRenderable.h"
#include "gamecore/IObjectList.h"
#include "gamecore/IEventList.h"
#include "events/Event.h"
#include "render/Material.h"
#include "render/GraphicsDevice.h"
#include "render/RenderContext.h"
#include "core/strs.h"
            
Animation::Animation() : frames(0), frame(0), elapsed(0), mMotionBlur(false),
                         frameTimeMs(33), loop(true), ended(false), mTagCallback(0), mTagCallbackUser(0), mAlpha(1.0f)
{
}

Animation::~Animation()
{
    if (frames)
        frames->Release();
}

void Animation::Load(XmlReadContext *context, const char *type)
{
    frames = context->objects[type];
    if (!frames)
    {
// $todo this should be put back in with a debug option
//      printf("** UNABLE TO LOAD ANIMATION %s **\n", type);
        return;
    }
    frames->Retain();
}

void Animation::Update(const GameTime &time)
{
    elapsed += time.elapsed;
    SetFrame(frame + elapsed / frameTimeMs);
    elapsed %= frameTimeMs;
}

ICollidable *Animation::GetCollidable()
{
    return frames->GetCollidable();
}

static void Draw(IGameObject *object, RenderContext &context, const Matrix &m, float alpha)
{
    if (object)
    {
        RenderContext rc(context);
        rc.transform = m;
        rc.transparent = alpha < 0.9f;
        //rc.color *= alpha;
        rc.color.w *= alpha;
        object->GetRenderable().Render(rc);
    }
}
/*
static void DrawFrame(IGameObject *object, int frame, RenderContext &context, const Matrix &m, float alpha)
{
    if (object)
        Draw(object->GetObjectList().GetObjects()[frame], context, m, alpha);
}
*/
void Animation::Render(RenderContext &context, const Matrix &transform)
{
    Draw(GetFrame(), context, transform, mAlpha);
    if (mMotionBlur)
    {
        int motionBlurFrames = 10;
        float maxAlpha = 0.01f;
        Matrix blurTranslate;
        blurTranslate.translate(context.camera.GetViewVector() * 10.f);
        Matrix blurTransform(transform * blurTranslate);
        RenderContext rc(context);
        rc.depthWrite = false;
        rc.blur = true;
        rc.color.x = rc.color.y = 0.3f;
        for (int i = 1; i < motionBlurFrames; i++)
        {
            if (HasPrevFrame(i))
            {
                rc.color.w = maxAlpha - ((i / (float)motionBlurFrames) * maxAlpha);
                Draw(GetPrevFrame(i), rc, blurTransform, mAlpha);
            }
        }
    }
}

IGameObject *Animation::GetFrame()
{
    return GetFrame(frames);
}

int GetTagIntArgument(const char *data, const char *name)
{
    if (!starts(data, name))
        return -1;
    data += static_cast<int>(strlen(name));
    int value = 0;
    if (read(&data, value))
        return value;
    printf("Animation: expected n.umber after '%s' tag!\n", name);
    return -1;
}

bool GetTagStringArgument(const char *data, const char *name, char *buffer)
{
    if (!starts(data, name))
        return false;
    data += static_cast<int>(strlen(name));
    if (read_quoted(&data, '"', buffer))
        return true;
    printf("Animation: expected quoted (\") string after '%s' tag!\n", name);
    return false;
}

void Animation::SetActive()
{
    ProcessTags(frames);
}

void Animation::ProcessTags(IGameObject *object)
{
    if (!object)
        return;
    IEventList &events = object->GetEventList();
    int count = events.GetEventCount();
    if (count)
    {
        Event **list = events.GetEvents();
        for (int i = 0; i < count; i++)
            if (list[i])
                ProcessTag(list[i]->text.c_str());
    }
}

bool Animation::ProcessTag(const char *data)
{
    if (*data == '$')
    {
        if      (starts(data, "$Reset"))
        {
            Reset();
        }
        else if (starts(data, "$GoToFrame"))
        {
            SetFrame(GetTagIntArgument(data, "$GoToFrame"));
        }
        else if (starts(data, "$Loop"))
        {
            SetLoopMode(true);
        }
        else if (starts(data, "$NoLoop"))
        {
            SetLoopMode(false);
        }
        else if (starts(data, "$MotionBlur"))
        {
            SetMotionBlur(true);
        }
        else if (starts(data, "$NoMotionBlur"))
        {
            SetMotionBlur(false);
        }
        else if (starts(data, "$FrameTime"))
        {
            SetFrameTime(GetTagIntArgument(data, "$FrameTime"));
        }
        else return false;
        return true;
    }
    else if (mTagCallback)
        return mTagCallback(mTagCallbackUser, data);
    return false;
}

int Animation::GetFrameCount(IGameObject *list)
{ 
    return list ? list->GetObjectList().GetObjectCount() : 0;
}

void Animation::Internal_SetFrame(int f)
{
    if (f == frame)
        return;
    if (f > frame)
    {
        for (int i = frame + 1; i < f; i++)
            ProcessTags(GetFrame(frames, i));    
    }
    else if (f < frame)
    {
        for (int i = frame + 1, count = GetFrameCount(); i < count; i++)
            ProcessTags(GetFrame(frames, i));
        for (int i = 0; i < f; i++)
            ProcessTags(GetFrame(frames, i));
    }
    frame = f;
    ProcessTags(GetFrame(frames));
}

void Animation::SetFrame(int f)
{ 
    if (f == frame)
        return;
    int count = GetFrameCount();
    if (!count)
    {
        frame = 0;
        return;
    }
    if (ended)
        return;
    if (loop)
        Internal_SetFrame(!count ? 0 : (f % count));
    else if (f < count)
        Internal_SetFrame(f);
    else
    {
        Internal_SetFrame(count - 1);
        ended = true;
    }
}

IGameObject *Animation::GetPrevFrame(int offset)
{
    int prev = frame - offset;
    return GetFrame(frames, prev < 0 ? 0 : prev);
}

IGameObject *Animation::GetFrame(IGameObject *list)
{
    return GetFrame(list, frame);
}

IGameObject *Animation::GetFrame(IGameObject *list, int frame)
{
    if (list && list->GetObjectList().GetObjectCount())
        return list->GetObjectList().GetObjects()[frame % list->GetObjectList().GetObjectCount()];
    return 0;
}
