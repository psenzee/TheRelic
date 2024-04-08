#pragma once

#include "core/core.h"
#include "IParticleController.h"

class ParticleCircle : public IParticleController
{
public:

    ParticleSystem2 *create(DeviceTexture *texture, const Vector3 &position, float time, float speed, bool perpetual, bool autodestroy);

    void regenerate(Particle &p, const Tuple3f &position) const;
    void update(Particle &p, const Tuple3f &position)     const;
    void end(ParticleSystem2 *ps)                         const;
    bool regenerates()                                    const { return _perpetual; }
    
private:

    bool _autodestroy;
    bool _perpetual;
};
