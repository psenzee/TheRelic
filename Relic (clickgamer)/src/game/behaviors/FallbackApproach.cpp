#include "FallbackApproach.h"
#include "core/core.h"
#include "Behaviors.h"
#include "game/Character.h"
#include "game/Meter.h"
#include "map/DynamicMap.h"

void FallbackApproach::Update(const GameTime &time)
{
    if (IsActive())
    {
        Character *self = GetCharacter(),
                  *to   = GetApproachTarget();

        if (IsDestroyed(to))
            return;

        Vector3    pos(self->GetPosition()),
                   topos(to->GetPosition()),
                   dir(topos - pos);
        float      distance = dir.length(),
                   speed    = GetApproachSpeed(),
                   mind     = GetApproachMinDistance();
        if (distance < mind || distance > to->GetRadius() + self->GetRadius())
        {
            if (distance < mind)
                speed = -speed;
            dir /= distance;
            dir *= GetApproachSpeed();           
            SetForce(self, GetMovableDirection(self, dir));
        }
    }
    Approach::Update(time);
}

void FallbackApproach::Serialize(IOutStream &s) const
{
    // $TODO
}

IBehavior *FallbackApproach::Deserialize(IInStream &s)
{
    // $TODO
    return 0;
}