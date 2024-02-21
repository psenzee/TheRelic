/*
 *  Material.h
 *  GLGravity
 *
 *  Created by Paul Senzee on 1/1/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _MATERIAL_H
#define _MATERIAL_H

#include "core/core.h"

class Material
{
public:
    
    Vector4 ambient,
            diffuse,
            emissive,
            specular;
    
    float   shininess;

    Material();    
    Material(const Vector4 &ambient, const Vector4 &diffuse, const Vector4 &emissive, const Vector4 &specular, float shininess);
};

#endif // _MATERIAL_H