#ifndef _PARTICLEEFFECTS_H
#define _PARTICLEEFFECTS_H

#include "core/core.h"
#include "fast/Allocator.h"

class RenderContext;
class DeviceTexture;

struct Particle
{
    Vector3                position,
                           velocity;
    Vector2                uv0,
                           uv1;
    float               // angle,
                           alpha,
                           size,
                           time,
                           expire;

    void                  *_user;

    float                  _maxSpeed,
                           _maxExpire,
                           _maxSize,
                           _maxZ;
    Vector3                _direction;
    bool                   _isShadow;

    inline Particle() : uv1(1.f, 1.f), alpha(1.f), size(10.f), time(0.0f), expire(1000.f), _maxSpeed(1.f), _maxExpire(1000.f), _maxSize(75.f), _maxZ(50.f), _isShadow(false), _user(0) {}
};

class ParticleSystem
{
public:
    
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

    enum { BUFFER_BYTES = 65536 };
};

void MoveUpdater(Particle &p);

class ParticleEffects
{
public:

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

    static void             CreateInstance()  { if (!mInstance) mInstance = new ParticleEffects; }
    static ParticleEffects *GetInstance()     { return mInstance; }
    static void             DestroyInstance() { if (mInstance) delete mInstance; mInstance = 0; }

private:

    static ParticleEffects *mInstance;

    std::vector<ParticleSystem *> mParticleSystems;
    std::vector<ParticleSystem *> mDeferred;
};

#endif // _PARTICLEEFFECTS_H
