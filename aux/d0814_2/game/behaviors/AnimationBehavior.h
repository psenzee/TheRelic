#ifndef _ANIMATIONBEHAVIOR_H
#define _ANIMATIONBEHAVIOR_H

#include "core/core.h"
#include "math/Filters.h"
#include "game/IBehavior.h"

class AnimationBehavior : public AbstractBehavior
{
public:

    CLASS_NEW_DELETE()

    enum { TYPE = 1003 };
    
    const float MIN_TURN_SPEED,
                MIN_WALK_SPEED,
                MIN_RUN_SPEED,
                MAX_WALK_SPEED,
                TURN_DIVISOR; // lower is faster

    AnimationBehavior();

    void        Start(Character *self) { SetCharacter(self); }
    void        Update(const GameTime &time);

    void        Signal(int signal);

    void        SetForce(const Vector3 &v);
    void        SetAction(const char *value, bool reset = true);

    bool        IsInAttack() const;

    inline void SetCanMove(bool value) { mCanMove = value; }
    inline bool CanMove() const        { return mCanMove; }

    void         Turn(float angle, float force);

    void Serialize(IOutStream &s) const
    {
        // $TODO
    }

    static IBehavior *Deserialize(IInStream &s);
     
private:

    String      mTurnName,
                mMovementName,
                mActionName;
    bool        mTurn,
                mCanMove,
                mIsAlive,
                mInitialized;
    int         mDisableMoveCount;
    float       mAngle,
                mTurnAngle,           
                mTurnForce;
    FilterValue mAccumulatedForce,
                mAccumulatedTurnForce,
                mAccumulatedCrossProduct,
                mAccumulatedForceChange;

    Vector3      mDirection;
//  FilterValue3 mCenter;
    AverageQueueValue<Vector3, 20> mCenter;
    bool         mActionReset;
        
    const char  *GetAnimationName() const;

    inline bool  IsTurning() const { return mTurn; }
    void         ClearTurn();

    void         SetTurnType(const char *value);
    void         SetMovementType(const char *value);
       
    void         SetAppropriateAnimation();
    void         UpdateTurn(float force);
    void         Update();
//  void         Turn(float angle, float force);

    void         HandleCollision(const Vector3 &pos, const Vector3 &newpos, bool uncollideAll);
    void         UncollideVisible();
    void         UpdateAccumulatedForce(float force);
    float        GetTurnLean();
    
    static float GetOrientationAngle(const Vector3 &dir);
};

#endif // _ANIMATIONBEHAVIOR_H