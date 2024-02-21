#ifndef _COLLIDER_H
#define _COLLIDER_H

class Tuple3f;

#include <string>

#include "gamecore/ICollidable.h"

class Collider : public ICollidable
{
public:

    typedef ICollidable::Classification Classification;

    Collider() : mData(0) {}
    ~Collider();

    static Collider *ReadCollider     (const char *filename);

    Classification   ClassifyPoint    (const Tuple3f &point)                            const;
    Classification   ClassifySphere   (const Tuple3f &point, float radius, Tuple3f &at) const;
    bool             CollisionOccurred(const Tuple3f &from, const Tuple3f &to)          const;

    void             SetIdentifier(const char *name) { mIdentifier = !name ? "" : name; }
    const char      *GetIdentifier() const { return mIdentifier.c_str(); }

private:

    std::string mIdentifier;
    void *mData;
};

#endif // _COLLIDER_H