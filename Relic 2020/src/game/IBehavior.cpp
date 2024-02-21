#include "IBehavior.h"
#include "Character.h"

Character *AbstractBehavior::GetCharacter()
{ 
    return (mCharacter = Character::GetValidCharacter(mCharacter));
}

const Character *AbstractBehavior::GetCharacter() const
{ 
    return (mCharacter = Character::GetValidCharacter(mCharacter));
}