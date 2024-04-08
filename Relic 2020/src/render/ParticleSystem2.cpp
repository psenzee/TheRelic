#include "ParticleSystem2.h"

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
#include "IParticleController.h"

#include <algorithm>

struct ZComparator
{
    inline bool operator()(const Particle &a, const Particle &b) const
    {
        return a.position.z > b.position.z;
    }
};

ParticleSystem2::ParticleSystem2() : _color(1.f, 1.f, 1.f, 1.f), _texture(0), _needs_reorder(false), _blend_type(PARTICLE_BLEND_DARK), _controller(nullptr) {}

ParticleSystem2::~ParticleSystem2()
{
    if (_controller) {
        delete _controller;
    }
    _controller = nullptr;
}

void ParticleSystem2::add(const Particle &p)
{
    std::vector<Particle>::iterator i = first_dead();
    if (i == _particles.end()) {
        if (_needs_reorder) { // if it needs sorting anyway, just stick it in the end
            _particles.push_back(p);
        } else { // otherwise, preserve the ordering
            ZComparator comparator;
            std::vector<Particle>::iterator i = std::upper_bound(_particles.begin(), _particles.end(), p, comparator);
            _particles.insert(i, p);
        }
    } else {
        (*i) = p;
        _needs_reorder = true;
    }
}

int ParticleSystem2::prepare(ImposterRenderer &ir, const GameTime &time)
{
    if (!update((float)time.elapsed)) {
        //mOnEnd(this, mOnEndUser); // mOnEnd could delete -this-, don't do anything that requires -this-  after calling it
        return 0;
    }
    insert_imposters(ir);
    return 1;
}

void ParticleSystem2::update_order()
{
    if (_needs_reorder) {
        ZComparator comparator;
        std::sort(_particles.begin(), _particles.end(), comparator);
        _needs_reorder = false;
    }
}

int ParticleSystem2::update(float ms)
{
    if (!_controller) {
        // if no controller is set, still keep it alive
        return 1;
    }
    int count = 0;
    for (particles_iter_t i = _particles.begin(), e = _particles.end(); i != e; ++i) {
        Particle &p = *i;
        if (p.time < p.expire) { // not dead yet
            p.time += ms;
            _controller->update(*i, _position);
            count++;
        } else if (_controller->regenerates()) { // dead - rebirths
            _controller->regenerate(*i, _position);
            count++;
        }
    }
    return count;
}

void ParticleSystem2::insert_imposters(ImposterRenderer &ir)
{
    Imposter imp;
    ImposterAttributes attrs;
    attrs.texture_id = attrs.program_id = 0;
    attrs.color = _color;
    attrs.emissive = Tuple4f(0.f, 0.f, 0.f, 0.f);
    attrs.average_distance = 0;
    attrs.blend_type = ImposterAttributes::BLEND_LIGHT;
    auto set = ir.get_set(attrs, _texture);
    for (std::vector<Particle>::const_iterator i = _particles.begin(), e = _particles.end(); i != e; ++i) {
        const Particle &particle = *i;
        if (particle.time < particle.expire) { // if not dead..
            imp.position = particle.position;
            imp.size = particle.size;
            imp.texture_uv = { particle.uv0, particle.uv1 };
            set->add(imp);
        }
    }
}

ParticleSystem2::particles_iter_t ParticleSystem2::first_dead()
{
    for (std::vector<Particle>::iterator i = _particles.begin(), e = _particles.end(); i != e; ++i) {
        const Particle &p = *i;
        if (p.time >= p.expire) {
            return i;
        }
    }
    return _particles.end();
}
