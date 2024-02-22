#include "Attack.h"
#include "core/core.h"
#include "Behaviors.h"
#include "game/Character.h"
#include "game/Meter.h"

float Attack::GetAttackDistance() const
{
    if (!GetCharacter() || !GetAttackTarget() ||
        IsDestroyed(const_cast<Character *>(GetCharacter())) || IsDestroyed(const_cast<Character *>(GetAttackTarget())) ||
        !GetAttackTarget()->IsVisible())
        return FLT_MAX;
    return GetCharacter()->GetPosition().distance(GetAttackTarget()->GetPosition());
}

bool Attack::IsAttackActive() const
{
    const Character *self = GetCharacter();
    return self && GetAttackTarget() && self->IsVisible() && !IsDestroyed(const_cast<Character *>(self));
}

bool Attack::IsAbleToAttack() const
{
    const Character *self = GetCharacter();
    return self && self->IsVisible() && !IsDestroyed(const_cast<Character *>(self));
}

/*
void Attack::SetAttackFrames(int frames, int fullFrames)
{
    Character *self = GetCharacter();
    if (self)
    {
        if (!mAttackFrames && frames)
        {
            self->Signal(SIGNAL_ATTACK_START);
            SetAction(self, "Attack");
            mAttackReady = false;
        }
        mAttackFrames = frames;
        mFullFrames   = fullFrames;
    }
}
*/

void Attack::StartAttack()
{
    if ((!IsAutoAttack() || IsAttackActive()) && IsAttackReady()) // auto attack does not attack if there's no one there to attack
    {
        if (/*!mAttackFrames && */mAttackMaxFrames)
        {
            Character *self = GetCharacter();
            self->Signal(SIGNAL_ATTACK_START);
            SetActionReset(self, "Attack");
            if (!GetWalkAndAttack(self))
                SetCanMove(self, false);
            mAttackReady = false;
        }
        mAttackFrames = mAttackMaxFrames;
        mFullFrames   = mMaxFullFrames;
    }
}

void Attack::Update(const GameTime &time)
{
    Character *self = GetCharacter();
    if (IsAttackActive())
    {
        bool processTriggerAttack = false;
        if (mAttackFrames > 0)
        {
            mAttackFrames--;
            if (!mAutoAttack && /*GetAttackCurrentFrame() > 6/*CONTACT_FRAME* / &&*/ mTriggerAttack)
            {
                processTriggerAttack = true;
            }
            else if (!mAttackFrames)
            {
                self->Signal(SIGNAL_ATTACK_COMPLETE);
                SetAction(self, "");
                SetCanMove(self, true);
                mLastAttack = mAttackTotalFrames;
                mAttackReady = false;
            }
        }
        else
        {
            processTriggerAttack = true;
        }
        if (processTriggerAttack && mTriggerAttack && (mAttackTotalFrames - mLastAttack > mAttackSpeed))
        {
            SetAction(self, "");
            mAttackReady = true;
            if (!mAutoAttack)
                mTriggerAttack = false;
        }
        if (mFullFrames > 0)
        {
            mFullFrames--;
            if (!mFullFrames)
            {
                SetAction(self, "");
                SetCanMove(self, true);
            }
        }
    }
    // support pointless swinging for non-auto attack
    else if (!mAutoAttack && mTriggerAttack && !GetAttackTarget())
    {
        mAttackReady = true;
        mTriggerAttack = false;
    }
    mAttackTotalFrames++;
}