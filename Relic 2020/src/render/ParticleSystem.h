#pragma once

#include "core/core.h"
#include "fast/Allocator.h"
#include "Particle.h"
#include "DynamicBuffer.h"

class RenderContext;
class DeviceTexture;

class ParticleSystem
{
public:
    
    typedef DynamicBuffer<CommonVertex> dyn_buffer_t;
    
    CLASS_NEW_DELETE()
    
    enum ParticleBlendType { PARTICLE_BLEND_LIGHT = 0, PARTICLE_BLEND_DARK = 1 };

    typedef void (*ParticleFunction)(Particle &, const Vector3 &position);
    typedef void (*OnEndFunction)(ParticleSystem *, void *);

    ParticleSystem();
    ~ParticleSystem();

    void Add(const Particle &p);
    void SetPosition(const Vector3 &p);
    const Vector3 &GetPosition() const { return mPosition; }
    void SetTexture(DeviceTexture *t);
    void SetColor(const Vector4 &c);
    void SetBlendType(ParticleBlendType type) { mBlendType = type; }
    void Clear();

    void SetUpdater(ParticleFunction f);
    void SetGenerator(ParticleFunction f);
    void SetOnEnd(OnEndFunction f, void *user);

    int  Render(RenderContext &context, const GameTime &time);

private:

    int                             UpdateParticles(float ms);
    void                            UpdateOrder();

    std::vector<Particle>::iterator FindFirstDeadParticle();

    int                             RenderParticles(RenderContext &context);

    std::vector<Particle>  mParticles;
    ParticleFunction       mUpdater,
                           mGenerator;
    OnEndFunction          mOnEnd;
    void                  *mOnEndUser;
    Vector3                mPosition;
    Vector4                mColor;
    char                  *mBuffer;
    DeviceTexture         *mTexture;
    ParticleBlendType      mBlendType;
    bool                   mNeedsReorder;
    dyn_buffer_t           mDynamicBuffer;

    enum { BUFFER_BYTES = 65536 };
};
