#include "ParticleCircle.h"
#include "core/core.h"
#include "ParticleEffects2.h"
#include "ParticleSystem2.h"

ParticleSystem2 *ParticleCircle::create(DeviceTexture *texture, const Vector3 &position, float time, float speed, bool perpetual, bool autodestroy)
{
    _autodestroy = autodestroy;
    _perpetual = perpetual;
    
    ParticleSystem2 *ps = new ParticleSystem2();
    ps->set_texture(texture);
    ps->set_position(position);

    for (float i = 0.f, to = math::PIf * 2.f + 0.1f; i <= to; i += 0.1f)
    {
        Particle p;
        p.expire = time;
        p.velocity = Vector3(sin(i) * speed, cos(i) * speed, 0.f);
        p.position.z = rand() * -position.z / (float)RAND_MAX;
        p.uv1.x = 0.5f;
        p._maxSpeed  = speed;
        p._maxExpire = time;
        p._maxZ      = -position.z;
        p._maxSize   = 75.0f;
        p._direction = Vector3(sin(i), cos(i), 0.f);
        p._isShadow  = false;
        ps->add(p); // original
        p.uv0.x = 0.5f;
        p.uv1.x = 1.f;
        p.position.z = -position.z - 1;
        p._isShadow  = true;
        ps->add(p); // shadow
    }
    return ps;
}

void ParticleCircle::regenerate(Particle &p, const Tuple3f &position) const
{
    if (_perpetual) {
        p.position   = position;
        p.velocity   = p._direction * p._maxSpeed;
        p.time       = 0.f;
        p.expire     = p._maxExpire;
        p.position.z += (p._isShadow ? (p._maxZ - 1) : (rand() * p._maxZ / (float)RAND_MAX));
        p.alpha      = 1.f - (p.time / p.expire);
        p.size       = p._maxSize;
    }
}

void ParticleCircle::update(Particle &p, const Tuple3f &position) const
{
    p.position += p.velocity;
    p.alpha = 1.f - (p.time / p.expire);
    p.size  = p._maxSize;
}

void ParticleCircle::end(ParticleSystem2 *ps) const
{
    if (_autodestroy) {
        ParticleEffects2 *fx = ParticleEffects2::instance();
        if (fx) {
            fx->remove(ps);
        }
        delete ps;
    }
}

