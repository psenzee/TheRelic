#include "ParticleFxDrawable.h"
#include "ParticleSystem.h"

#include "core/core.h"
#include "core/aabox.h"

#include "IDrawable.h"
#include "ParticleEffects.h"
#include "RenderContext.h"
#include "GraphicsDevice.h"
#include "OverheadCamera.h"
#include "Visibility.h"

class RenderContext;
class Texture;

GameTime ParticleFxDrawable::sTime(0);

ParticleFxDrawable::ParticleFxDrawable(ParticleSystem *fx) : mFx(fx)
{
    AABox bounds;
    mBounds = bounds;
}

ParticleFxDrawable::~ParticleFxDrawable()
{
    delete mFx;
    mBounds = AABox();
}

void ParticleFxDrawable::RenderImmediate(RenderContext &context)
{
    mFx->Render(context, sTime);
}

bool ParticleFxDrawable::IsVisible(const RenderContext &context) const
{
    return Visibility::IsVisible(context.camera.GetViewProjection(), context.transform, mBounds);
}

const AABox &ParticleFxDrawable::GetBounds() const
{
    return mBounds;
}

void ParticleFxDrawable::Retain()
{
    mRef++;
}

void ParticleFxDrawable::Release() 
{
    if (!--mRef)
        delete this;
}
