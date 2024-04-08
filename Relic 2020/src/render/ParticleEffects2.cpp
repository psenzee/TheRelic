#include "ParticleEffects2.h"
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

#include <algorithm>
#include <iostream>

ParticleEffects2::ParticleEffects2(ImposterRenderer *imposter_renderer) : _imposter_renderer(imposter_renderer)
{
}

ParticleEffects2 *ParticleEffects2::_instance;

void ParticleEffects2::remove(const ParticleSystem2 *ps)
{
    _particle_systems.erase(
        std::remove(_particle_systems.begin(), _particle_systems.end(), ps), _particle_systems.end());
}

void ParticleEffects2::destroy(ParticleSystem2 *ps)
{
    remove(ps);
    delete ps;
}

void ParticleEffects2::deferred_destroy(ParticleSystem2 *ps)
{
    // here we destroy the particle system by turning off its Generator and setting its OnEnd function,
    // then letting it run its course
    if (ps)
    {
        //ps->SetGenerator(0);
        //ps->SetOnEnd(OnEndDestroy, this);
    }
}

int ParticleEffects2::render(RenderContext &context, const GameTime &time)
{
    int count = 0;
    _imposter_renderer->clear();
    std::vector<ParticleSystem2 *> fx(_particle_systems); // make a copy, because we're going to delete from the original
    append(std::span<ParticleSystem2 *>(_deferred.data(), _deferred.size()), fx);
    for (std::vector<ParticleSystem2 *>::iterator i = fx.begin(), e = fx.end(); i != e; ++i) {
        count += (*i)->prepare(*_imposter_renderer, time);
    }
    _deferred.clear();
    _imposter_renderer->prepare();
    _imposter_renderer->render(context);
    return count;
}
