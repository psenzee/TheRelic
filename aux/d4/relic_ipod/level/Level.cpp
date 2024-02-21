#include "Level.h"

#include "core/core.h"
#include "core/random.h"
#include "core/global.h"

#include "map/Map.h"
#include "map/MapLoader.h"
#include "map/MapDescription.h"
#include "map/TileMap.h"
#include "map/ITileMap.h"
#include "map/DynamicMap.h"

#include "render/OverheadCamera.h"
#include "gamecore/IGameObject.h"
#include "gamecore/IEventList.h"
#include "events/Event.h"
#include "events/IEventListener.h"
#include "time/Timer.h"

#include "game/Character.h"
#include "game/Multiplayer.h"
#include "game/Sync.h"

#include "core/strs.h"

class XmlElement;

bool MapCollider(const Vector3 &p, float radius, Vector3 &resolve, void *user);

void GetNamed(std::vector<Map *> &map, const char *name, std::vector<core::Point> &items)
{
    ITileMapModel *model = map[0]->GetMapModel();
    core::Size mapSizeInTiles(model->GetMapSizeInTiles());
    for (int j = 0; j < mapSizeInTiles.height; j++)
    {
        for (int i = 0; i < mapSizeInTiles.width; i++)
        {
            core::Point p(i, j);
            for (std::vector<Map *>::iterator i = map.begin(), e = map.end(); i != e; ++i)
            {
                ITileMapModel *model = (*i)->GetMapModel();
                if (model->GetTile(p)->HasEvent(name))
                {
                    items.push_back(p);
                    break;
                }
            }
        }
    }
}

Character *Level::GetCharacter(int i)
{ 
    return static_cast<Character *>(dynamic->GetMovableById(i));
}

const std::vector<core::Point> &Level::GetItems(const char *name)
{
    Items &items = namedItems[name];
    if (!items.searched && items.items.empty())
    {
        GetNamed(maps, name, items.items);
        items.searched = true;
    }
    return items.items;
}

static Vector2 GetPositionVector2(const Vector3 &player, const Vector3 &map)
{
    return Vector2(player.x - map.x, player.y - map.y);
}

static core::Point GetPositionPoint(const Vector3 &player, const Vector3 &map)
{
    return core::Point(int(player.x - map.x), int(player.y - map.y));
}

ITileInstance *GetTileAt(Map *map, const Vector3 &position)
{
    return TileMaps::GetTileInstanceAtWorld(map->GetMapModel(), GetPositionPoint(position, map->GetPosition()));
}

ITileInstance *Level::GetTileAt(Map *map, const Vector3 &position)
{
    return ::GetTileAt(map, position);
}

static core::Point GetTileLocationAt(Map *map, const Vector3 &position)
{
    return TileMaps::GetTileLocationAtWorld(map->GetMapModel(), GetPositionPoint(position, map->GetPosition()));
}

static int GetTileEventCountAt(std::vector<Map *> &maps, const Vector3 &position)
{
    int count = 0;
    for (std::vector<Map *>::iterator i = maps.begin(), e = maps.end(); i != e; ++i)
    {
        ITileInstance *instance = GetTileAt(*i, position);
        IGameObject *object = instance->GetGameObject();
        IEventList &event = object->GetEventList();
        count += event.GetEventCount();
    }
    return count;
}

static const char *GetTileEventAt(std::vector<Map *> &maps, const Vector3 &position, int index)
{
    int count = 0;
    for (std::vector<Map *>::iterator i = maps.begin(), e = maps.end(); i != e; ++i)
    {
        ITileInstance *instance = GetTileAt(*i, position);
        IGameObject *object = instance->GetGameObject();
        IEventList &event = object->GetEventList();
        count += event.GetEventCount();
        if (index < count)
        {
            Event **list = event.GetEvents();
            return list[index - count]->text.c_str();
        }
    }
    return "";
}

static const char *HasTileEventAt(std::vector<Map *> &maps, const char *name, const Vector3 &position)
{
    int count = 0;
    for (std::vector<Map *>::iterator i = maps.begin(), e = maps.end(); i != e; ++i)
    {
        ITileInstance *instance = GetTileAt(*i, position);
        IGameObject *object = instance->GetGameObject();
        if (object)
        {
            IEventList &event = object->GetEventList();
            count = event.GetEventCount();
            Event **list = event.GetEvents();
            for (int i = 0; i < count; i++)
                if (starts(list[i]->text.c_str(), name))
                    return list[i]->text.c_str();
        }
    }
    return 0;
}

int Level::GetTileEventCountAt(const Vector3 &position)
{
    return ::GetTileEventCountAt(maps, position);
}

const char *Level::GetTileEventAt(const Vector3 &position, int index)
{
    return ::GetTileEventAt(maps, position, index);
} 

const char *Level::HasTileEventAt(const char *name, const Vector3 &position)
{
    return ::HasTileEventAt(maps, name, position);
}

Level::Level(ContentLoader *loader, const char *path, int level, Multiplayer *multiplayer) : dynamic(0), levelNumber(level), mMultiplayer(multiplayer), mSync(0)
{
    options = new LevelOptions(path);
    MapLoader mapLoader;
    for (std::vector<MapDescription>::const_iterator i = options->maps.begin(), e = options->maps.end(); i != e; ++i)
        maps.push_back(mapLoader.Create(*i, loader));
    // create dynamic map
    if (maps.size() > BASE_MAP)
    {
        Map *mainMap = maps[BASE_MAP];
	    core::Size mapSize(mainMap->GetMapModel()->GetMapSizeInPixels()),
		           tileSize(mainMap->GetMapModel()->GetTileSize());
        dynamic = new DynamicMap(Vector2(static_cast<float>(mapSize.width),  static_cast<float>(mapSize.height)),
		                         Vector2(static_cast<float>(tileSize.width), static_cast<float>(tileSize.height)));
    }
    if (mMultiplayer)
        mSync = new CharacterSync(mMultiplayer, dynamic);
}

Vector3 Level::GetSpawnablePoint(int index)
{
    return GetNamedPoint("Spawnable", index);
}

Vector3 Level::GetStartPoint()
{
    return GetNamedPoint("Start", 0);
}

int Level::GetNamedPointCount(const char *name)
{
    const std::vector<core::Point> &items = GetItems(name);
    return static_cast<int>(items.size());
}

Vector3 Level::GetNamedPoint(const char *name, int index)
{
    const std::vector<core::Point> &items = GetItems(name);
    if (items.empty())
        return Vector3();
    core::Size tileSize(maps[BASE_MAP]->GetMapModel()->GetTileSize());
    core::Point p = items[index % items.size()];
    return Vector3(static_cast<float>(p.x * tileSize.width), static_cast<float>(p.y * tileSize.height), 0.f);
}

void Level::AddCharacter(Character *ch)
{
    if (ch)
    {
        ch->SetMap(maps[BASE_MAP]);
        dynamic->GetRegistry().Register(ch->GetId(), ch);
        ch->SetDynamicMap(dynamic);
        PositionVelocity::SetCollider(MapCollider, maps[BASE_MAP]);
    }
}

void Level::RemoveCharacter(Character *ch)
{
    if (ch)
    {
        ch->SetDynamicMap(0);
        dynamic->GetRegistry().Unregister(ch);
    }
}

Level::~Level()
{
    for (std::vector<Map *>::iterator i = maps.begin(), e = maps.end(); i != e; ++i)
        delete *i;
    maps.clear();
    // $TODO iterate through the characters in DynamicMap and remove them
    if (mSync)
        delete mSync;
    delete dynamic;
    dynamic = 0;
    delete options;
    options = 0;
}

core::Size Level::GetSize()
{ 
    return maps[BASE_MAP]->GetMapModel()->GetMapSizeInPixels();
}

static void UpdateDynamicMap(Map *m, DynamicMap *map, const GameTime &time)
{
    DynamicMapIterator iterator(map, m->GetRenderRange());
    while (iterator.HasNext())
    {
        Character *ch = (Character *)iterator.GetNext();
        ch->Update(time);
    }
}
        
void Level::Update(RenderContext &context, const GameTime &time, bool paused)
{
    if (!paused && mSync)
        mSync->Update();
    for (std::vector<Map *>::iterator i = maps.begin(), e = maps.end(); i != e; ++i)
        if (*i) (*i)->Update(context, time);
    if (!paused)
    {
        UpdateDynamicMap(maps[BASE_MAP], dynamic, time);
        if (mSync)
            mSync->SendCharacterUpdates(maps[BASE_MAP]);
    }
}

bool Level::UncollideVisible()
{
    return dynamic->Uncollide(maps[BASE_MAP]->GetRenderRange());
}

int Level::GetFreeCharacterId() const
{ 
    enum { LOWEST_RESERVED_CHARID = 32 };
    int id = dynamic->GetNextFreeId(-1), playerId = mMultiplayer->GetId();
    if (playerId < 0)
        playerId = 0;
    while (id != -1 && id < LOWEST_RESERVED_CHARID && (id % Multiplayer::MAX_PLAYERS) != playerId)
    {
        id = dynamic->GetNextFreeId(id);
    }
    return id;
}

int Level::GetCharacterIdOwner(int id)
{
    return id % Multiplayer::MAX_PLAYERS;
}

static void DrawDynamicMap(Map *m, DynamicMap *map, RenderContext &context, const GameTime &time)
{
    core::Rectangle r(m->GetRenderRange());
    r.width--; r.height--; // we can get away with rendering a bit less of this than of the environment
    DynamicMapIterator iterator(map, r);
    while (iterator.HasNext())
    {
        Character *ch = (Character *)iterator.GetNext();
        ch->Draw(context, time);
    }
}

void Level::Draw(RenderContext &context, const GameTime &time)
{
    /*
    for (std::vector<Map *>::iterator i = maps.begin(), e = maps.end(); i != e; ++i)
        if (*i) (*i)->Draw(context, time);
    */
    maps[BASE_MAP]->Draw(context, time);
    DrawDynamicMap(maps[BASE_MAP], dynamic, context, time);
}

void Level::RunEvents(const Vector3 &position, IEventListener &listener)
{
    int index = 0;
    for (std::vector<Map *>::iterator i = maps.begin(), e = maps.end(); i != e; ++i)
    {
        ITileInstance *tile = GetTileAt(*i, position);
        if (tile)
        {
            IGameObject *r = tile->GetGameObject();
            if (r)
            {
                IEventList &elist = r->GetEventList();
                Event **events = elist.GetEvents();
                int     count  = elist.GetEventCount();
                if (count)
                    for (int i = 0; i < count; i++)
                        listener.Notify(events[i]->text.c_str());
            }
        }
        index++;
    }
}