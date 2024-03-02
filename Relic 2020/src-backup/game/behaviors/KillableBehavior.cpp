#include "KillableBehavior.h"
#include "Signals.h"
#include "Behaviors.h"
#include "game/Meter.h"

// $TEST! this stuff is here just to test VectorFieldMap
#include "game/GameState.h"
#include "level/Level.h"

void KillableBehavior::Start(Character *self)
{
    SetCharacter(self);
    Meter *meter = new Meter;
    self->SetMeter(meter);
    meter->SetVisible(true);
}

void KillableBehavior::Stop()
{
    if (GetCharacter())
    {
        Meter *meter = GetCharacter()->GetMeter();
        GetCharacter()->SetMeter(0);
        delete meter;
    }
    AbstractBehavior::Stop();
}

void KillableBehavior::KnockBack(float distance, Character *from)
{
    if (GetCharacter())
    {
        mKnockBack = KnockBackState(GetCharacter(), from, distance / GetCharacter()->GetWeight());
        mKnockBack.SetSpin(mKnockSpin);
    }
}

void KillableBehavior::Reset()
{
    if (GetCharacter())
    {
        Character *self = GetCharacter();
        Meter *meter = self->GetMeter();
        meter->SetVisible(true);
        mHitPoints = 1.f;
        mKiller = 0;
        mHitFrames = 0;
    }
}

void KillableBehavior::Update(const GameTime &time)
{
    Character *self = GetCharacter();
// just to test vectorfieldmap
//Vector3 v = state->GetLevel()->GetFieldVector(self->GetPosition());
//SetForce(self, GetMovableDirection(self, -v));
//return;
    if (self && !IsDestroyed())
    {
        // $HACK TO MAKE SURE OUR Z-COOR STAYS AT ZERO       
        self->SetPosition(self->GetPosition() * Vector3(1.f, 1.f, 0.f));

        if (!IsDestroyed())
            mKnockBack.Update(self);
        else
            mHitFrames = 0;
        if (self->GetMeter())
            self->GetMeter()->SetValue(GetHitPoints() / GetMaxHitPoints());
        if (mHitFrames)
        {
            mHitFrames--;
            if (!mHitFrames)
                SetAction(self, "");
        }
    }
    if (mAttackedBy && Character::IsCharacterValid(mAttackedBy))
    {
        // here need to signal on the attacker that he hit
        mAttackedBy->Signal(SIGNAL_LANDED_HIT);
    }
    mAttackedBy = 0;
}

void KillableBehavior::ReceiveHit(float points, float distance, Character *from)
{
    Character *self = GetCharacter();
    if (!mInvulnerable && self && from != self && points > 0.f)
    {
        self->Signal(SIGNAL_RECEIVED_HIT);
        DustHit(self);
        KnockBack(mKnockBackMultiplier * distance, from);
        SetHitPoints(GetHitPoints() - points, from);
        if (from != 0)
        {
            // here need to signal on the attacker that he hit
            mAttackedBy = from;
        }
        if (!IsDestroyed() && !IsInAttack(self)) // $TODO we can't show a receive hit animation if we're attacking, unless we add a new animation for it, which we probably should
        {
            mHitFrames = RECEIVEHIT_FRAMES;
            SetAction(self, "ReceiveHit");
        }
    }
    else if (mInvulnerable && self && from != self && points > 0.f)
    {
        self->Signal(SIGNAL_EVADED_HIT);
    }
}

void KillableBehavior::ReceiveHitInMotion(float points, float distance, Character *from)
{
    Character *self = GetCharacter();
    if (!mInvulnerable && self && from != self && points > 0.f)
    {
        self->Signal(SIGNAL_RECEIVED_HIT);
        DustHit(self);
      //KnockBack(mKnockBackMultiplier * distance, from);
        SetHitPoints(GetHitPoints() - points, from);
        if (from != 0)
        {
            // here need to signal on the attacker that he hit
            mAttackedBy = from;
        }
        if (!IsDestroyed() && !IsInAttack(self)) // $TODO we can't show a receive hit animation if we're attacking, unless we add a new animation for it, which we probably should
        {
            mHitFrames = RECEIVEHIT_FRAMES;
            SetAction(self, "ReceiveHit");
        }
    }
    else if (mInvulnerable && self && from != self && points > 0.f)
    {
        self->Signal(SIGNAL_EVADED_HIT);
    }
}

extern float GetGainedExperience(Character *killer, Character *killed);

void KillableBehavior::Killed(Character *killer)
{
    mHitPoints = 0.f;
    mKiller = killer;
    GetCharacter()->Signal(SIGNAL_DIED);
    if (GetCharacter()->GetMeter())
        GetCharacter()->GetMeter()->SetVisible(false);
    if (IsKillable(killer))
    {
        float xp = GetGainedExperience(killer, GetCharacter());
        // $TODO send a message that killer gained xp
        ::SetExperience(killer, ::GetExperience(killer) + xp);
    }      
}

void KillableBehavior::SetHitPoints(float value, Character *from)
{
    if (value < 0.f)
        value = 0.f;
    if (value == mHitPoints)
        return;
    mHitPoints = value;
    if (mHitPoints > mMaxHitPoints)
        mHitPoints = mMaxHitPoints;
    if (mHitPoints <= 0.f)
        Killed(from);
    else mKiller = 0;
}

void KillableBehavior::SetMaxHitPoints(float value)
{ 
    mMaxHitPoints = value;
    if (mMaxHitPoints < 1.f)
        mMaxHitPoints = 1.f;
}
