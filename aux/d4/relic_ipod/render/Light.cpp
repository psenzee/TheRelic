/*
 *  Light.cpp
 *  GLGravity
 *
 *  Created by Paul Senzee on 1/1/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#include "Light.h"

Light::Light() : id(0), shininess(1.0f)
{
}

Light::Light(int id, const Vector3 &position, const Vector4 &ambient, const Vector4 &diffuse, const Vector4 &specular, float shininess) 
  : position(position), ambient(ambient), diffuse(diffuse), specular(specular), shininess(shininess)
{
}