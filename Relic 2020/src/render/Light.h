#ifndef _LIGHT_H
#define _LIGHT_H

#include "core/core.h"
#include "fast/Allocator.h"

class Light
{
public:
    
    CLASS_NEW_DELETE()
    
    size_t  id;
    bool    enabled;
    Vector4 position;
    Vector4 ambient,
            diffuse,
            specular;
    float   shininess;

    inline Light(const Light &u) : id(u.id), enabled(u.enabled), position(u.position), ambient(u.ambient), diffuse(u.diffuse), specular(u.specular), shininess(u.shininess) {}
    inline Light(size_t id = 0,
                 bool enabled = false,
                 const Vector4 &position = Vector4(0, 0, 0, 0),
                 const Vector4 &ambient  = Vector4(0, 0, 0, 0),
                 const Vector4 &diffuse  = Vector4(0, 0, 0, 0),
                 const Vector4 &specular = Vector4(0, 0, 0, 0), float shininess = 1.f) :
        id(id), enabled(enabled), position(position), ambient(ambient), diffuse(diffuse), specular(specular), shininess(shininess) {}
};

#endif // _LIGHT_H
