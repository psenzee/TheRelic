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
#include "std_utils.h"

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

static void _RenderQuads(RenderContext &context, const float *vertices, const float *uvs, int count, const Vector4 &color)
{
    if (!count || !vertices || !uvs) return;
        
    // RENDER DRAW LIST
    Vector4 ambient(color * Vector4(2.0f, 2.0f, 2.0f, 0.0f)),
            diffuse(1.0f, 1.0f, 1.0f, color.w);
    
    GLSetMaterial4(GL_AMBIENT, ambient);
    GLSetMaterial4(GL_DIFFUSE, diffuse);
    
    GLSetVertexPointer(vertices);
    GLSetTexCoordPointer(uvs);

    GLDrawArrays(GL_TRIANGLES, count);
}

ParticleSystem::ParticleSystem() : mUpdater(0), mGenerator(0), mBuffer(0), mColor(1.f, 1.f, 1.f, 1.f), mTexture(0), mNeedsReorder(false), mOnEnd(0), mOnEndUser(0), mBlendType(PARTICLE_BLEND_DARK)
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
    if (i == mParticles.end()) {
        if (mNeedsReorder) { // if it needs sorting anyway, just stick it in the end
            mParticles.push_back(p);
        } else { // otherwise, preserve the ordering
            ZComparator comparator;
            std::vector<Particle>::iterator i = std::upper_bound(mParticles.begin(), mParticles.end(), p, comparator);
            mParticles.insert(i, p);
        }
    } else {
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

int ParticleSystem::prepare(ImposterRenderer &ir, const GameTime &time)
{
    if (!UpdateParticles((float)time.elapsed) && mOnEnd) {
        mOnEnd(this, mOnEndUser); // mOnEnd could delete -this-, don't do anything that requires -this-  after calling it
        return 0;
    }
    insertImposters(ir);
    return 1;
}

void ParticleSystem::UpdateOrder()
{
    if (mNeedsReorder) {
        ZComparator comparator;
        std::sort(mParticles.begin(), mParticles.end(), comparator);
        mNeedsReorder = false;
    }
}

int ParticleSystem::UpdateParticles(float ms)
{
    if (!mUpdater) {
        // if no updater is set, still keep it alive
        return 1;
    }
    int count = 0;
    for (std::vector<Particle>::iterator i = mParticles.begin(), e = mParticles.end(); i != e; ++i) {
        Particle &p = *i;
        if (p.time < p.expire) { // not dead yet
            p.time += ms;
            mUpdater(*i, mPosition);
            count++;
        } else if (mGenerator) { // dead - rebirths
            mGenerator(p, mPosition);
            count++;
        }
    }
    return count;
}

void ParticleSystem::insertImposters(ImposterRenderer &ir)
{
    Imposter imp;
    ImposterAttributes attrs;
    attrs.texture_id = attrs.program_id = 0;
    attrs.color = mColor;
    attrs.emissive = Tuple4f(0.f, 0.f, 0.f, 0.f);
    attrs.average_distance = 0;
    attrs.blend_type = ImposterAttributes::BLEND_LIGHT;
    auto set = ir.get_set(attrs, mTexture);
    for (std::vector<Particle>::const_iterator i = mParticles.begin(), e = mParticles.end(); i != e; ++i) {
        const Particle &particle = *i;
        if (particle.time < particle.expire) { // if not dead..
            imp.position = particle.position;
            imp.size = particle.size;
            imp.texture_uv = { particle.uv0, particle.uv1 };
            set->add(imp);
        }
    }
}

std::vector<Particle>::iterator ParticleSystem::FindFirstDeadParticle()
{
    for (std::vector<Particle>::iterator i = mParticles.begin(), e = mParticles.end(); i != e; ++i) {
        Particle &p = *i;
        if (p.time >= p.expire) {
            return i;
        }
    }
    return mParticles.end();
}
