#ifndef _ROLEPLAYER_H
#define _ROLEPLAYER_H

#include "core/core.h"
#include "core/random.h"
#include "game/IBehavior.h"
#include "game/IRolePlayer.h"

class RolePlayer : public AbstractBehavior, public IRolePlayer
{
public:

    CLASS_NEW_DELETE()

    enum { TYPE = 1009 };

    RolePlayer();

    void     Start(Character *self);
    void     Update(const GameTime &time);
        
    void     SetStrength  (unsigned value);
    void     SetDexterity (unsigned value);
    void     SetVitality  (unsigned value);
    void     SetEnergy    (unsigned value);

    void     SetLuck      (float value);
    
    unsigned GetStrength  () const;
    unsigned GetDexterity () const;
    unsigned GetVitality  () const;
    unsigned GetEnergy    () const;

    float    GetLuck      () const;

    void     SetDefaultAttributes();

    float    Random(float undesirable, float desirable);
    float    GetClassBonus() const;
    float    GetRatingBonus() const;
    float    GetAttackRating() const;    
    float    GetDefenseRating() const;
    
    float    GetChanceToHit(Character *defender);

	void     SetOverallAttackMultiplier(float value) { mOverallAttackMultiplier = value; }
	float    GetOverallAttackMultiplier() const      { return mOverallAttackMultiplier; }

    core::Random &GetRandom() { return mRandom; }

    void     Serialize(IOutStream &s) const
    {
        // $TODO
    }

    static IBehavior *Deserialize(IInStream &s);

private:

    core::Random mRandom;

    unsigned mStrength,
             mDexterity, // another name? agility?
             mVitality,  // another name? endurance?
             mEnergy;    // intelligence?
             
    float    mLuck,  // luck = 1.f certainty of desirable outcome, luck of 0.0f = neutral, luck of -1.f certainty of undesirable outcome
             mClassBonus,
             mRatingBonus,
			 mOverallAttackMultiplier;
};

int        GetNumberOfPlayers();
Character *GetPlayer(int index);
float      GetPlayerExperienceFromLevel(int level);
float      GetDefeatedExperienceFromLevel(int level);
int        GetLevelFromPlayerExperience(float xp);
int        GetLevelFromDefeatedExperience(float xp);
int        GetSumOfPlayersLevels();
float      GetMonsterLife(Character *c);
float      GetMonsterExperience(Character *c);
float      GetGainedExperienceUnadjusted(Character *killer, Character *killed);
float      GetGainedExperience(Character *killer, Character *killed);

#endif // _ROLEPLAYER_H