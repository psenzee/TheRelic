#include "DieCollapseBehavior.h"
#include "core/core.h"
#include "Behaviors.h"
#include "game/Character.h"

static void SetAnimationAction(Character *self, const char *name)
{
    if (self)
    {
        if (IsAnimation(self)) SetAction(self, name);
        else                   self->SetAnimation(name);
    }
}

DieCollapseBehavior::DieCollapseBehavior() : DieBehavior(), mDieFrames(0), mDeathState(ALIVE)
{
}

void DieCollapseBehavior::Reset()
{
    Character *self = GetCharacter();
    if (self)
    {
        mDieFrames = 0;
        mDeathState = ALIVE;
        SetCanMove(self, true);
        SetAnimationAction(self, "Idle");
        self->SetCollidable(true);
    }
    DieBehavior::Reset();
}

void DieCollapseBehavior::Update(const GameTime &time)
{
    Character *self = GetCharacter();
    if (self)
    {
        if (mDeathState == DEATH_INPROGRESS)
        {        
            if (!mDieFrames--)
            {
                mDeathState = DEATH_COMPLETE;
                SetDeathComplete(true);
                self->Signal(SIGNAL_DEATH_COMPLETE);
            }
        }
        if (mDeathState == DEATH_COMPLETE)
            self->ClearSignalHandlers();
    }
}

void DieCollapseBehavior::Signal(int signal)
{
    if (signal == SIGNAL_DIED)
    {
        mDieFrames  = DIE_FRAMES;
        mDeathState = DEATH_INPROGRESS;
        Character *self = GetCharacter();
        if (self)
        {
            SetCanMove(self, false);
            SetAnimationAction(self, "Die");
            self->SetCollidable(false);
            Animation *animation = self->GetAnimation();
            if (animation)
            {
                animation->Reset();
                animation->SetLoopMode(false);
                Vector3 p(self->GetPosition());
                self->SetPosition(Vector3(p.x, p.y, RandomFloat() * 2.f)); // avoid z-fighting among corpse imposters
            }
        }
    }
}

void DieCollapseBehavior::Serialize(IOutStream &s) const
{
    // $TODO
}

IBehavior *DieCollapseBehavior::Deserialize(IInStream &s)
{
    // $TODO
    return 0;
}