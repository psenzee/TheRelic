#include "MeleeAttack.h"
#include "core/core.h"
#include "Behaviors.h"

void MeleeAttack::Hit(float multiplier)
{
    Character *self = GetCharacter(),
              *to   = GetAttackTarget();
    bool inRange = GetAttackDistance() < (self->GetRadius() + to->GetRadius()) * 1.5f;
    // here we do some fancy math to figure out whether we'll hit and for how many points, and how far to knock back
    if (!inRange)
    {
        // missed
    }
    else if (self->IsFacing(GetAttackTarget()->GetPosition(), 1.552))
    {
        float hitPoints = 5.0f; // a formula based on enemy character level for non-role-players
        if (IsRolePlayer(self))
        {
            hitPoints = GetAttackRating(self);
        }
        hitPoints *= multiplier;
        printf("hit for %.2f points\n", hitPoints);
        ReceiveHit(to, self, hitPoints, 60.f * multiplier); // figure out how many hp and the knockback distance
    }
}

void MeleeAttack::Update(const GameTime &time)
{
    Attack::Update(time);
    if (IsAbleToAttack())
    {
        Character *self = GetCharacter(),
                  *to   = GetAttackTarget();
        if (to)
        {
            bool inRange = GetAttackDistance() < (self->GetRadius() + to->GetRadius()) * 1.5f;
            if (IsAttackInProgress() && GetAttackCurrentFrame() == CONTACT_FRAME)
            {
                Hit(1.f);
            }
            else if (IsAttackReady())
            {
                if (!IsAutoAttack() || (self->IsFacing(GetAttackTarget()->GetPosition(), 1.0) && inRange)) // allow pointless attacks if not auto attack
                {
                    int currentFrame = GetAttackCurrentFrame();
                    if (currentFrame > 0 && currentFrame < CONTACT_FRAME)
                    {
                        // here, we're starting another attack before we've completed the first.
                        // to make this feel right, we need to finish our first hit with some (diminished) damage
                        Hit(0.33f);
                    }
                    StartAttack();
                }
            }
        }
        else if (IsAttackReady())
        {
            StartAttack();
        }
    }
}
