#ifndef _KNOCKBACKSTATE_H
#define _KNOCKBACKSTATE_H

#include "core/core.h"

class KnockBackState
{
    enum { FRAMES = 10 };

    Vector3 mOrigin,
            mDirection;
    float   mOrientation;
    int     mFrames;
    float   mDistance;
    float   mSpin;

public:

    KnockBackState() : mOrientation(0.f), mFrames(0), mDistance(0.f), mSpin(1.f) {}

    KnockBackState(Character *character, Character *from, float distance) : mSpin(1.f)
    {
        if (!from) from = character;

        Vector3 a = character->GetPosition(),
                b = from->GetPosition();
        mOrigin      = a;
        mDirection   = (b - a).normal();
        mFrames      = FRAMES;
        mOrientation = character->GetOrientationAngle();
        mDistance    = distance;
    }

    void  SetSpin(float value) { mSpin = value; }
    float GetSpin() const      { return mSpin; }

    void Update(Character *character) // don't call if dead
    {
        if (character && mFrames > 0)
        {
            float complete = /*(FRAMES -*/ mFrames/*)*/ / (float)FRAMES;
            float back = mDistance / FRAMES * (FRAMES - mFrames);
            character->SetPosition(mOrigin - mDirection * back);
            character->SetOrientationAngle(mOrientation + math::PIf * complete * mSpin);
            mFrames--;
        }
    }
};

#endif // _KNOCKBACKSTATE_H
