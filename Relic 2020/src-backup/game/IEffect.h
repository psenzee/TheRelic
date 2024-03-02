#ifndef _IEFFECT_H
#define _IEFFECT_H

#include "core/core.h"

class GameTime;
class RenderContext;

class IEffect
{
public:

    enum Type { FX_NONE, FX_CIRCLE, FX_CYLINDER, FX_PARTICLE };

    virtual ~IEffect() {}

    virtual Type GetType() const                = 0;
    virtual void SetPosition(const Vector3 &p)  = 0;
    virtual void Update(const GameTime &time)   = 0;
    virtual void Render(RenderContext &context) = 0;
    virtual void Complete()                     = 0;
    virtual bool IsComplete() const             = 0;
};

#endif // _IEFFECT_H