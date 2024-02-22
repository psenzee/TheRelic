#include "ITileMap.h"
#include "ObjectTile.h"
#include "gamecore/IGameObject.h"
#include "gamecore/IEventList.h"
#include "events/Event.h"
#include "core/strs.h"

static bool HasEvent(IGameObject *object, const char *name)
{
    if (!object)
        return false;

    IEventList  &elist  = object->GetEventList();
    Event      **events = elist.GetEvents();
    int          count  = elist.GetEventCount();

    for (int i = 0; i < count; i++)
        if (starts(events[i]->text.c_str(), name))
            return true;

    return false;
}

ObjectTile::ObjectTile(IGameObject *object, int id) : mObject(object), mId(id)
{
}

ObjectTile::~ObjectTile()
{
}

IGameObject *ObjectTile::GetGameObject()
{
    return mObject;
}

bool ObjectTile::HasEvent(const char *name) const
{
    return mObject ? ::HasEvent(mObject, name) : false;
}

int ObjectTile::GetId() const
{
    return mId;
}

void ObjectTile::Update(const GameTime &time)
{
    if (mObject)
        mObject->Update(time);
}
