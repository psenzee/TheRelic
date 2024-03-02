#ifndef _PARTICLESYSTEMEFFECT_H
#define _PARTICLESYSTEMEFFECT_H

#include "core/core.h"
#include "IEffect.h"
#include "fast/Allocator.h"

class ParticleSystem;

class ParticleSystemEffect : public IEffect
{
public:

    CLASS_NEW_DELETE()

    ParticleSystemEffect(ParticleSystem *particles, float atZ);
    ~ParticleSystemEffect();

    Type GetType() const                  { return IEffect::FX_PARTICLE; }
    void SetPosition(const Vector3 &p);
    void Update(const GameTime &time);
    void Complete();
    bool IsComplete() const;
    void Render(RenderContext &context);

private:

    enum { COMPLETE_FRAMES = 20 };

    ParticleSystem *mParticleSystem;
    int             mCompleteFrames;
    GameTime        mTime;
    float           mAtZ;
};

#endif // _PARTICLESYSTEMEFFECT_H