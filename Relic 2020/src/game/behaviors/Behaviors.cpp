#include "Behaviors.h"
#include "KillableBehavior.h"
#include "DriftApproach.h"
#include "AnimationBehavior.h"
#include "DieBehavior.h"
#include "Approach.h"
#include "Attack.h"
#include "RolePlayer.h"
#include "GeneratedRolePlayer.h"
#include "RolePlayRules.h"
#include "game/IRolePlayer.h"

#include "render/ContentLoader.h"
#include "render/DeviceTexture.h"
#include "render/ParticleEffects.h"

#include <stdlib.h> // rand()

// support code
static void CreateCloud(const char *filename, const Vector3 &position, float time, float speed, float lit)
{ 
    DeviceTexture *texture = ContentLoader::GetInstance()->GetTexture(filename);
    if (!texture) printf("Unable to find texture '%s' for effect!\n", filename);
    else
    {
        ParticleSystem *ps = ParticleEffects::GetInstance()->CreateCloud(texture, position, time, speed, false, true, lit);
        (void)ps; // don't use, this is auto-cleanup
    }
}

void DustHit(Character *c)
{
    DustHit(c, c->GetPosition());
}

void DustHit(Character *c, const Vector3 &at)
{
    return CreateCloud("dust_glow_shadow_desat", Vector3(at.x, at.y, -100.f), 500.f, 3.f, false);
}

void BlueGlow(Character *c)
{
    BlueGlow(c, c->GetPosition());
}

void BlueGlow(Character *c, const Vector3 &at)
{
    return CreateCloud("blue_glow_shadow", Vector3(at.x, at.y, -145.f), 500.f, 4.f, true);
}

Vector3 GetMovableDirection(Character *self, const Vector3 &direction)
{
    Vector3 dir(direction);
    while (dir.lengthsq() > 1.f)
    {
        int collides = self->CollidesAt(self->GetPosition() + dir, self->GetRadius());
        if (collides == 0 || collides == 512)
            break;
        dir *= 0.5f; // try half as far, go as far as possible
    }
    return (dir.lengthsq() > 1.f) ? dir : Vector3(0.f, 0.f, 0.f);
}

void Move(Character *self, const Vector3 &direction, bool collide)
{
    Vector3 dir(collide ? GetMovableDirection(self, direction) : direction);
    if (dir.lengthsq() > 1.f)
        self->SetPosition(self->GetPosition() + dir); // raw?
}

float RandomFloat()
{
    const float RAND_INV = 1.f / float(RAND_MAX);
    return rand() * RAND_INV;
}

Vector3 Random3()
{
    return Vector3(RandomFloat(), RandomFloat(), RandomFloat());
}

// KillableBehavior
bool IsKillable(Character *c)
{ 
    return GetKillableBehavior(c) != 0;
}

bool IsDestroyed(Character *c)
{ 
    KillableBehavior *kb = GetKillableBehavior(c);
    return !kb ? false : kb->IsDestroyed();
}

float GetHitPoints(Character *c)
{ 
    KillableBehavior *kb = GetKillableBehavior(c);
    return !kb ? 1.f : kb->GetHitPoints();
}

void SetHitPoints(Character *c, Character *from, float value)
{
    KillableBehavior *kb = GetKillableBehavior(c);
    if (kb) kb->SetHitPoints(value, from);
}

float GetMaxHitPoints(Character *c)
{
    KillableBehavior *kb = GetKillableBehavior(c);
    return !kb ? 1.f : kb->GetMaxHitPoints();
}

void SetMaxHitPoints(Character *c, float value)
{
    KillableBehavior *kb = GetKillableBehavior(c);
    if (kb) kb->SetMaxHitPoints(value);
}

void SetInvulnerable(Character *c, bool value)
{
    KillableBehavior *kb = GetKillableBehavior(c);
    if (kb) kb->SetInvulnerable(value);
}

bool IsInvulnerable(Character *c)
{ 
    KillableBehavior *kb = GetKillableBehavior(c);
    return !kb ? false : kb->IsInvulnerable();
}

void ReceiveHit(Character *c, Character *from, float hp, float distance)
{
    KillableBehavior *kb = GetKillableBehavior(c);
    if (kb) kb->ReceiveHit(hp, distance, from);
}

void ReceiveHitInMotion(Character *c, Character *from, float hp, float distance)
{
    KillableBehavior *kb = GetKillableBehavior(c);
    if (kb) kb->ReceiveHitInMotion(hp, distance, from);
}

float GetKnockSpin(Character *c)
{
    KillableBehavior *kb = GetKillableBehavior(c);
    return kb ? kb->GetKnockSpin() : 0.f;
}

void SetKnockSpin(Character *c, float value)
{
    KillableBehavior *kb = GetKillableBehavior(c);
    if (kb) kb->SetKnockSpin(value);
}

float GetKnockBack(Character *c)
{
    KillableBehavior *kb = GetKillableBehavior(c);
    return kb ? kb->GetKnockBack() : 0.f;
}

void SetKnockBack(Character *c, float value)
{
    KillableBehavior *kb = GetKillableBehavior(c);
    if (kb) kb->SetKnockBack(value);
}

unsigned GetExperience(Character *c)
{
    KillableBehavior *kb = GetKillableBehavior(c);
    return kb ? kb->GetExperience() : 10;
}

void SetExperience(Character *c, unsigned value)
{
    KillableBehavior *kb = GetKillableBehavior(c);
    if (kb) kb->SetExperience(value);
}

int GetLevel(Character *c)
{
    KillableBehavior *kb = GetKillableBehavior(c);
    return kb ? kb->GetLevel() : 1;
}

void SetLevel(Character *c, int value)
{
    KillableBehavior *kb = GetKillableBehavior(c);
    if (kb) kb->SetLevel(value);
}

// AnimationBehavior
bool IsAnimation(Character *c)
{ 
    return GetAnimationBehavior(c) != 0;
}

bool IsInAttack(Character *c)
{
    AnimationBehavior *b = GetAnimationBehavior(c);
    return b ? b->IsInAttack() : false;
}

void SetForce(Character *c, const Vector3 &value)
{
    AnimationBehavior *b = GetAnimationBehavior(c);
    if (b) b->SetForce(value);
}

void SetAttackTypeCount(Character *c, int value)
{
    AnimationBehavior *b = GetAnimationBehavior(c);
    if (b) b->SetAttackTypeCount(value);
}

void SetAction(Character *c, const char *value)
{
    AnimationBehavior *b = GetAnimationBehavior(c);
    if (b) b->SetAction(value);
}

void SetActionReset(Character *c, const char *value)
{
    AnimationBehavior *b = GetAnimationBehavior(c);
    if (b) b->SetAction(value, true);
}

void SetWalkAndAttack(Character *c, bool value)
{
    AnimationBehavior *b = GetAnimationBehavior(c);
    if (b) b->SetWalkAndAttack(value);
}

bool GetWalkAndAttack(Character *c)
{
    AnimationBehavior *b = GetAnimationBehavior(c);
    return b ? b->GetWalkAndAttack() : false;
}

void SetCanMove(Character *c, bool value)
{
    AnimationBehavior *b = GetAnimationBehavior(c);
    if (b) b->SetCanMove(value);
}

bool CanMove(Character *c)
{
    AnimationBehavior *b = GetAnimationBehavior(c);
    return b ? b->CanMove() : false;
}

void Turn(Character *c, float angle, float force)
{
    AnimationBehavior *b = GetAnimationBehavior(c);
    if (b) b->Turn(angle, force);
}

// Approach
bool IsApproach(Character *c)
{ 
    return GetApproach(c) != 0;
}

bool IsApproachActive(Character *c)
{ 
    Approach *b = GetApproach(c);
    return b ? b->IsActive() : false;
}

void SetApproachSpeed(Character *c, float speed)
{
    Approach *b = GetApproach(c);
    if (b) b->SetApproachSpeed(speed);
}

void SetApproachMinDistance(Character *c, float v)
{
    Approach *b = GetApproach(c);
    if (b) b->SetApproachMinDistance(v);
}

void SetApproachTooFar(Character *c, float v)
{
    Approach *b = GetApproach(c);
    if (b) b->SetApproachTooFar(v);
}

void SetApproachCollide(Character *c, bool v)
{
    Approach *b = GetApproach(c);
    if (b) b->SetApproachCollide(v);
}

float GetApproachSpeed(Character *c)
{
    Approach *b = GetApproach(c);
    return b ? b->GetApproachSpeed() : 0.f;
}

float GetApproachMinDistance(Character *c)
{
    Approach *b = GetApproach(c);
    return b ? b->GetApproachMinDistance() : 0.f;
}

float GetApproachTooFar(Character *c)
{
    Approach *b = GetApproach(c);
    return b ? b->GetApproachTooFar() : 0.f;
}

bool GetApproachCollide(Character *c)
{
    Approach *b = GetApproach(c);
    return b ? b->GetApproachCollide() : false;
}

void SetApproachTarget(Character *c, Character *target)
{
    Approach *b = GetApproach(c);
    if (b) b->SetApproachTarget(target);
}

// Attack
bool IsAttack(Character *c)
{ 
    return GetAttack(c) != 0;
}

bool IsAttackActive(Character *c)
{ 
    Attack *b = GetAttack(c);
    return b ? b->IsAttackActive() : false;
}

void SetAttackSpeed(Character *c, float speed)
{
    Attack *b = GetAttack(c);
    if (b) b->SetAttackSpeed(speed);
}

void SetAttackMultiplier(Character *c, float multiplier)
{
    Attack *b = GetAttack(c);
    if (b) b->SetAttackMultiplier(multiplier);
}

float GetAttackMultiplier(Character *c)
{
    Attack *b = GetAttack(c);
    return b ? b->GetAttackMultiplier() : 1.f;
}

void SetMaxAttackDistance(Character *c, float distance)
{
    Attack *b = GetAttack(c);
    if (b) b->SetMaxAttackDistance(distance);
}

void SetAttackTarget(Character *c, Character *target)
{
    Attack *b = GetAttack(c);
    if (b) b->SetAttackTarget(target);
}

void SetAutoAttack(Character *c, bool value)
{
    Attack *b = GetAttack(c);
    if (b) b->SetAutoAttack(value);
}

void TriggerAttack(Character *c)
{
    Attack *b = GetAttack(c);
    if (b) b->TriggerAttack();
}

// die behavior
bool IsDieBehavior(Character *c)
{ 
    return GetDieBehavior(c) != 0;
}

void SetDeathComplete(Character *c, bool complete)
{
    DieBehavior *b = GetDieBehavior(c);
    if (b) b->SetDeathComplete(complete);
}

bool IsDeathComplete(Character *c)
{ 
    DieBehavior *b = GetDieBehavior(c);
    return b ? b->IsDeathComplete() : false;
}

// RolePlayer
void SetStrength(Character *c, unsigned value)
{
    IRolePlayer *b = GetRolePlayer(c);
    if (b) b->SetStrength(value);
}

void SetDexterity(Character *c, unsigned value)
{
    IRolePlayer *b = GetRolePlayer(c);
    if (b) b->SetDexterity(value);
}

void SetVitality(Character *c, unsigned value)
{
    IRolePlayer *b = GetRolePlayer(c);
    if (b) b->SetVitality(value);
}

void SetEnergy(Character *c, unsigned value)
{
    IRolePlayer *b = GetRolePlayer(c);
    if (b) b->SetEnergy(value);
}

void SetLuck(Character *c, float value)
{
    IRolePlayer *b = GetRolePlayer(c);
    if (b) b->SetLuck(value);
}

unsigned GetStrength(Character *c)
{
    IRolePlayer *b = GetRolePlayer(c);
    return b ? b->GetStrength() : 0;
}

unsigned GetDexterity(Character *c)
{
    IRolePlayer *b = GetRolePlayer(c);
    return b ? b->GetDexterity() : 0;
}

unsigned GetVitality(Character *c)
{
    IRolePlayer *b = GetRolePlayer(c);
    return b ? b->GetVitality() : 0;
}

unsigned GetEnergy(Character *c)
{
    IRolePlayer *b = GetRolePlayer(c);
    return b ? b->GetEnergy() : 0;
}

float GetLuck(Character *c)
{
    IRolePlayer *b = GetRolePlayer(c);
    return b ? b->GetLuck() : 0.f;
}

float GetRolePlayerRandom(Character *c, float undesirable, float desirable)
{
    IRolePlayer *b = GetRolePlayer(c);
    return b ? RolePlayRandom(b->GetRandom(), b->GetLuck(), undesirable, desirable) : 0.f;
}

void SetRolePlayerDefaultAttributes(Character *c)
{
    IRolePlayer *b = GetRolePlayer(c);
    if (b) SetDefaultRpgAttributes(c);
//  if (b) b->SetDefaultAttributes();
}

float GetClassBonus(Character *c)
{
    IRolePlayer *b = GetRolePlayer(c);
    return b ? b->GetClassBonus() : 0.f;
}

float GetRatingBonus(Character *c)
{
    IRolePlayer *b = GetRolePlayer(c);
    return b ? b->GetRatingBonus() : 0.f;
}

float GetOverallAttackMultiplier(Character *c)
{
    IRolePlayer *b = GetRolePlayer(c);
    return b ? b->GetOverallAttackMultiplier() : 0.f;
}

void SetOverallAttackMultiplier(Character *c, float value)
{
    IRolePlayer *b = GetRolePlayer(c);
    if (b) b->SetOverallAttackMultiplier(value);
}

/*

float GetAttackRating(Character *c)
{
    IRolePlayer *b = GetRolePlayer(c);
    return b ? GetAttackRating(c) : 0.f;
//  return b ? b->GetAttackRating() : 0.f;
}

float GetDefenseRating(Character *c)
{
    IRolePlayer *b = GetRolePlayer(c);
    return b ? GetDefenseRating(c) : 0.f;
//  return b ? b->GetDefenseRating() : 0.f;
}

float GetChanceToHit(Character *c, Character *defender)
{
    IRolePlayer *b = GetRolePlayer(c);
    return b ? GetChanceToHit(c, defender) : 0.f;
  //return b ? b->GetChanceToHit(defender) : 0.f;
}
*/

bool IsRolePlayer(Character *c)
{ 
    return GetRolePlayer(c) != 0;
}

IRolePlayer *GetRolePlayer(Character *c)
{
    if (!c) return 0;
    RolePlayer *rp = GetRegularRolePlayer(c);
    return rp ? static_cast<IRolePlayer *>(rp) : static_cast<IRolePlayer *>(GetGeneratedRolePlayer(c));
}
