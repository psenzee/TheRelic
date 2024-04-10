#include "ParticleEffects.h"
#include "ParticleSystem.h"

#include "core/core.h"
#include "core/random.h"

#include "render/GLUtils.h"
#include "render/GLStates.h"
#include "render/GLIncludes.h"
#include "render/DeviceTexture.h"
#include "render/GraphicsDevice.h"
#include "render/OverheadCamera.h"
#include "render/Material.h"
#include "render/RenderContext.h"

// the following are for testing collision with particle effects
#include "map/Map.h" 
#include "game/GameState.h"
#include "game/Game.h"
#include "level/Level.h"
#include "GLAbstract.h"
#include "Timer.h"

#include <algorithm>
#include <iostream>

extern Game *GetGlobalGame();

static Map *GetGlobalMap(){ return GetGlobalGame()->GetLevel()->GetMap(); }

ParticleEffects::ParticleEffects() : mRenderer(0)
{
    mRenderer = new renderer_t();
}

void CloudUpdater(Particle &p, const Vector3 &position)
{
    p.position += p.velocity;
    p.alpha =  1.f - (p.time / p.expire);
    p.size  = (1.f - (p.time / p.expire)) * p._maxSize;
}

void CloudGenerator(Particle &p, const Vector3 &position)
{
    p.position   = position;
    p.velocity   = p._direction * (rand() * p._maxSpeed / (float)RAND_MAX);
    p.time       = 0.f;
    p.expire     = rand() * p._maxExpire / (float)RAND_MAX;
    if (p._isShadow)
        p.position.z += p._maxZ - 1;
    else
        p.position.z += rand() * p._maxZ / (float)RAND_MAX;
    p.alpha      =  1.f - (p.time / p.expire);
    p.size       = (1.f - (p.time / p.expire)) * p._maxSize;
}

void OnEndDestroy(ParticleSystem *ps, void *user)
{
    ParticleEffects *fx = reinterpret_cast<ParticleEffects *>(user);
    fx->Destroy(ps);
}

void CircleUpdater(Particle &p, const Vector3 &position)
{
    p.position += p.velocity;
    p.alpha = 1.f - (p.time / p.expire);
    p.size  = p._maxSize;
}

void CircleGenerator(Particle &p, const Vector3 &position)
{
    p.position   = position;
    p.velocity   = p._direction * p._maxSpeed;
    p.time       = 0.f;
    p.expire     = p._maxExpire;
    if (p._isShadow)
        p.position.z += p._maxZ - 1;
    else
        p.position.z += rand() * p._maxZ / (float)RAND_MAX;
    p.alpha      = 1.f - (p.time / p.expire);
    p.size       = p._maxSize;
}

struct MapAndParticleSystem
{
    Map            *map;
    ParticleSystem *effect;

    inline MapAndParticleSystem(ParticleSystem *ps) : map(GetGlobalMap()), effect(ps) {}
};

void CollisionUpdater(Particle &p, const Vector3 &position)
{
    if (p._user)
    {
        MapAndParticleSystem *map = reinterpret_cast<MapAndParticleSystem *>(p._user);
        Vector3 at;
        ICollidable::Classification classify = map->map->Collision(p.position + map->effect->GetPosition(), p.size, at);
        if (classify == ICollidable::CLASS_ON || classify == ICollidable::CLASS_IN) {
            // stop
            p.velocity = Vector3();
        } else {
            p.velocity = p._direction * p._maxSpeed;
        }
    }
    p.position += p.velocity;
    p.alpha = 1.f - (p.time / p.expire);
    p.size  = p._maxSize;
}

void CollisionGenerator(Particle &p, const Vector3 &position)
{
    p.position   = position;
    p.velocity   = p._direction * p._maxSpeed;
    p.time       = 0.f;
    p.expire     = p._maxExpire;
    p.position.z = p._isShadow ? (p._maxZ - 1) : (rand() * p._maxZ / (float)RAND_MAX);
    p.alpha      = 1.f - (p.time / p.expire);
    p.size       = p._maxSize;
}

ParticleEffects *ParticleEffects::mInstance;

ParticleSystem *ParticleEffects::CreateCircle(DeviceTexture *texture, const Vector3 &position, float time, float _speed, bool perpetual, bool autodestroy)
{
    ParticleSystem *ps = CreateCircleIndependent(texture, position, time, _speed, perpetual, autodestroy);
    mParticleSystems.push_back(ps);
    return ps;
}

ParticleSystem *ParticleEffects::CreateCircleIndependent(DeviceTexture *texture, const Vector3 &position, float time, float _speed, bool perpetual, bool autodestroy)
{
    ParticleSystem *ps = new ParticleSystem();
    ps->SetTexture(texture);

    for (float i = 0.f, to = math::PIf * 2.f + 0.1f; i <= to; i += 0.1f)
    {
        Particle p;
        float speed  = _speed;
        p.expire = time;
        p.velocity = Vector3(sin(i) * speed, cos(i) * speed, 0.f);
        p.position.z = rand() * -position.z / (float)RAND_MAX;
        p.uv1.x = 0.5f;
        p._maxSpeed  = _speed;
        p._maxExpire = time;
        p._maxZ      = -position.z;
        p._maxSize   = 75.0f;
        p._direction = Vector3(sin(i), cos(i), 0.f);
        p._isShadow  = false;
        ps->Add(p); // original
        p.uv0.x = 0.5f;
        p.uv1.x = 1.f;
        p.position.z = -position.z - 1;
        p._isShadow  = true;
        ps->Add(p); // shadow 
    }
    ps->SetPosition(position);
    ps->SetUpdater(CircleUpdater);
    if (perpetual)
        ps->SetGenerator(CircleGenerator);
    if (autodestroy)
        ps->SetOnEnd(OnEndDestroy, this);
    return ps;
}

ParticleSystem *ParticleEffects::CreateCollisionTest(DeviceTexture *texture, const Vector3 &position, float time, float _speed)
{
    ParticleSystem *ps = new ParticleSystem();
    ps->SetTexture(texture);
    MapAndParticleSystem *mapAndFx = new MapAndParticleSystem(ps);

    for (float i = 0.f, to = math::PIf * 2.f + 0.1f; i <= to; i += 0.025f)
    {
        Particle p;
        float speed  = _speed;
        p.expire = time;
        p.velocity = Vector3(sin(i) * speed, cos(i) * speed, 0.f);
        p.position.z = -100.f + position.z;
        p.uv1.x = 0.5f;
        p._user      = mapAndFx;
        p._maxSpeed  = _speed;
        p._maxExpire = time;
        p._maxZ      = -position.z;
        p._maxSize   = 75.0f;
        p._direction = Vector3(sin(i), cos(i), 0.f);
        p._isShadow  = false;
        ps->Add(p); // original
        p.uv0.x = 0.5f;
        p.uv1.x = 1.f;
        p.position.z = -position.z - 1;
        p._isShadow  = true;
        ps->Add(p); // shadow 
    }
    ps->SetPosition(position);
    ps->SetUpdater(CollisionUpdater);
    ps->SetGenerator(CollisionGenerator);
    mParticleSystems.push_back(ps);
    return ps;
}

// $TODO only render visible particles

ParticleSystem *ParticleEffects::CreateCloud(DeviceTexture *texture, const Vector3 &position, float time, float _speed, bool perpetual, bool autodestroy, bool light)
{
    ParticleSystem *ps = CreateCloudIndependent(texture, position, time, _speed, perpetual, autodestroy, light);
    mParticleSystems.push_back(ps);
    return ps;
}

ParticleSystem *ParticleEffects::CreateCloudIndependent(DeviceTexture *texture, const Vector3 &position, float time, float _speed, bool perpetual, bool autodestroy, bool light)
{
    ParticleSystem *ps = new ParticleSystem();
    ps->SetTexture(texture);
    if (light) {
        ps->SetBlendType(ParticleSystem::PARTICLE_BLEND_LIGHT);
    }
    for (float i = 0.f, to = math::PIf * 2.f + 0.1f; i <= to; i += 0.1f)
    {
        Particle p;
        p.uv0.x = 0.0f;
        p.uv1.x = 0.5f;
        p._maxSpeed  = _speed;
        p._maxExpire = time;
        
        p._maxZ      = -position.z;
        p._maxSize   = 75.0f;
        p._direction = Vector3(sin(i), cos(i), 0.f);
        CloudGenerator(p, position);
        p._isShadow  = false;
        ps->Add(p); // original
        p.uv0.x = 0.5f;
        p.uv1.x = 1.f;
        p._isShadow  = true;
        CloudGenerator(p, position);
        ps->Add(p); // shadow
    }
    ps->SetPosition(position);
    ps->SetUpdater(CloudUpdater);
    if (perpetual) {
        ps->SetGenerator(CloudGenerator);
    }
    if (autodestroy) {
        ps->SetOnEnd(OnEndDestroy, this);
    }
    return ps;
}

void ParticleEffects::Destroy(ParticleSystem *ps)
{
    mParticleSystems.erase(
        std::remove(mParticleSystems.begin(), mParticleSystems.end(), ps), mParticleSystems.end());
    delete ps;
}

void ParticleEffects::DeferredDestroy(ParticleSystem *ps)
{
    // here we destroy the particle system by turning off its Generator and setting its OnEnd function,
    // then letting it run its course
    if (ps)
    {
        ps->SetGenerator(0);
        ps->SetOnEnd(OnEndDestroy, this);
    }
}

int ParticleEffects::Render(RenderContext &context, const GameTime &time)
{
    int count = 0;
    mRenderer->clear();
    std::vector<ParticleSystem *> fx(mParticleSystems); // make a copy, because we're going to delete from the original
    append(std::span<ParticleSystem *>(mDeferred.data(), mDeferred.size()), fx);
    for (std::vector<ParticleSystem *>::iterator i = fx.begin(), e = fx.end(); i != e; ++i) {
        //count += (*i)->Render(context, time);
        count += (*i)->prepare(*mRenderer, time);
    }
    mDeferred.clear();
    mRenderer->prepare();
    mRenderer->render(context);
    return count;
}
