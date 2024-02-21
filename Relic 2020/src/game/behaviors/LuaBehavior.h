#ifndef _LUABEHAVIOR_H
#define _LUABEHAVIOR_H

#include "game/IBehavior.h"
#include "game/Character.h"

class LuaBehavior : public AbstractBehavior
{
public:
    
    CLASS_NEW_DELETE()

    LuaBehavior(int behaviorTypeId, const String &function);

    void Start(Character *self);
    void Stop();
    
    void Update(const GameTime &time);
    
private:

    String mFunction;

    bool Execute(const String &function);
};

#endif  // _LUABEHAVIOR_H