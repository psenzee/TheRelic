#ifndef _XMLREADCONTEXT_H
#define _XMLREADCONTEXT_H

class ContentLoader;
class Collider;
class IAnimator;
class Event;
class Texture;
class Drawable;
class ObjectList;
class IGameObject;
class XmlElement;
class ICollidable;

#include "core/core.h"
#include "fast/Allocator.h"
#include <map>
#include <set>

class XmlReadContext
{
public:
    
    CLASS_NEW_DELETE()

    ContentLoader                   *loader;

    std::map<String, Event       *>  events;
    std::map<String, Texture     *>  textures;
    std::map<String, ICollidable *>  colliders;
    std::map<String, IGameObject *>  objects;
    std::set<XmlElement *>           xml;

    XmlReadContext(ContentLoader *loader) : loader(loader) {}
};

#endif // _XMLREADCONTEXT_H