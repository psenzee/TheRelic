#include "core/core.h"
#include "PositionVelocity.h"

Vector3 GetNewVector(PositionVelocity::collider_fn collider, float radius, void *user, const Vector3 &prev, const Vector3 &next)
{
    if (!collider)
        return next;
    static bool inited = false;
    static const float PI2 = 3.14159f * 2.0f,
                       INCREMENT = 0.1f;
    static Matrix L1, L2, R1, R2;
    if (!inited)
    {
        L1.rotationz(PI2 *  INCREMENT); 
        L2.rotationz(PI2 *  INCREMENT * 2.0f); 
        R1.rotationz(PI2 * -INCREMENT); 
        R2.rotationz(PI2 * -INCREMENT * 2.0f);
        inited = true;
    }
    Vector3 throwaway;
    if (!collider(next, radius, throwaway, user))
        return next;
    Vector3 delta(next - prev),
            dL1(L1 * delta + prev), dR1(R1 * delta + prev),
            dL2(L2 * delta + prev), dR2(R2 * delta + prev);
    if (!collider(dL1, radius, throwaway, user)) return dL1;
    if (!collider(dR1, radius, throwaway, user)) return dR1;
    if (!collider(dL2, radius, throwaway, user)) return dL2;
    if (!collider(dR2, radius, throwaway, user)) return dR2;
    return prev;
}

PositionVelocity::collider_fn  PositionVelocity::sCollider;
void                          *PositionVelocity::sUser;

// for now - assumes a constant time step in between calls to SetPosition()

PositionVelocity::PositionVelocity() : mFilter(1.0f)
{
}

PositionVelocity::PositionVelocity(const Vector3 &position) : mPosition(position), mLast(position), mVelocity(), mFilter(0.1f)
{
}

PositionVelocity::PositionVelocity(const PositionVelocity &u) : mPosition(u.mPosition), mLast(u.mLast), mVelocity(u.mVelocity), mFilter(u.mFilter)
{
}

const Vector3 &PositionVelocity::SetPositionRaw(const Vector3 &p)
{
    mPosition = GetNewVector(sCollider, 28.0f, sUser, mPosition, p);
    return mPosition;
}

void PositionVelocity::SetPosition(const Vector3 &p)
{
    Vector3 q(p);
    mLast = mPosition;
    q = SetPositionRaw(p);
    Vector3 d(mPosition - mLast);
    mVelocity = mVelocity * (1.0f - mFilter) + d * mFilter;
    /*
    if (mVelocity.lengthsq() < 0.2f) // if not moving much, then stop
    {
        mVelocity = 0.f;
        mPosition = mLast;
    }
    */
}

void PositionVelocity::SetVelocity(const Vector3 &p)
{
    mVelocity = p;
    /*
    if (mVelocity.lengthsq() < 1.0f) // if not moving much, then stop
       mVelocity = 0.f;
    */
}

void PositionVelocity::SetCollider(collider_fn fn, void *user)
{
    sCollider = fn; sUser = user;
}
