#pragma once

#include "core/core.h"
#include "fast/Allocator.h"
#include "Particle.h"
#include "DynamicBuffer.h"
#include "ImposterRenderer.h"

class RenderContext;
class DeviceTexture;

class IParticleController;

class ParticleSystem2
{
public:
    
    typedef std::vector<Particle>           particles_t;
    typedef std::vector<Particle>::iterator particles_iter_t;
    
    CLASS_NEW_DELETE()
    
    enum ParticleBlendType { PARTICLE_BLEND_LIGHT = 0, PARTICLE_BLEND_DARK = 1 };

    ParticleSystem2();
    ~ParticleSystem2();

    void           set_position(const Vector3 &p)                       { _position = p; }
    const Vector3 &get_position()                                 const { return _position; }
    void           clear()                                              { _particles.clear(); }
    
    void           set_controller(IParticleController *controller)      { _controller = controller; }

    void           set_texture(DeviceTexture *t)                        { _texture = t; }
    void           set_color(const Vector4 &c)                          { _color = c; }
    void           set_blend_type(ParticleBlendType type)               { _blend_type = type; }
    
    void           add(const Particle &p);
    int            prepare(ImposterRenderer &ir, const GameTime &time);

private:

    void insert_imposters(ImposterRenderer &ir);
    int  update(float ms);
    void update_order();

    particles_iter_t first_dead();

    particles_t            _particles;
    Vector3                _position;
    Vector4                _color;
    DeviceTexture         *_texture;
    ParticleBlendType      _blend_type;
    bool                   _needs_reorder;
    IParticleController   *_controller;
};
