#include "HitAndRunApproach.h"
#include "core/core.h"
#include "Behaviors.h"
#include "game/Character.h"
#include "game/Meter.h"
#include "map/DynamicMap.h"

// $TEST! this stuff is here just to test VectorFieldMap
#include "game/Game.h"
#include "level/Level.h"

extern Game *GetGlobalGame();

void HitAndRunApproach::Update(const GameTime &time)
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
        float distance = dir.length(),
              speed    = GetApproachSpeed(),
              mind     = GetApproachMinDistance(),
              toofar   = GetApproachTooFar();
		
        dir /= distance;
        if (true)//(distance < toofar)
        {
            float repel = 1.f;
            if (GetHitPoints(self) > 0 && GetHitPoints(self) < GetMaxHitPoints(self) * 0.25f && count < 30)
            {
                repel = -1.f; // make it run if health drops low
                SetHitPoints(self, self, GetHitPoints(self) + 0.25f);
				count++;
            }
			else
			{
				count = 0;
			}
            float hi = mind * 2.f, lo = mind;
            if (distance > math::max(to->GetRadius() /*+ self->GetRadius()*/, hi))
            {
                float speed = GetApproachSpeed();
                if (repel < 0.f)
                    speed *= 2.f;
                dir *= math::clamp((((distance - lo) / (hi - lo)) * 2.f - 1.f), -1.f, 1.f) * speed;
                SetForce(self, GetMovableDirection(self, repel * dir + -v));
            }
            else
            {
                dir *= 1.5f; // just enough to make it face the right direction..
                SetForce(self, GetMovableDirection(self, repel * dir + -v));
            }
        }
    }
    Approach::Update(time);
}

void HitAndRunApproach::Serialize(IOutStream &s) const
{
    // $TODO
}

IBehavior *HitAndRunApproach::Deserialize(IInStream &s)
{
    // $TODO
    return 0;
}