/*
 *  Collider_.cpp
 *  GLGravity
 *
 *  Created by Paul Senzee on 12/17/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */


/*
 */

#include "Collider.h"

CollidableBox::CollidableBox(const Vector3 &a, const Vector3 &b)
{
    minimum = Vector3(math::min(a.x, b.x), math::min(a.y, b.y), math::min(a.z, b.z));
    maximum = Vector3(math::max(a.x, b.x), math::max(a.y, b.y), math::max(a.z, b.z));
}

bool Collisions::Collide(const CollidableBox &a, const CollidableBox &b)
{
    return a.minimum.x < b.maximum.x && a.maximum.x > b.minimum.x &&
           a.minimum.y < b.maximum.y && a.maximum.y > b.minimum.y &&
           a.minimum.z < b.maximum.z && a.maximum.z > b.minimum.z;
}

bool Collisions::Collide(const CollidableSphere &a, const CollidableBox &b)
{
    float dmin = 0.0f;
    if      (a.center.x < b.minimum.x) dmin += sq(a.center.x - b.minimum.x);
    else if (a.center.x > b.maximum.x) dmin += sq(a.center.x - b.maximum.x);
    if      (a.center.y < b.minimum.y) dmin += sq(a.center.y - b.minimum.y);
    else if (a.center.y > b.maximum.y) dmin += sq(a.center.y - b.maximum.y);
    if      (a.center.z < b.minimum.z) dmin += sq(a.center.z - b.minimum.z);
    else if (a.center.z > b.maximum.z) dmin += sq(a.center.z - b.maximum.z);
    return dmin <= (a.radius * a.radius);
}

bool Collisions::Collide(const CollidableSphere &a, const CollidableSphere &b)
{
    return (a.radius + b.radius) > (a.center - b.center).length();
}

bool CollidableBox::Collide(const ICollidable &u) const
{
    return u.CollideBox(*this);
}

bool CollidableBox::CollideBox(const CollidableBox &u) const
{
    return Collisions::Collide(*this, u);
}

bool CollidableBox::CollideSphere(const CollidableSphere &u) const
{
    return Collisions::Collide(u, *this);
}

bool CollidableSphere::Collide(const ICollidable &u) const
{
    return u.CollideSphere(*this);
}

bool CollidableSphere::CollideBox(const CollidableBox &u) const
{
    return Collisions::Collide(*this, u);
}

bool CollidableSphere::CollideSphere(const CollidableSphere &u) const
{
    return Collisions::Collide(*this, u);
}

Collider::Collider(std::vector<ICollidable *> &colliders)
{
    this->colliders = colliders;
}

bool Collider::Collide(const ICollidable &u) const
{
    std::vector<ICollidable *>::const_iterator i = colliders.begin(), e = colliders.end();
    for (; i != e; ++i)
        if ((*i)->Collide(u))
            return true;
    return false;
}

bool Collider::CollideBox(const CollidableBox &u) const
{ 
    return Collide(u);
}

bool Collider::CollideSphere(const CollidableSphere &u) const
{ 
    return Collide(u);
}
