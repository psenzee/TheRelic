#include "CircleEffect.h"

#include "core/core.h"
#include "core/gametime.h"
#include "render/Drawable.h"
#include "render/DrawableUtils.h"
#include "render/RenderContext.h"

CircleEffect::CircleEffect(const char *filename, float rotate, float innerRadius, float innerRadiusDelta, float outerRadius, float outerRadiusDelta, float atZ) 
    : mDrawable(0), mAngle(0.f), mFilename(filename), mRotate(rotate),
     mInnerRadius(innerRadius), mOuterRadius(outerRadius), mInnerRadiusDelta(innerRadiusDelta), mOuterRadiusDelta(outerRadiusDelta), mAtZ(atZ), mCompleteFrames(-1)
{ 
}

CircleEffect::~CircleEffect()
{
    if (mDrawable)
        mDrawable->Release();
    mDrawable = 0;
}

void CircleEffect::Update(const GameTime &time)
{
    if (mCompleteFrames != 0)
    {
        mAngle += mRotate;
        mInnerRadius += mInnerRadiusDelta;
        mOuterRadius += mOuterRadiusDelta;
        if (mDrawable)
            mDrawable->Release();
        mDrawable = CreateRing("fx", mFilename.c_str(), 32, mInnerRadius, mOuterRadius, mAtZ);
        if (mDrawable->GetStates() && mCompleteFrames != -1)
            mDrawable->GetStates()->Color.w = mCompleteFrames / float(COMPLETE_FRAMES);
        if (mCompleteFrames > 0)
            mCompleteFrames--;
    }
}

void CircleEffect::Complete()
{
    if (mCompleteFrames == -1)
        mCompleteFrames = COMPLETE_FRAMES;
}

bool CircleEffect::IsComplete() const
{
    return (mInnerRadius >= 512.f || mCompleteFrames == 0);
}

void CircleEffect::Render(RenderContext &context)
{
    if (mDrawable)
    {
        Matrix rotation;
        rotation.rotationz(mAngle);
        RenderContext rc(context);
        rc.transform = rotation * rc.transform;
        Matrix translatez;
        translatez.translate(Vector3(0.f, 0.f, -70.f)); // wo, watch out hard coded!
        rc.transform = rc.transform * translatez;
        mDrawable->Render(rc);
    }
}
