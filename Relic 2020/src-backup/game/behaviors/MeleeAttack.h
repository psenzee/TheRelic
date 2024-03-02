#ifndef _MELEEATTACK_H
#define _MELEEATTACK_H

#include "core/core.h"
#include "Attack.h"

class MeleeAttack : public Attack
{
//  enum { ATTACK_FRAMES = 10, FULL_FRAMES = 15, CONTACT_FRAME = 6, RESET_FRAMES = 8 };
    enum { ATTACK_FRAMES = 25, FULL_FRAMES = 29, CONTACT_FRAME = 6, RESET_FRAMES = 8 };

    void Hit(float multiplier);

public:

    MeleeAttack() : Attack() { SetAttackMaxFrames(ATTACK_FRAMES, FULL_FRAMES); }

    void Update(const GameTime &time);

    void Serialize(IOutStream &s) const
    {
        // $TODO
    }

    static IBehavior *Deserialize(IInStream &s);
};

#endif // _MELEEATTACK_H