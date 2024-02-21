#include "core/core.h"
#include "core/random.h"

#include "ParticleEffects.h"
#include "render/GLUtils.h"
#include "render/GLStates.h"
#include "platform/GLIncludes.h"
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

extern Game *GetGlobalGame();

static Map *GetGlobalMap(){ return GetGlobalGame()->GetLevel()->GetMap(); }

#include <algorithm>

struct ZComparator
{
    inline bool operator()(const Particle &a, const Particle &b) const
    {
        return a.position.z > b.position.z;
    }
};

extern void ClearCachedPointers();

inline static void _append(const Vector3 &pos, float u, float v, float **ppos, float **puv)
{
    *((Vector3  *)*ppos)   = pos;    (*ppos) += 3;
    *((float    *)*puv)    = u;      (*puv)++;
    *((float    *)*puv)    = v;      (*puv)++;
}

static void _RenderQuads(RenderContext &context, float *vertices, float *uvs, int count, const Vector4 &color)
{
    if (!count || !vertices || !uvs) return;
        
    // RENDER DRAW LIST
    Vector4 ambient(color * Vector4(2.0f, 2.0f, 2.0f, 0.0f)),
            diffuse(1.0f, 1.0f, 1.0f, color.w);

    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, (GLfloat *)&ambient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, (GLfloat *)&diffuse);
    
    glVertexPointer  (3, GL_FLOAT,         0, (GLfloat *)&vertices[0]);
    glTexCoordPointer(2, GL_FLOAT,         0, (GLfloat *)&uvs[0]);

    glDrawArrays(GL_TRIANGLES, 0, count);
}

ParticleSystem::ParticleSystem() : mUpdater(0), mGenerator(0), mBuffer(0), mColor(1.f, 1.f, 1.f, 1.f), mTexture(0), mNeedsReorder(false), mOnEnd(0), mOnEndUser(0)
{
    mBuffer = new char [BUFFER_BYTES];    
}

ParticleSystem::~ParticleSystem()
{
    delete [] mBuffer;
    mBuffer = 0;
}

void ParticleSystem::Add(const Particle &p)
{
    std::vector<Particle>::iterator i = FindFirstDeadParticle();
    if (i == mParticles.end())
    {
        if (mNeedsReorder) // if it needs sorting anyway, just stick it in the end
        {
            mParticles.push_back(p);
        }
        else // otherwise, preserve the ordering
        {
            ZComparator comparator;
            std::vector<Particle>::iterator i = std::upper_bound(mParticles.begin(), mParticles.end(), p, comparator);
            mParticles.insert(i, p);
        }
    }
    else
    {
        (*i) = p;
        mNeedsReorder = true;
    }
}

void ParticleSystem::SetPosition(const Vector3 &p)
{ 
    mPosition = p;
}

void ParticleSystem::Clear()
{ 
    mParticles.clear();
}

void ParticleSystem::SetUpdater(ParticleFunction f)
{
    mUpdater = f;
}

void ParticleSystem::SetOnEnd(OnEndFunction f, void *user)
{
    mOnEnd = f; mOnEndUser = user;
}

void ParticleSystem::SetGenerator(ParticleFunction f)
{
    mGenerator = f;
}

void ParticleSystem::SetTexture(DeviceTexture *t)
{
    mTexture = t;
}

void ParticleSystem::SetColor(const Vector4 &c)
{
    mColor = c;
}

int ParticleSystem::Render(RenderContext &context, const GameTime &time)
{
    UpdateOrder();
    if (!UpdateParticles((float)time.elapsed) && mOnEnd)
    {
        mOnEnd(this, mOnEndUser); // mOnEnd could delete -this-, don't do anything that requires -this-
                                  // after calling it
        return 0;
    }
    return RenderParticles(context);
}

void ParticleSystem::UpdateOrder()
{
    if (mNeedsReorder)
    {
        ZComparator comparator;
        std::sort(mParticles.begin(), mParticles.end(), comparator);
        mNeedsReorder = false;
    }
}

int ParticleSystem::UpdateParticles(float ms)
{
    if (!mUpdater)
        // if no updater is set, still keep it alive
        return 1; 
    int count = 0;
    for (std::vector<Particle>::iterator i = mParticles.begin(), e = mParticles.end(); i != e; ++i)
    {
        Particle &p = *i;
        if (p.time < p.expire) // not dead yet
        {
            p.time += ms;
            mUpdater(*i);
            count++;
        }
        else if (mGenerator) // dead - rebirth
        {
            mGenerator(p);
            count++;
        }
    }
    return count;
}

std::vector<Particle>::iterator ParticleSystem::FindFirstDeadParticle()
{
    for (std::vector<Particle>::iterator i = mParticles.begin(), e = mParticles.end(); i != e; ++i)
    {
        Particle &p = *i;
        if (p.time >= p.expire)
            return i;
    }
    return mParticles.end();
}


int ParticleSystem::RenderParticles(RenderContext &context)
{
    if (mColor.w < 0.01f || !mTexture) return 0;
    
    Vector3              position(mPosition);

    void                *buffer = mBuffer;
        
    static const Vector3 SCALE(-1.0f, -1.0f, 1.0f);
    static const Vector3 UL(Vector3(-0.5f, -0.5f, 0.0f) * SCALE), UR(Vector3( 0.5f, -0.5f, 0.0f) * SCALE),
                         LL(Vector3(-0.5f,  0.5f, 0.0f) * SCALE), LR(Vector3( 0.5f,  0.5f, 0.0f) * SCALE);
          
    enum { VERTICES_PER_QUAD = 6 }; // for now, non-indexed, non-stripped

    size_t               particles = mParticles.size(),
                         count     = particles * VERTICES_PER_QUAD,
                         bytes     = (count * 3 /* vertices */ + count * 2 /* uvs */) * sizeof(float);
    
    void                *data      = buffer;

    if (!buffer || bytes > BUFFER_BYTES)
    {
        // if no buffer is supplied or it's too small        
        buffer = 0;
        data   = new unsigned [bytes / sizeof(unsigned)]; // 1 single allocation
    }

    float               *vertices  = ((float *)data),
                        *pvertices = vertices,
                        *uvs       = ((float *)data) + count * 3,
                        *puvs      = uvs;
    unsigned             vcount    = 0;

    glLoadMatrixf((GLfloat *)context.camera.GetView().data);
    glMultMatrixf((GLfloat *)context.transform.data);

    ClearCachedPointers();
    mTexture->Set(context.device, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glDisableClientState(GL_NORMAL_ARRAY);  
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    glDisable(GL_CULL_FACE);  // we can eliminate this if we ensure the correct orientation of the vertices

    int renderedCount = 0;
    
    for (std::vector<Particle>::const_iterator i = mParticles.begin(), e = mParticles.end(); i != e; ++i)
    {
        const Particle &particle = *i;

        if (particle.time < particle.expire) // if not dead..
        {
            position = mPosition + particle.position;

            float u0   = particle.uv0.x, v0 = particle.uv0.y,
                  u1   = particle.uv1.x, v1 = particle.uv1.y,
                  size = particle.size;

            _append(position + UL * size, u0, v0, &pvertices, &puvs);
            _append(position + LL * size, u0, v1, &pvertices, &puvs);
            _append(position + LR * size, u1, v1, &pvertices, &puvs);
                
            _append(position + UL * size, u0, v0, &pvertices, &puvs);
            _append(position + LR * size, u1, v1, &pvertices, &puvs);
            _append(position + UR * size, u1, v0, &pvertices, &puvs);

            vcount += VERTICES_PER_QUAD;

            renderedCount++;
        }
    }

    GLStates::depthWrite.Set(false);

    if (vcount)
        _RenderQuads(context, vertices, uvs, vcount, mColor);
    
    GLStates::depthWrite.Set(true);
    glEnable(GL_CULL_FACE); // we can eliminate this if we ensure the correct orientation of the vertices   
    
    if (!buffer)
        delete [] (unsigned *)data;

    return renderedCount;
}

void CloudUpdater(Particle &p)
{
    p.position += p.velocity;
    p.alpha =  1.f - (p.time / p.expire);
    p.size  = (1.f - (p.time / p.expire)) * p._maxSize;
}

void CloudGenerator(Particle &p)
{
    p.position   = Vector3();
    p.velocity   = p._direction * (rand() * p._maxSpeed / (float)RAND_MAX);
    p.time       = 0.f;
    p.expire     = rand() * p._maxExpire / (float)RAND_MAX;
    if (p._isShadow)
        p.position.z = p._maxZ - 1;
    else
        p.position.z = rand() * p._maxZ / (float)RAND_MAX;
    p.alpha      =  1.f - (p.time / p.expire);
    p.size       = (1.f - (p.time / p.expire)) * p._maxSize;
}

void OnEndDestroy(ParticleSystem *ps, void *user)
{
    ParticleEffects *fx = reinterpret_cast<ParticleEffects *>(user);
    fx->Destroy(ps);
}

void CircleUpdater(Particle &p)
{
    p.position += p.velocity;
    p.alpha = 1.f - (p.time / p.expire);
    p.size  = p._maxSize;
}

void CircleGenerator(Particle &p)
{
    p.position   = Vector3();
    p.velocity   = p._direction * p._maxSpeed;
    p.time       = 0.f;
    p.expire     = p._maxExpire;
    if (p._isShadow)
        p.position.z = p._maxZ - 1;
    else
        p.position.z = rand() * p._maxZ / (float)RAND_MAX;
    p.alpha      = 1.f - (p.time / p.expire);
    p.size       = p._maxSize;
}

struct MapAndParticleSystem
{
    Map            *map;
    ParticleSystem *effect;

    inline MapAndParticleSystem(ParticleSystem *ps) : map(GetGlobalMap()), effect(ps) {}
};

void CollisionUpdater(Particle &p)
{
    if (p._user)
    {
        MapAndParticleSystem *map = reinterpret_cast<MapAndParticleSystem *>(p._user);
        Vector3 at;
        ICollidable::Classification classify = map->map->Collision(p.position + map->effect->GetPosition(), p.size, at);
        if (classify == ICollidable::CLASS_ON || classify == ICollidable::CLASS_IN)
        {
            // stop
            p.velocity = Vector3();
        }
        else
        {
            p.velocity = p._direction * p._maxSpeed;
        }
    }
    p.position += p.velocity;
    p.alpha = 1.f - (p.time / p.expire);
    p.size  = p._maxSize;
}

void CollisionGenerator(Particle &p)
{
    p.position   = Vector3();
    p.velocity   = p._direction * p._maxSpeed;
    p.time       = 0.f;
    p.expire     = p._maxExpire;
    if (p._isShadow)
        p.position.z = p._maxZ - 1;
    else
        p.position.z = rand() * p._maxZ / (float)RAND_MAX;
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

ParticleSystem *ParticleEffects::CreateCloud(DeviceTexture *texture, const Vector3 &position, float time, float _speed, bool perpetual, bool autodestroy)
{
    ParticleSystem *ps = CreateCloudIndependent(texture, position, time, _speed, perpetual, autodestroy);
    mParticleSystems.push_back(ps);
    return ps;
}

ParticleSystem *ParticleEffects::CreateCloudIndependent(DeviceTexture *texture, const Vector3 &position, float time, float _speed, bool perpetual, bool autodestroy)
{
    ParticleSystem *ps = new ParticleSystem();
    ps->SetTexture(texture);
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
        CloudGenerator(p);
        p._isShadow  = false;
        ps->Add(p); // original
        p.uv0.x = 0.5f;
        p.uv1.x = 1.f;
        p._isShadow  = true;
        CloudGenerator(p);
        ps->Add(p); // shadow
    }
    ps->SetPosition(position);
    ps->SetUpdater(CloudUpdater);
    if (perpetual)
        ps->SetGenerator(CloudGenerator);
    if (autodestroy)
        ps->SetOnEnd(OnEndDestroy, this);
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

extern void SetDefaultLightingType(int);

int ParticleEffects::Render(RenderContext &context, const GameTime &time)
{
    int count = 0;
//  SetDefaultLightingType(1);
    std::vector<ParticleSystem *> fx = mParticleSystems; // we make a copy, because we're going to delete from the original
    for (std::vector<ParticleSystem *>::iterator i = fx.begin(), e = fx.end(); i != e; ++i)
       count += (*i)->Render(context, time);
    fx = mDeferred; // we make a copy, because we're going to delete from the original
    for (std::vector<ParticleSystem *>::iterator i = fx.begin(), e = fx.end(); i != e; ++i)
       count += (*i)->Render(context, time);
    mDeferred.clear();
//  if (count)
//     printf("Particles rendered: %d\n", count);
//  SetDefaultLightingType(0);
    return count;
}