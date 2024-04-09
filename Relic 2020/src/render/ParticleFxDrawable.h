#pragma once

#include "core/core.h"
#include "core/aabox.h"
#include "core/gametime.h"

#include "IDrawable.h"
#include "ParticleEffects.h"
#include "fast/Allocator.h"

class RenderContext;
class Texture;


class ParticleFxDrawable : public IDrawable
{
public:
    
    typedef ImposterRenderer<CommonVertex> renderer_t;

    CLASS_NEW_DELETE()

    ParticleFxDrawable(ParticleSystem *fx, renderer_t &ir);
    ~ParticleFxDrawable();

    void         RenderImmediate(RenderContext &context);
    bool         IsVisible(const RenderContext &context) const;
    const AABox &GetBounds() const;

    bool         SetProperty(const char *, const char *) { return false; }

    void         Retain();
    void         Release();

    static void  SetGameTime(const GameTime &time) { sTime = time; }

private:

    static GameTime  sTime;

    ParticleSystem   *mFx;
    renderer_t       &mImposterRenderer;
    int               mCount;
    AABox             mBounds;
    int               mRef;
};
