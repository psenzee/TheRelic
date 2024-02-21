/*
 *  Material.cpp
 *  GLGravity
 *
 *  Created by Paul Senzee on 1/1/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#include "Material.h"
#include "GLUtils.h"

Material::Material()
: ambient(0.2f, 0.2f, 0.2f, 1.0f), diffuse(1.0f, 1.0f, 1.0f, 1.0f),
  emissive(0.0f, 0.0f, 0.0f, 0.0f), specular(1.0f, 1.0f, 1.0f, 1.0f), shininess(100.0f)
{
}

Material::Material(const Vector4 &ambient, const Vector4 &diffuse, const Vector4 &emissive, const Vector4 &specular, float shininess)
  : ambient(ambient), diffuse(diffuse), emissive(emissive), specular(specular), shininess(shininess)
{
}

void Material::Set(GraphicsDevice &device)
{
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT,   (GLfloat *)&ambient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE,   (GLfloat *)&diffuse);
//    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION,  (GLfloat *)&emissive);        
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR,  (GLfloat *)&specular);
    glMaterialf (GL_FRONT_AND_BACK, GL_SHININESS, (GLfloat)shininess);
}