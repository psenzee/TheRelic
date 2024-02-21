/*
 *  Light.h
 *  GLGravity
 *
 *  Created by Paul Senzee on 1/1/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _LIGHT_H
#define _LIGHT_H

#include "core/core.h"

class Light
{
public:
    
    int     id;
    Vector3 position;
    Vector4 ambient,
            diffuse,
            specular;
    float   shininess;

    Light();
    Light(int id, const Vector3 &position,
                  const Vector4 &ambient,
                  const Vector4 &diffuse,
                  const Vector4 &specular, float shininess);
};

#endif // _LIGHT_H