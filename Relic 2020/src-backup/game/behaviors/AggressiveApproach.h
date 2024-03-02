#ifndef _AGGRESSIVEAPPROACH_H
#define _AGGRESSIVEAPPROACH_H

#include "core/core.h"
#include "Approach.h"

class AggressiveApproach : public Approach
{
public:

    CLASS_NEW_DELETE()

    void Update(const GameTime &time);

    void Serialize(IOutStream &s) const;

    static IBehavior *Deserialize(IInStream &s);
};

#endif // _AGGRESSIVEAPPROACH_H