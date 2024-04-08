#pragma once

#include "core/core.h"
#include "fast/Allocator.h"

class RenderContext;
class DeviceTexture;
class ParticleSystem2;
class ImposterRenderer;

class ParticleEffects2
{
public:

    void                     defer_render(ParticleSystem2 *ps) { _deferred.push_back(ps); }
    void                     destroy(ParticleSystem2 *ps);
    void                     remove(const ParticleSystem2 *ps);
    void                     deferred_destroy(ParticleSystem2 *ps);

    int                      render(RenderContext &context, const GameTime &time);
    
    ImposterRenderer        *imposter_renderer()                                  { return _imposter_renderer; }

    static void              create_instance(ImposterRenderer *imposter_renderer) { if (!_instance) _instance = new ParticleEffects2(imposter_renderer); }
    static void              destroy_instance()                                   { if (_instance) delete _instance; _instance = nullptr; }
    
    static ParticleEffects2 *instance()                                           { return _instance; }

private:

    ParticleEffects2(ImposterRenderer *imposter_renderer);
    
    static ParticleEffects2        *_instance;
    
    ImposterRenderer               *_imposter_renderer;
    std::vector<ParticleSystem2 *>  _particle_systems;
    std::vector<ParticleSystem2 *>  _deferred;
};
