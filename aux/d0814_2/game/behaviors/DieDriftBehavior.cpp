#include "DieDriftBehavior.h"
#include "core/core.h"
#include "DriftApproach.h"
#include "Behaviors.h"
#include "game/Character.h"

DieDriftBehavior::DieDriftBehavior() : DieBehavior()
{
}

void DieDriftBehavior::Update(const GameTime &time)
{
    Character *self = GetCharacter();
    if (self && IsDeathComplete())
        self->ClearSignalHandlers();
}

static void SetAnimationAction(Character *self, const char *name)
{
    if (self)
    {
        if (IsAnimation(self)) SetAction(self, name);
        else                   self->SetAnimation(name);
    }
}

void DieDriftBehavior::Signal(int signal)
{
    Character *self = GetCharacter();
    if (!self)
        return;
    if (signal == SIGNAL_DIED)
    {
        SetAnimationAction(self, "Die");
        self->SetCollidable(false);
        SetCanMove(self, true);
        Animation *animation = self->GetAnimation();
        if (animation)
        {
            animation->Reset();
            animation->SetLoopMode(false);
        }
        self->RemoveBehavior(Approach::TYPE);
        Approach *approach = new DriftApproach(5.f, 1000.0f, 3.f);
        self->AddBehavior(approach);
        approach->SetApproachCollide(false);
    }
    else if (signal == SIGNAL_ABSORBED)
    {
        SetDeathComplete(true);
        self->Signal(SIGNAL_DEATH_COMPLETE);
        self->RemoveBehavior(Approach::TYPE);
    }
}

void DieDriftBehavior::Serialize(IOutStream &s) const
{
    // $TODO
}

IBehavior *DieDriftBehavior::Deserialize(IInStream &s)
{
    // $TODO
    return 0;
}