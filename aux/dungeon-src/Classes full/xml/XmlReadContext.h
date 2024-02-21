#ifndef _XMLREADCONTEXT_H
#define _XMLREADCONTEXT_H

class ContentCache;
class Collider;
class IAnimator;
class Event;
class Texture;
class IModel;
class Renderable;

#include <map>
#include <string>

class XmlReadContext
{
public:

    ContentCache                        *content; 
    std::map<std::string, IAnimator  *>  animators;
    std::map<std::string, Collider   *>  colliders;
    std::map<std::string, Event      *>  events;
    std::map<std::string, IModel     *>  models;
    std::map<std::string, Texture    *>  textures;
    std::map<std::string, Renderable *>  objects;

    XmlReadContext(ContentCache *cache) : content(cache) {}
};

#endif // _XMLREADCONTEXT_H