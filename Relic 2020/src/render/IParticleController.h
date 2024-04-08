#pragma once

#include "core/core.h"
#include "tuple3f.h"
#include "Particle.h"
#include "IParticleController.h"

class ParticleSystem2;

class IParticleController
{
public:
    virtual ~IParticleController();

    virtual void regenerate(Particle &p, const Tuple3f &position) const = 0;
    virtual void update(Particle &p, const Tuple3f &position)     const = 0;
    virtual void end(ParticleSystem2 *ps)                         const = 0;
    virtual bool regenerates()                                    const = 0;
};
