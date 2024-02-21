#ifndef _XMLOBJECTREADER_H
#define _XMLOBJECTREADER_H

#include "core/xna.h"

#include "render/Transform.h"

class XmlElement;
class XmlReadContext;

class Collider;
class CollidableBox;
class CollidableSphere;
class IAnimator;
class Event;
class Texture;
class RenderStates;
class IModel;
class Renderable;

#include <vector>

class RenderObjectLists
{
public:
    std::vector<Texture *>    textures;
    std::vector<IAnimator *>  animators;
    std::vector<Collider *>   colliders;
    std::vector<IModel *>     models;
    std::vector<Renderable *> renderables;
};

class XmlObjectReader
{
public:

    static Vector3                    ReadVector3(XmlElement *xml, const char *name0, const char *name1, const char *name2);
    static Vector3                    ReadPoint3(XmlElement *xml);
    static Vector3                    ReadSize3(XmlElement *xml);
    
    static Vector2                    ReadVector2(XmlElement *xml, const char *name0, const char *name1);
    static Vector2                    ReadPoint2(XmlElement *xml);
    static Vector2                    ReadSize2(XmlElement *xml);    

    static CollidableBox             *ReadBox(XmlElement *xml);
    static CollidableSphere          *ReadSphere(XmlElement *xml);

    static Collider                  *ReadCollider(XmlElement *xml, XmlReadContext *context);
    static std::vector<Collider *>    ReadColliders(XmlElement *xml, XmlReadContext *context);

    static IAnimator                 *ReadAnimator(XmlElement *xml, XmlReadContext *context);
    static std::vector<IAnimator *>   ReadAnimators(XmlElement *xml, XmlReadContext *context);

    static Event                     *ReadEvent(XmlElement *xml, XmlReadContext *context);
    static std::vector<Event *>       ReadEvents(XmlElement *xml, XmlReadContext *context);

    static Texture                   *ReadTexture(XmlElement *xml, XmlReadContext *context);
    static std::vector<Texture *>     ReadTextures(XmlElement *xml, XmlReadContext *context);

    static Transform                 *ReadTransform(XmlElement *xml);
    static Transform                  ReadTransformStatic(XmlElement *xml);
    static std::vector<Transform *>   ReadTransforms(XmlElement *xml);
    
    static RenderStates              *ReadStates(XmlElement *xml);

    static IModel                    *ReadModel(XmlElement *xml, XmlReadContext *context);
    static std::vector<IModel *>      ReadModels(XmlElement *xml, XmlReadContext *context);

    static Renderable                *ReadObject(XmlElement *xml, XmlReadContext *context);
    static std::vector<Renderable *>  ReadObjects(XmlElement *xml, XmlReadContext *context);

    static void                       Read(XmlElement *xml, XmlReadContext *context, std::vector<Renderable *> &renderable);
    static std::vector<Renderable *>  ReadText(const char *filename, XmlReadContext *context);
    static std::vector<Renderable *>  ReadFile(const char *filename, XmlReadContext *context);
        
    static bool                       ReadInOrder(XmlElement *xml, XmlReadContext *context, RenderObjectLists &lists);
    static bool                       ReadInOrderFile(const char *filename, XmlReadContext *context, RenderObjectLists &lists);
};

#endif // _XMLOBJECTREADER_H