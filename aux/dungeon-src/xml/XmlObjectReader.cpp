#include "XmlObjectReader.h"

#include "core/xna.h"
#include "core/format.h"
#include "core/file.h"
#include "core/global.h"

#include "XmlUtil.h"
#include "XmlReadContext.h"

#include "math/Collider.h"
#include "render/Animator.h"
#include "events/Event.h"
#include "render/Texture.h"
#include "render/RenderStates.h"
#include "render/Transform.h"
#include "render/Models.h"
#include "render/Renderable.h"

#include <string.h>
#include <vector>

static int IndexOf(const char *s, const char **list)
{
    const char **p = list;
    for (int i = 0; *p; i++, p++)
        if (strcmp(s, *p) == 0)
            return i;
    return -1;        
}

// Here type T is assumed to have a method:
// bool SetProperty(const char *key, const char *value);
template <typename T>
void SetProperties(T &object, XmlElement *xml, const char **ignore)
{
    if (!xml) return;
    for (int i = 0, sz = xml->attributeCount; i < sz; i++)
    {
        XmlAttribute *a = xml->attributes[i];
        if (IndexOf(a->key, ignore) == -1 && !object.SetProperty(a->key, a->value))
            printf("Property %s=%s unrecognized here\n", a->key, a->value);
    }    
}

Vector3 XmlObjectReader::ReadVector3(XmlElement *xml, const char *name0, const char *name1, const char *name2)
{
    return Vector3(XmlUtil::GetFloat(xml, name0), XmlUtil::GetFloat(xml, name1), XmlUtil::GetFloat(xml, name2));
}

Vector3 XmlObjectReader::ReadPoint3(XmlElement *xml)
{
    return ReadVector3(xml, "x", "y", "z");
}

Vector3 XmlObjectReader::ReadSize3(XmlElement *xml)
{
    return ReadVector3(xml, "width", "height", "depth");
}

Vector2 XmlObjectReader::ReadVector2(XmlElement *xml, const char *name0, const char *name1)
{
    return Vector2(XmlUtil::GetFloat(xml, name0), XmlUtil::GetFloat(xml, name1));
}

Vector2 XmlObjectReader::ReadPoint2(XmlElement *xml)
{
    return ReadVector2(xml, "x", "y");
}

Vector2 XmlObjectReader::ReadSize2(XmlElement *xml)
{
    return ReadVector2(xml, "width", "height");
}

CollidableBox *XmlObjectReader::ReadBox(XmlElement *xml)
{
    return new CollidableBox(ReadPoint3(xml), ReadSize3(xml));
}

CollidableSphere *XmlObjectReader::ReadSphere(XmlElement *xml)
{
    return new CollidableSphere(ReadPoint3(xml), XmlUtil::GetFloat(xml, "radius"));
}

Collider *XmlObjectReader::ReadCollider(XmlElement *xml, XmlReadContext *context)
{
    if (strcmp(xml->name, "collider") != 0) 
        return 0;
    const char *name = XmlUtil::GetAttributeValue(xml, "name");
    if (name && strlen(name) > 0)
    {
        Collider *u = context->colliders[name];
        if (u) return u;
    }
    std::vector<ICollidable *> colliders;
    for (int i = 0, sz = xml->childrenCount; i < sz; i++)
    {
        XmlElement *e = xml->children[i];
        if      (strcmp(e->name, "box") == 0)
            colliders.push_back(ReadBox(e));
        else if (strcmp(e->name, "sphere") == 0)
            colliders.push_back(ReadSphere(e));
    }
    Collider *o = new Collider(colliders);
    if (name)
        context->colliders[name] = o;
    return o;
}

std::vector<Collider *> XmlObjectReader::ReadColliders(XmlElement *xml, XmlReadContext *context)
{
    std::vector<Collider *> objects;
    for (int i = 0, sz = xml->childrenCount; i < sz; i++)
        if (strcmp(xml->children[i]->name, "collider") == 0)
            objects.push_back(ReadCollider(xml->children[i], context));
    return objects;
}

IAnimator *XmlObjectReader::ReadAnimator(XmlElement *xml, XmlReadContext *context)
{
    if (strcmp(xml->name, "animator") != 0)
        return 0;
    const char *name = XmlUtil::GetAttributeValue(xml, "name");
    if (name && strlen(name) > 0)
    {
        IAnimator *u = context->animators[name];
        if (u) return u;
    }
    const char *type = XmlUtil::GetAttributeValue(xml, "type");
    IAnimator *o = 0;
    if (strcmp(type, "Rotate") == 0) o = new RotateAnimator(ReadPoint3(xml));
    else                             o = new NullAnimator();
    if (name && o)
        context->animators[name] = o;
    return o;
}

std::vector<IAnimator *> XmlObjectReader::ReadAnimators(XmlElement *xml, XmlReadContext *context)
{
    std::vector<IAnimator *> objects;
    for (int i = 0, sz = xml->childrenCount; i < sz; i++)
        if (strcmp(xml->children[i]->name, "animator") == 0)
            objects.push_back(ReadAnimator(xml->children[i], context));
    return objects;
}

Event *XmlObjectReader::ReadEvent(XmlElement *xml, XmlReadContext *context)
{
    if (strcmp(xml->name, "event") != 0) 
        return 0;
    const char *name = XmlUtil::GetAttributeValue(xml, "name");
    if (name && strlen(name) > 0)
    {
        Event *u = context->events[name];
        if (u) return u;
    }
    Event *o = new Event(XmlUtil::GetText(xml));
    if (name)
        context->events[name] = o;
    return o;
}

std::vector<Event *> XmlObjectReader::ReadEvents(XmlElement *xml, XmlReadContext *context)
{
    std::vector<Event *> objects;
    for (int i = 0, sz = xml->childrenCount; i < sz; i++)
        if (strcmp(xml->children[i]->name, "event") == 0)
            objects.push_back(ReadEvent(xml->children[i], context));
    return objects;
}

Texture *XmlObjectReader::ReadTexture(XmlElement *xml, XmlReadContext *context)
{
    if (strcmp(xml->name, "texture") != 0) 
        return 0;
    const char *name = XmlUtil::GetAttributeValue(xml, "name");
    if (name && strlen(name) > 0)
    {
        Texture *u = context->textures[name];
        if (u) return u;
    }
    Texture *o = new Texture(context->content,
                             XmlUtil::GetExpectedAttributeValue(xml, "file"),
                             XmlUtil::GetAttributeValue(xml, "blend-source"),
                             XmlUtil::GetAttributeValue(xml, "blend-destination"));                             
    if (name)
        context->textures[name] = o;
    return o;
}

std::vector<Texture *> XmlObjectReader::ReadTextures(XmlElement *xml, XmlReadContext *context)
{
    std::vector<Texture *> objects;
    for (int i = 0, sz = xml->childrenCount; i < sz; i++)
        if (strcmp(xml->children[i]->name, "texture") == 0)
            objects.push_back(ReadTexture(xml->children[i], context));
    return objects;
}

Transform *XmlObjectReader::ReadTransform(XmlElement *xml)
{
    if (strcmp(xml->name, "transform") != 0) 
        return new Transform(); // identity..
    // $todo
    return new Transform(); 
}

Transform XmlObjectReader::ReadTransformStatic(XmlElement *xml)
{
    if (strcmp(xml->name, "transform") != 0) 
        return Transform(); // identity..
    // $todo
    return Transform(); 
}

std::vector<Transform *> XmlObjectReader::ReadTransforms(XmlElement *xml)
{
    std::vector<Transform *> objects;
    for (int i = 0, sz = xml->childrenCount; i < sz; i++)
        if (strcmp(xml->children[i]->name, "transform") == 0)
            objects.push_back(ReadTransform(xml->children[i]));
    return objects;
}

RenderStates *XmlObjectReader::ReadStates(XmlElement *xml)
{
    RenderStates *states = new RenderStates();
    states->DepthBias   = XmlUtil::GetFloat(xml, "depth-bias",   0.0f);
    states->Transparent = XmlUtil::GetBool (xml, "transparent",  false);
    states->DepthEnable = XmlUtil::GetBool (xml, "depth-enable", false);    
    return states;
}

IModel *XmlObjectReader::ReadModel(XmlElement *xml, XmlReadContext *context)
{
    if (strcmp(xml->name, "model") != 0) 
        return 0;
    const char *name = XmlUtil::GetAttributeValue(xml, "name");
    if (name && strlen(name) > 0)
    {
        IModel *u = context->models[name];
        if (u) return u;
    }
    const char *file = XmlUtil::GetExpectedAttributeValue(xml, "file");
    std::vector<IAnimator *> animators = ReadAnimators(xml, context);
    std::vector<Collider  *> colliders = ReadColliders(xml, context);    
    RenderableModel *o = new RenderableModel(context->content, 
                                             name, file, ReadTextures(xml, context),
                                                            /*ReadTransforms(xml),*/         ReadTransformStatic(xml),
                                                            animators.empty() ? 0 : animators[0],
                                                            colliders.empty() ? 0 : colliders[0],
                                                            ReadStates(xml));
    const char *ignore[] = { "name", "file", 0 };
    SetProperties(*o, xml, ignore);
    if (name)
        context->models[name] = o;
    return o;
}

std::vector<IModel *> XmlObjectReader::ReadModels(XmlElement *xml, XmlReadContext *context)
{
    std::vector<IModel *> objects;
    for (int i = 0, sz = xml->childrenCount; i < sz; i++)
        if (strcmp(xml->children[i]->name, "model") == 0)
            objects.push_back(ReadModel(xml->children[i], context));
    return objects;
}

Renderable *XmlObjectReader::ReadObject(XmlElement *xml, XmlReadContext *context)
{
    if (strcmp(xml->name, "object") != 0) 
        return 0;
    const char *name = XmlUtil::GetAttributeValue(xml, "name");
    if (name && strlen(name) > 0)
    {
        Renderable *u = context->objects[name];
        if (u) return u;
    }
    const char *refers = XmlUtil::GetAttributeValue(xml, "refers");
    Renderable *o = 0;
    if (!refers)
    {
        o = new Renderable(name, ReadModels(xml, context),
                                 ReadTransformStatic(xml),
                                 ReadEvents(xml, context));
    }
    else
    {
        Renderable *u = context->objects[refers];
        if (!u)
            globalErrorHandler("Definition of <object> '%s' refers to undefined object '%s'", name, refers);
        o = u;
    }
    if (name)
        context->objects[name] = o;
    return o;
}

std::vector<Renderable *> XmlObjectReader::ReadObjects(XmlElement *xml, XmlReadContext *context)
{
    std::vector<Renderable *> objects;
    for (int i = 0, sz = xml->childrenCount; i < sz; i++)
        if (strcmp(xml->children[i]->name, "object") == 0)
            objects.push_back(ReadObject(xml->children[i], context));
    return objects;
}

void XmlObjectReader::Read(XmlElement *xml, XmlReadContext *context, std::vector<Renderable *> &renderable)
{
    ReadTextures  (xml, context);
    ReadAnimators (xml, context);
    ReadColliders (xml, context);
    ReadModels    (xml, context);
    renderable = ReadObjects(xml, context);
}

bool XmlObjectReader::ReadInOrder(XmlElement *xml, XmlReadContext *context, RenderObjectLists &lists)
{
    for (int i = 0, sz = xml->childrenCount; i < sz; i++)
    {
        XmlElement *element = xml->children[i];
        const char *name    = element->name;
        if      (strcmp(name, "include") == 0)
        {
            const char *file = XmlUtil::GetExpectedAttributeValue(element, "file");
            if (!file)
                return false;
            if (!ReadInOrderFile(file, context, lists))
                return false;
        }
        else if (strcmp(name, "texture") == 0)
            lists.textures.push_back(ReadTexture(element, context));
        else if (strcmp(name, "animator") == 0)
            lists.animators.push_back(ReadAnimator(element, context));
        else if (strcmp(name, "collider") == 0)
            lists.colliders.push_back(ReadCollider(element, context));
        else if (strcmp(name, "model") == 0)
            lists.models.push_back(ReadModel(element, context));
        else if (strcmp(name, "object") == 0)
            lists.renderables.push_back(ReadObject(element, context));
    }
    return true;
}

bool XmlObjectReader::ReadInOrderFile(const char *filename, XmlReadContext *context, RenderObjectLists &lists)
{
    std::string file(file::read_file_string(filename));
    std::vector<Renderable *> r;
    if (file.empty())
    {
        globalErrorHandler("Unable to open xml object file '%s' (XmlObjectReader)\n", filename);
        return false;
    }
    XmlElement *xml = parse(file.c_str());
    bool success = ((xml != 0) && ReadInOrder(xml, context, lists));
    if (xml)
        delete xml;
    return success;
}

std::vector<Renderable *> XmlObjectReader::ReadText(const char *text, XmlReadContext *context)
{
    std::vector<Renderable *> r;
    XmlElement *xml = parse(text);  
    Read(xml, context, r);
    delete xml;
    return r;
}

std::vector<Renderable *> XmlObjectReader::ReadFile(const char *filename, XmlReadContext *context)
{
    // the old way
    /*
    std::string file(file::read_file_string(filename));
    std::vector<Renderable *> r;
    if (file.empty())
    {
        globalErrorHandler("Unable to open xml object file '%s' (XmlObjectReader)\n", filename);
        return r;
    }
    XmlElement *xml = parse(file.c_str());
    Read(xml, context, r);
    delete xml;
    return r;
    */
    // the new way (allow includes)
    RenderObjectLists lists;
    ReadInOrderFile(filename, context, lists);
    return lists.renderables;
}

XmlReadContext *ReadXmlConfigFile(const char *filename)
{
    ContentCache *cache = new ContentCache;
    XmlReadContext *context = new XmlReadContext(cache);
    XmlObjectReader::ReadFile(filename, context);
    return context;
}

XmlReadContext *ReadXmlConfigText(const char *text, XmlReadContext *context)
{
    XmlObjectReader::ReadText(text, context);
    return context;
}

XmlReadContext *ReadXmlConfigText(const char *text)
{
    ContentCache *cache = new ContentCache;
    XmlReadContext *context = new XmlReadContext(cache);
    XmlObjectReader::ReadText(text, context);
    return context;
}
