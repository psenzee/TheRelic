/*
 *  Collider_.h
 *  GLGravity
 *
 *  Created by Paul Senzee on 12/17/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */


/*
 */

#ifndef _COLLIDER_H
#define _COLLIDER_H

#include "core/xna.h"

#include <vector>

class CollidableBox;
class CollidableSphere;

class ICollidable
{
public:
    virtual ~ICollidable() {}
        
    virtual bool Collide(const ICollidable &u)           const = 0;
        
    virtual bool CollideBox(const CollidableBox &u)       const = 0;
    virtual bool CollideSphere(const CollidableSphere &u) const = 0;
};

class CollidableBox : public ICollidable
{
public:
        
    Vector3 minimum, maximum;
        
    inline Vector3 GetSize() const { return maximum - minimum; }
        
    CollidableBox(const Vector3 &a, const Vector3 &b);
        
    bool Collide(const ICollidable &u)            const;
    bool CollideBox(const CollidableBox &u)       const;
    bool CollideSphere(const CollidableSphere &u) const;
};

class CollidableSphere : public ICollidable
{
public:
        
    Vector3 center;
    float   radius;
        
    inline CollidableSphere(const Vector3 &c, float r)
    {
        center = c; radius = r;
    }
        
    bool Collide(const ICollidable &u)            const;
    bool CollideBox(const CollidableBox &u)       const;
    bool CollideSphere(const CollidableSphere &u) const;
};

class Collisions
{
public:
        
    static bool Collide(const CollidableBox &a, const CollidableBox &b);
    static bool Collide(const CollidableSphere &a, const CollidableBox &b);
    static bool Collide(const CollidableSphere &a, const CollidableSphere &b);
        
private:
        
    static inline float sq(float x) { return x * x; }
};   

class Collider : ICollidable
{
public:
        
    Collider(std::vector<ICollidable *> &colliders);
    
    bool Collide(const ICollidable &u)  const;
    bool CollideBox(const CollidableBox &u)       const;
    bool CollideSphere(const CollidableSphere &u) const;
        
private:
        
    std::vector<ICollidable *> colliders;
};

#endif  // _COLLIDER_H