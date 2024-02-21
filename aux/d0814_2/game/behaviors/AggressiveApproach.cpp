#include "AggressiveApproach.h"
#include "core/core.h"
#include "Behaviors.h"
#include "game/Character.h"
#include "game/Meter.h"
#include "map/DynamicMap.h"

// $TEST! this stuff is here just to test VectorFieldMap
#include "game/Game.h"
#include "level/Level.h"

extern Game *GetGlobalGame();

void AggressiveApproach::Update(const GameTime &time)
{
    if (IsActive())
    {
        Character *self = GetCharacter(),
                  *to   = GetApproachTarget();
// $TEST! this stuff is here just to test VectorFieldMap
Vector3 v = GetGlobalGame()->GetLevel()->GetFieldVector(self->GetPosition());
//SetForce(self, GetMovableDirection(self, -v));
//Approach::Update(time);
//return;

        if (IsDestroyed(to))
            return;

        Vector3    pos(self->GetPosition()),
                   topos(to->GetPosition()),
                   dir(topos - pos);
        float      distance = dir.length(),
                   speed    = GetApproachSpeed(),
                   mind     = GetApproachMinDistance(),
                   toofar   = GetApproachTooFar();

        dir /= distance;
        if (distance < toofar)
        {
            float hi = mind * 2.f, lo = mind;
            if (distance > math::max(to->GetRadius() /*+ self->GetRadius()*/, hi))
            {
                dir *= math::clamp((((distance - lo) / (hi - lo)) * 2.f - 1.f), -1.f, 1.f) * GetApproachSpeed();
//              SetForce(self, GetMovableDirection(self, dir));
              SetForce(self, GetMovableDirection(self, dir + -v));
            }
            else
            {
                dir *= 1.5f; // just enough to make it face the right direction..
//SetForce(self, GetMovableDirection(self, dir));
SetForce(self, GetMovableDirection(self, dir + -v));
            }
            //SetForce(self, GetMovableDirection(self, dir));
        }
    }
    Approach::Update(time);
}

void AggressiveApproach::Serialize(IOutStream &s) const
{
    // $TODO
}

IBehavior *AggressiveApproach::Deserialize(IInStream &s)
{
    // $TODO
    return 0;
}