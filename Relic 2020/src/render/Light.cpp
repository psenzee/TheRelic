#include "Light.h"

Light::Light() : id(0), shininess(1.0f)
{
}

Light::Light(int id, const Vector4 &position, const Vector4 &ambient, const Vector4 &diffuse, const Vector4 &specular, float shininess) 
  : position(position), ambient(ambient), diffuse(diffuse), specular(specular), shininess(shininess)
{
}