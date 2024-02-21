#include "Object.h"
#include "gamecore/IGameObject.h"
#include "gamecore/IRenderable.h"
#include "gamecore/ListUtils.h"
#include "GraphicsDevice.h"

class RenderContext;

Object::Object(std::vector<IGameObject *> objects, const Matrix &transform, std::vector<Event *> events, ICollidable *collider)
    : mRef(1), mObjects(objects), mTransform(transform), mEvents(events), mCollidable(collider)
{
    mBounds = ListUtils::GetBounds(mObjects);
    // $TODO Set Colliders
}

IGameObject *Object::Copy()
{
    this->Retain();
    return this; // $TODO
}

void Object::Retain()
{
    mRef++;        
}

void Object::Release()
{
    if (--mRef == 0)
        delete this;
}