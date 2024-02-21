#ifndef _IROLEPLAYER_H
#define _IROLEPLAYER_H

#include "core/core.h"
#include "core/random.h"

class IRolePlayer
{
public:

    virtual ~IRolePlayer() {}
        
    virtual void          SetStrength    (unsigned value) = 0;
    virtual void          SetDexterity   (unsigned value) = 0;
    virtual void          SetVitality    (unsigned value) = 0;
    virtual void          SetEnergy      (unsigned value) = 0;

    virtual void          SetLuck        (float value) = 0;
    
    virtual unsigned      GetStrength    () const = 0;
    virtual unsigned      GetDexterity   () const = 0;
    virtual unsigned      GetVitality    () const = 0;
    virtual unsigned      GetEnergy      () const = 0;

    virtual float         GetLuck        () const = 0;

    virtual float         GetClassBonus  () const = 0;
    virtual float         GetRatingBonus () const = 0;

    virtual core::Random &GetRandom      ()       = 0;
};

#endif // _IROLEPLAYER_H