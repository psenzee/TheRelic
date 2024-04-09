#pragma once

#include "core/core.h"
#include "fast/Allocator.h"
#include "ImposterRenderer.h"
#include "CommonVertex.h"

class RenderContext;
class DeviceTexture;
class ParticleSystem;

class ParticleEffects
{
public:
    
    typedef ImposterRenderer<CommonVertex> renderer_t;

    // create types of ParticleSystems here
    ParticleSystem         *CreateCloud(DeviceTexture *texture, const Vector3 &position, float time, float speed, bool perpetual, bool autodestroy, bool light);
    ParticleSystem         *CreateCloudIndependent(DeviceTexture *texture, const Vector3 &position, float time, float speed, bool perpetual, bool autodestroy, bool light);
    ParticleSystem         *CreateCircle(DeviceTexture *texture, const Vector3 &position, float time, float speed, bool perpetual, bool autodestroy);
    ParticleSystem         *CreateCircleIndependent(DeviceTexture *texture, const Vector3 &position, float time, float speed, bool perpetual, bool autodestroy);

    ParticleSystem         *CreateCollisionTest(DeviceTexture *texture, const Vector3 &position, float time, float speed);

    void                    DeferRender(ParticleSystem *ps) { mDeferred.push_back(ps); }
    void                    Destroy(ParticleSystem *ps);
    void                    DeferredDestroy(ParticleSystem *ps);

    int                     Render(RenderContext &context, const GameTime &time);
    
    renderer_t             *GetImposterRenderer() { return mImposterRenderer; }

    static void             CreateInstance()  { if (!mInstance) mInstance = new ParticleEffects; }
    static ParticleEffects *GetInstance()     { return mInstance; }
    static void             DestroyInstance() { if (mInstance) delete mInstance; mInstance = 0; }

private:

    ParticleEffects();
    
    static ParticleEffects *mInstance;
    
    renderer_t                    *mImposterRenderer;
    std::vector<ParticleSystem *>  mParticleSystems;
    std::vector<ParticleSystem *>  mDeferred;
};

