/*
 *  Visibility.cpp
 *  GLGravity
 *
 *  Created by Paul Senzee on 1/2/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#include "Visibility.h"

bool Visibility::IsVisible(const Matrix &viewproj, const Matrix &transform, const AABox &bounds)
{
    // naive but simple and quick visibility check
    // simply transforms each of the eight points of the bounding box and checks to see if they're in the view volume
    // will return false for cases where the bounding box intersects the frustum but no vertices are within the frustum
    // these cases should be rare or nonexistent in the current game
    
    Matrix vt(transform * viewproj);
    Vector3 v[] = { bounds.minimum, bounds.maximum };
    
    static Vector2 minrange(-1.05f, -1.05f), maxrange(1.05f, 1.05f);
    
    for (int i = 0; i < 8; i++)
    {
        Vector3 u(v[i & 1].x, v[(i & 2) >> 1].y, v[(i & 4) >> 2].z);
        u = vt * u; u /= u.z;
        if (u.x >= minrange.x && u.x <= maxrange.x && u.y >= minrange.y && u.y <= maxrange.y)
            return true;
    }
    return false;
}