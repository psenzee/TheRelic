#include "RolePlayRules.h"
#include "Behaviors.h"

enum { MAX_LEVELUP = 128 };

void SetDefaultRpgAttributes(Character *character)
{
    // default barbarian attributes..
    SetStrength(character, 30);
    SetDexterity(character, 20);
    SetVitality(character, 25);
    SetEnergy(character, 10);
    
    SetLuck(character, 0.f); // -1.f to 1.f
}

float RolePlayRandom(core::Random &random, float luck, float undesirable, float desirable) 
{
    float value = math::clamp(random.realf() + luck, 0.f, 1.f);
    return (desirable - undesirable) * value + undesirable;
}

int GetNumberOfPlayers()
{
    // $TODO
    return 1;
}

Character *GetPlayer(int index)
{
    // $TODO
    return 0;
}

float GetAttackRating(Character *character)
{
    // Base Attack Rating
    // (Your attack rating without +AR or +Ar% from equipment, charms or skills)
    // = (Dex * 5) - 35 + ClassBonus 
    return (GetDexterity(character) * 5.f) - 35.f + GetClassBonus(character) + GetRatingBonus(character);
}

float GetDefenseRating(Character *character)
{
    // Base Defense Rating
    // -------------
    // (Your Defense rating without Defense, +Def or +Def% from equipment, charms or skills)
    // = Floor(Dex / 4)
    return floorf(GetDexterity(character) * 0.25f);
}

float GetChanceToHit(Character *attacker, Character *defender)
{
    // Clamp(100 * AR / (AR + DR) * 2 * alvl / (alvl + dlvl), 5, 95)
    float ar   = GetAttackRating(attacker), dr   = GetDefenseRating(defender);
    float alvl = GetLevel(attacker),        dlvl = GetLevel(defender);
    return math::clamp(100.f * ar / (ar + dr) * 2.f * alvl / (alvl + dlvl), 5.f, 95.f);
}

float GetPlayerExperienceFromLevel(int level)
{
    // I think this may not be right, it may be different from GetDefeatedExperienceFromLevel(), ie perhaps this should be higher
    return level * 10.f + level * level; 
}

float GetDefeatedExperienceFromLevel(int level)
{
    // empirical formula from d2 tables
    return level * 10.f + level * level; 
}

int GetLevelFromPlayerExperience(float xp)
{
    static float table[MAX_LEVELUP];
    static bool  initialized = false;
    if (!initialized)
        for (int i = 0; i < MAX_LEVELUP; i++)
            table[i] = GetPlayerExperienceFromLevel(i);
    for (int i = 1; i < MAX_LEVELUP; i++)
        if (xp < table[i + 1])
            return i;
    return MAX_LEVELUP - 1;
}

int GetLevelFromDefeatedExperience(float xp)
{
    static float table[MAX_LEVELUP];
    static bool  initialized = false;
    if (!initialized)
        for (int i = 0; i < MAX_LEVELUP; i++)
            table[i] = GetDefeatedExperienceFromLevel(i);
    for (int i = 1; i < MAX_LEVELUP; i++)
        if (xp < table[i + 1])
            return i;
    return MAX_LEVELUP - 1;
}

int GetSumOfPlayersLevels()
{
    int levels = 0;
    for (int i = 0; i < GetNumberOfPlayers(); i++)
        levels += GetLevel(GetPlayer(i));
    return levels;
}

float GetMonsterLife(Character *c)
{
    // Monster Life = HP * (# of players in game)
    return GetHitPoints(c) * GetNumberOfPlayers();
}

float GetMonsterExperience(Character *c)
{
    // Monster exp  = Base Exp + (# of players - 1)*((2/3)*Base Exp)
    float xp = GetExperience(c);
    return xp + (GetNumberOfPlayers() - 1) * (2.f / 3.f) * xp;
}

float GetGainedExperienceUnadjusted(Character *killer, Character *killed)
{
    // Exp gained = Monster's Exp * Exp fraction
    // Exp fraction = Your level / (total of all party member levels)
    return GetMonsterExperience(killed) * (GetLevel(killer) / GetSumOfPlayersLevels());
}

float GetGainedExperience(Character *killer, Character *killed)
{
    float killedLevel = GetLevel(killed),
          killerLevel = GetLevel(killer),
          xp          = GetGainedExperienceUnadjusted(killer, killed);
    if (killedLevel > killerLevel)
        return xp * (killerLevel / killedLevel);
    static float scale[] = { 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 
                             0.8f, 0.6f, 0.4f, 0.2f, 0.05f };
    float factor = 0.05f;
    int difference = abs(GetLevel(killed) - GetLevel(killer));
    if (difference <= 10)
        factor = scale[difference];
    return xp * factor;
}
