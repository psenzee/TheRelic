#ifndef _FALLBACKAPPROACH_H
#define _FALLBACKAPPROACH_H

#include "core/core.h"
#include "Approach.h"

class FallbackApproach : public Approach
{
public:

    CLASS_NEW_DELETE()

    void Update(const GameTime &time);

    void Serialize(IOutStream &s) const;

    static IBehavior *Deserialize(IInStream &s);
};

#endif // _FALLBACKAPPROACH_H