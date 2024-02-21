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

#include <map>
#include <string>
#include <set>

class XmlReadContext
{
public:

    ContentLoader                        *loader;

    std::map<std::string, Event       *>  events;
    std::map<std::string, Texture     *>  textures;
    std::map<std::string, ICollidable *>  colliders;
    std::map<std::string, IGameObject *>  objects;
    std::set<XmlElement *>                xml;

    XmlReadContext(ContentLoader *loader) : loader(loader) {}
};

#endif // _XMLREADCONTEXT_H