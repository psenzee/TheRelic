#include "ITileMap.h"
#include "ObjectTile.h"
#include "SimpleTileInstance.h"
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

ObjectTile::ObjectTile(IGameObject *object, int id) : object(object), shared(0), id(id)
{
}

ObjectTile::~ObjectTile()
{
    if (shared)
        shared->Release();
    // $todo?
}

IGameObject *ObjectTile::GetGameObject()
{
    return object;
}

bool ObjectTile::HasEvent(const char *name) const
{
    return object ? ::HasEvent(object, name) : false;
}

int ObjectTile::GetId() const
{
    return id;
}

ITileInstance *ObjectTile::CreateTileInstance()
{
    if (!shared)
        shared = new SimpleTileInstance(this);
    shared->Retain();
    return shared;
}

void ObjectTile::Update(const GameTime &time)
{
    object->Update(time);
}
