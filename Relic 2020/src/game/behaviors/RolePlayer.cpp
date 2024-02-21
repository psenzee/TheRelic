#include "RolePlayer.h"
#include "Behaviors.h"

enum { MAX_LEVELUP = 128 };

RolePlayer::RolePlayer() : AbstractBehavior(TYPE), mRandom(0), mStrength(0), mDexterity(0), mVitality(0), mEnergy(0), mLuck(0.f), mClassBonus(0.f), mRatingBonus(0.f), mOverallAttackMultiplier(1.0f)
{
}

void RolePlayer::Start(Character *self)
{ 
    SetCharacter(self);
}

void RolePlayer::Update(const GameTime &time)
{
}
    
void RolePlayer::SetStrength(unsigned value)
{ 
    mStrength = value;
}

void RolePlayer::SetDexterity(unsigned value)
{ 
    mDexterity = value;
}

void RolePlayer::SetVitality(unsigned value)
{ 
    mVitality = value;
}

void RolePlayer::SetEnergy(unsigned value)
{ 
    mEnergy = value;
}

void RolePlayer::SetLuck(float value)
{ 
    mLuck = value;
}

unsigned RolePlayer::GetStrength() const
{
    return mStrength;
}

unsigned RolePlayer::GetDexterity() const
{ 
    return mDexterity;
}

unsigned RolePlayer::GetVitality() const
{ 
    return mVitality;
}

unsigned RolePlayer::GetEnergy() const
{ 
    return mEnergy;
}

float RolePlayer::GetLuck() const
{ 
    return mLuck;
}

void RolePlayer::SetDefaultAttributes()
{
    // default barbarian attributes..
    SetStrength(30);
    SetDexterity(20);
    SetVitality(25);
    SetEnergy(10);
    
    SetLuck(0.f); // -1.f to 1.f
}

float RolePlayer::Random(float undesirable, float desirable) 
{
    float value = math::clamp(mRandom.realf() + mLuck, 0.f, 1.f);
    return (desirable - undesirable) * value + undesirable;
}

float RolePlayer::GetClassBonus() const
{ 
    return mClassBonus;
}

float RolePlayer::GetRatingBonus() const
{ 
    return mRatingBonus;
}

float RolePlayer::GetAttackRating() const
{
    // Base Attack Rating
    // (Your attack rating without +AR or +Ar% from equipment, charms or skills)
    // = (Dex * 5) - 35 + ClassBonus 
    return ((GetDexterity() * 5.f) - 35.f + GetClassBonus() + GetRatingBonus()) * GetOverallAttackMultiplier();
}

float RolePlayer::GetDefenseRating() const
{
    // Base Defense Rating
    // -------------
    // (Your Defense rating without Defense, +Def or +Def% from equipment, charms or skills)
    // = Floor(Dex / 4)
    return floorf(GetDexterity() * 0.25f);
}

float RolePlayer::GetChanceToHit(Character *defender)/* const*/
{
    // Clamp(100 * AR / (AR + DR) * 2 * alvl / (alvl + dlvl), 5, 95)
    float ar   = GetAttackRating(),        dr   = GetDefenseRating();
    float alvl = GetLevel(GetCharacter()), dlvl = GetLevel(defender);
    return math::clamp(100.f * ar / (ar + dr) * 2.f * alvl / (alvl + dlvl), 5.f, 95.f);
}
/*
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
*/