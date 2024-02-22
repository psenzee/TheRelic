#ifndef _HITANDRUNAPPROACH_H
#define _HITANDRUNAPPROACH_H

#include "core/core.h"
#include "Approach.h"

class HitAndRunApproach : public Approach
{
	int count;
	
public:

    CLASS_NEW_DELETE()

    void Update(const GameTime &time);

    void Serialize(IOutStream &s) const;

    static IBehavior *Deserialize(IInStream &s);
};

#endif // _HITANDRUNAPPROACH_H