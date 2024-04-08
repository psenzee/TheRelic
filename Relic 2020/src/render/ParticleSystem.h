#pragma once

#include "core/core.h"
#include "fast/Allocator.h"
#include "Particle.h"
#include "DynamicBuffer.h"
#include "ImposterRenderer.h"

class RenderContext;
class DeviceTexture;

class IParticleController;

class ParticleSystem
{
public:
    
    CLASS_NEW_DELETE()
    
    enum ParticleBlendType { PARTICLE_BLEND_LIGHT = 0, PARTICLE_BLEND_DARK = 1 };

    typedef void (*ParticleFunction)(Particle &, const Vector3 &position);
    typedef void (*OnEndFunction)(ParticleSystem *, void *);

    ParticleSystem();
    ~ParticleSystem();

    void           SetPosition(const Vector3 &p)                       { mPosition = p; }
    const Vector3 &GetPosition()                                 const { return mPosition; }
    void           Clear()                                             { mParticles.clear(); }
    
    void           SetUpdater(ParticleFunction f)                      { mUpdater = f; }
    void           SetOnEnd(OnEndFunction f, void *user)               { mOnEnd = f; mOnEndUser = user; }
    void           SetGenerator(ParticleFunction f)                    { mGenerator = f; }
    
    void           SetController(IParticleController *controller)      { mController = controller; }

    void           SetTexture(DeviceTexture *t)                        { mTexture = t; }
    void           SetColor(const Vector4 &c)                          { mColor = c; }
    void           SetBlendType(ParticleBlendType type)                { mBlendType = type; }
    
    void           Add(const Particle &p);
    int            prepare(ImposterRenderer &ir, const GameTime &time);

private:

    void insertImposters(ImposterRenderer &ir);
    int  UpdateParticles(float ms);
    void UpdateOrder();

    std::vector<Particle>::iterator FindFirstDeadParticle();

    std::vector<Particle>  mParticles;
    ParticleFunction       mUpdater,
                           mGenerator;
    OnEndFunction          mOnEnd;
    void                  *mOnEndUser;
    Vector3                mPosition;
    Vector4                mColor;
    DeviceTexture         *mTexture;
    ParticleBlendType      mBlendType;
    bool                   mNeedsReorder;
    IParticleController   *mController;
};
