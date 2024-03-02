#ifndef _ROLEPLAYRULES_H
#define _ROLEPLAYRULES_H

#include "core/core.h"
#include "core/random.h"

class Character;

float      RolePlayRandom(core::Random &random, float luck, float undesirable, float desirable);

void       SetDefaultRpgAttributes(Character *character);
int        GetNumberOfPlayers();
Character *GetPlayer(int index);
float      GetPlayerExperienceFromLevel(int level);
float      GetAttackRating(Character *character);
float      GetDefenseRating(Character *character);
float      GetChanceToHit(Character *attacker, Character *defender);
float      GetDefeatedExperienceFromLevel(int level);
int        GetLevelFromPlayerExperience(float xp);
int        GetLevelFromDefeatedExperience(float xp);
int        GetSumOfPlayersLevels();
float      GetMonsterLife(Character *c);
float      GetMonsterExperience(Character *c);
float      GetGainedExperienceUnadjusted(Character *killer, Character *killed);
float      GetGainedExperience(Character *killer, Character *killed);

#endif // _ROLEPLAYRULES_H