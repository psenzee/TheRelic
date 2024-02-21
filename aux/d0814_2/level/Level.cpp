#include "Level.h"

#include "core/core.h"
#include "core/random.h"
#include "core/global.h"
#include "core/bitmap2d.h"

#include "map/Map.h"
#include "map/MapLoader.h"
#include "map/MapDescription.h"
#include "map/TileMap.h"
#include "map/ITileMap.h"
#include "map/DynamicMap.h"
#include "map/VectorFieldMap.h"
#include "map/DebrisMap.h"
#include "map/ObjectTileSet.h"

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

void GetNamed(std::vector<Map *> &map, const std::vector<MapDescription> &descriptions, const char *name, std::vector<core::Point> &items)
{
    ITileMapModel *model = map[0]->GetMapModel();
    core::Size mapSizeInTiles(model->GetMapSizeInTiles());
    for (int j = 0; j < mapSizeInTiles.height; j++)
    {
        for (int i = 0; i < mapSizeInTiles.width; i++)
        {
            core::Point p(i, j);
            std::vector<MapDescription>::const_iterator di = descriptions.begin();
            for (std::vector<Map *>::iterator i = map.begin(), e = map.end(); i != e; ++i)
            {
                if ((*di).events)
                {
                    ITileMapModel *model = (*i)->GetMapModel();
                    if (model->GetTile(p)->HasEvent(name))
                    {
                        items.push_back(p);
                        break;
                    }
                }
                ++di;
            }
        }
    }
}

Character *Level::GetCharacter(int i)
{ 
    return static_cast<Character *>(mDynamic->GetMovableById(i));
}

const std::vector<core::Point> &Level::GetItems(const char *name)
{
    Items &items = mNamedItems[name];
    if (!items.searched && items.items.empty())
    {
        GetNamed(mMaps, mOptions->maps, name, items.items);
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

static ITile *GetTileAt(Map *map, const Vector3 &position)
{
    if (!map) return 0;
    return TileMaps::GetTileAtWorld(map->GetMapModel(), GetPositionPoint(position, map->GetPosition()));
}

ITile *Level::GetTileAt(Map *map, const Vector3 &position)
{
    return ::GetTileAt(map, position);
}

int Level::GetTileIdAt(Map *map, const Vector3 &position)
{
    return map->GetTileIdAt(v2(position));
}

core::Point Level::GetTileLocationAt(Map *map, const Vector3 &position)
{
    return TileMaps::GetTileLocationAtWorld(map->GetMapModel(), GetPositionPoint(position, map->GetPosition()));
}

static int GetTileEventCountAt(std::vector<Map *> &maps, const std::vector<MapDescription> &descriptions, const Vector3 &position)
{
    int count = 0;
    std::vector<MapDescription>::const_iterator di = descriptions.begin();
    for (std::vector<Map *>::iterator i = maps.begin(), e = maps.end(); i != e; ++i)
    {
        if ((*di).events)
        {
            ITile *tile = GetTileAt(*i, position);
            IGameObject *object = tile->GetGameObject();
            IEventList &event = object->GetEventList();
            count += event.GetEventCount();
        }
        ++di;
    }
    return count;
}

static const char *GetTileEventAt(std::vector<Map *> &maps, const std::vector<MapDescription> &descriptions, const Vector3 &position, int index)
{
    int count = 0;
    std::vector<MapDescription>::const_iterator di = descriptions.begin();
    for (std::vector<Map *>::iterator i = maps.begin(), e = maps.end(); i != e; ++i)
    {
        if ((*di).events)
        {
            ITile *tile = GetTileAt(*i, position);
            IGameObject *object = tile->GetGameObject();
            IEventList &event = object->GetEventList();
            count += event.GetEventCount();
            if (index < count)
            {
                Event **list = event.GetEvents();
                return list[index - count]->text.c_str();
            }
        }
        ++di;
    }
    return "";
}

static const char *HasTileEventAt(std::vector<Map *> &maps, const std::vector<MapDescription> &descriptions, const char *name, const Vector3 &position)
{
    int count = 0;
    std::vector<MapDescription>::const_iterator di = descriptions.begin();
    for (std::vector<Map *>::iterator i = maps.begin(), e = maps.end(); i != e; ++i)
    {
        if ((*di).events)
        {
            ITile *tile = GetTileAt(*i, position);
            if (tile)
            {
                IGameObject *object = tile->GetGameObject();
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
        }
        ++di;
    }
    return 0;
}

int Level::GetTileEventCountAt(const Vector3 &position)
{
    return ::GetTileEventCountAt(mMaps, mOptions->maps, position);
}

const char *Level::GetTileEventAt(const Vector3 &position, int index)
{
    return ::GetTileEventAt(mMaps, mOptions->maps, position, index);
} 

const char *Level::HasTileEventAt(const char *name, const Vector3 &position)
{
    return ::HasTileEventAt(mMaps, mOptions->maps, name, position);
}

Vector3 Level::GetFieldVector(const Vector3 &at) const
{
    VectorFieldItem item = mVectorField->GetItemAt(v2(at));
    return v3(item.vector * item.weight, 0.f);
}

static const float COLLIDE_WEIGHT = 10.f;

static float GetCollisionWeight(const Vector2 &at, void *user)
{
    Level   *level = reinterpret_cast<Level *>(user);
    Map     *map   = level->GetMap();
    Vector3  resolve;
    ICollidable::Classification classification = map->Collision(v3(at, 10.f), 32.f, resolve);    
    switch (classification)
    {
    case ICollidable::CLASS_IN:  return COLLIDE_WEIGHT;
    case ICollidable::CLASS_ON:  return COLLIDE_WEIGHT * 0.5f;
    case ICollidable::CLASS_OUT: return 0.f;
    }
    return COLLIDE_WEIGHT;
}

const char *Level::GetPlaceName(const Vector3 &player) const
{
    if (mMaps[LABEL_MAP])
    {
        int value = mMaps[LABEL_MAP]->GetTileIdAt(v2(player));
        switch (value)
        {
        case 32896: return "Pandora's Gape";
        case 32897: return "Cave of the Amaranth Ring";
        case  8446: return "The Quinotaur's Chamber";
        case 31451: return "Amaranth Systems";
        case 31450: return "The Relic";
        default: break;
        }
        core::Random r(value);
static int last = 0;
if (last != value)
{
  printf("GetPlaceName value is %d\n", value);
  last = value;
}
        return mPlaceNameGenerator.GetPlaceName(r);
    }
    return mPlaceNameGenerator.GetPlaceNameAt(32 /*$TODO TEMPORARY */, static_cast<int>(fabsf(player.x / 1024.f)), 
                                                                       static_cast<int>(fabsf(player.y / 1024.f)));
}

static int _tmpGetDownStairsFloorType(int floorType)
{
    switch (floorType)
    {
    case 160: case 169: return 165;
    case 170: case 179: return 175;
    case 180: case 189: return 185;
    case 190: case 199: return 195;
    }
    return floorType;
}

static void _tmpResolveDownStairs(std::vector<Map *> &maps)
{
    core::Size sz = maps[0]->GetMapModel()->GetMapSizeInTiles();
    for (int j = 0; j < sz.height; j++)
        for (int i = 0; i < sz.width; i++)
            if (maps[0]->GetMapModel()->GetTileId(i, j) == 17)
                maps[1]->GetMapModel()->SetTileId(i, j, _tmpGetDownStairsFloorType(maps[1]->GetMapModel()->GetTileId(i, j)));
}

Level::Level(ContentLoader *loader, const char *path, int level, Multiplayer *multiplayer) : mDynamic(0), mLevelNumber(level), mMultiplayer(multiplayer), mSync(0), mVectorField(0), mVisited(0), mVisitedMap(0)
{
    mOptions = new LevelOptions(path);
    MapLoader mapLoader;
    for (std::vector<MapDescription>::const_iterator i = mOptions->maps.begin(), e = mOptions->maps.end(); i != e; ++i)
        mMaps.push_back(mapLoader.Create(*i, loader));
    // create dynamic map
    if (mMaps.size() <= BASE_MAP)
    {
        printf("ERROR: No Maps To Load! Level is uninitialized!\n");
        return;
    }
    if (mMaps.size() > 2)
        _tmpResolveDownStairs(mMaps);
    Map *mainMap = mMaps[BASE_MAP];
    PositionVelocity::SetCollider(MapCollider, mainMap);    
    core::Size mapSize(mainMap->GetMapModel()->GetMapSizeInPixels()),
               tileSize(mainMap->GetMapModel()->GetTileSize());
    Vector2 mapDimensions(static_cast<float>(mapSize.width),  static_cast<float>(mapSize.height));
    mDynamic = new DynamicMap(mapDimensions,
                             Vector2(static_cast<float>(tileSize.width), static_cast<float>(tileSize.height)));
    if (mMultiplayer)
        mSync = new CharacterSync(mMultiplayer, mDynamic);
    // $TODO these hardcoded values should be configurable
    VectorFieldMap field(0.f, COLLIDE_WEIGHT, 4, 5);
    field.SetDimensions(Vector2(0.f, 0.f) /* we always start at 0, 0 in this game */,
                                mapDimensions,
                                static_cast<int>(tileSize.width / 8.f));
    int time = GetCurrentTimeMs();
    field.Create(GetCollisionWeight, this);
    printf("VectorField::Create %dms\n", GetCurrentTimeMs() - time);
    mVectorField = new CompactVectorFieldMap(field);
    core::Size sz = mainMap->GetMapModel()->GetMapSizeInTiles();
    mVisited = new BitMap2d(sz.width, sz.height, true); // defaults to true (visited) so that we don't continually (or ever) spawn beyond the edges
    mVisitedMap = new BitMap2d(sz.width, sz.height, false); // defaults to false for the displayed visited map
    mDebrisMap = new DebrisMap(sz, tileSize);
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
    core::Size tileSize(mMaps[BASE_MAP]->GetMapModel()->GetTileSize());
    core::Point p = items[index % items.size()];
    return Vector3(static_cast<float>(p.x * tileSize.width), static_cast<float>(p.y * tileSize.height), 0.f);
}

void Level::AddCharacter(Character *ch)
{
    if (ch && Character::IsCharacterValid(ch))
    {
        ch->SetMap(mMaps[BASE_MAP]);
        mDynamic->GetRegistry().Register(ch->GetId(), ch);
        ch->SetDynamicMap(mDynamic);
        ch->SetAddFrameToDebrisMapCallback(Level::AddFrameToDebrisMap, this);
    }
}

void Level::RemoveCharacter(Character *ch)
{
    if (ch)
    {
        ch->SetDynamicMap(0);
        ch->SetAddFrameToDebrisMapCallback(0, 0);
        mDynamic->GetRegistry().Unregister(ch);
    }
}

char *Level::GetVisitedData(int *size)
{
    int size1    = mVisited->map.width * mVisited->map.height,
        size2    = mVisitedMap->map.width * mVisitedMap->map.height;
    *size = size1 + size2 + sizeof(int) + sizeof(int);
    unsigned char *data = new unsigned char [*size];
    unsigned char *p = data;
    memcpy(p, &size1, sizeof(int)); p += sizeof(int);
    memcpy(p, &size2, sizeof(int)); p += sizeof(int);
    memcpy(p, mVisited->map.data,    size1); p += size1;
    memcpy(p, mVisitedMap->map.data, size2); p += size2;
    return (char *)data;
}

bool Level::SetVisitedData(const char *data, int size)
{
    int size1    = 0,
        size2    = 0;
    const unsigned char *p = (const unsigned char *)data;
    memcpy(&size1, p, sizeof(int)); p += sizeof(int);
    memcpy(&size2, p, sizeof(int)); p += sizeof(int);
    int prevsize1 = mVisited->map.width * mVisited->map.height,
        prevsize2 = mVisitedMap->map.width * mVisitedMap->map.height;
    if (prevsize1 == size1 && prevsize2 == size2)
    {
        memcpy(mVisited->map.data,    p, size1); p += size1;
        memcpy(mVisitedMap->map.data, p, size2); p += size2;
        return true;
    }
    return false;
}

Level::~Level()
{
    CharacterSync *sync = mSync;	
    mSync = 0;
    if (sync)
        delete sync;	
    for (std::vector<Map *>::iterator i = mMaps.begin(), e = mMaps.end(); i != e; ++i)
        delete *i;
    mMaps.clear();
    // $TODO iterate through the characters in DynamicMap and remove them
    delete mDynamic;
    mDynamic = 0;
    delete mOptions;
    mOptions = 0;
    delete mVectorField;
    mVectorField = 0;
    delete mVisited;
    mVisited = 0;
    delete mVisitedMap;
    mVisitedMap = 0;
    delete mDebrisMap;
    mDebrisMap = 0;
}

core::Size Level::GetSize()
{ 
    return mMaps[BASE_MAP]->GetMapModel()->GetMapSizeInPixels();
}

static void UpdateDynamicMap(Map *m, DynamicMap *map, const GameTime &time)
{
    DynamicMapIterator iterator(map, m->GetRenderRange());
    while (iterator.HasNext())
    {
        Character *ch = (Character *)iterator.GetNext();
        if (Character::IsCharacterValid(ch))
            ch->Update(time);
    }
}

static void SetPerimeter(BitMap2d *map, const core::Rectangle &range, int margin, bool value)
{
    for (int i = -margin; i < range.height + margin * 2; i++)
    {
        map->set(range.x - margin,                   i + range.y, value);
        map->set(range.x + range.width + margin - 1, i + range.y, value);
    }
    for (int i = -margin + 1; i < range.width + margin * 2 - 1; i++)
    {
        map->set(i + range.x, range.y - margin, value);
        map->set(i + range.x, range.y + range.height + margin - 1, value);
    }
}
        
void Level::Update(RenderContext &context, const GameTime &time, const PauseState &pause, IEventListener &listener)
{
    enum { CONSTRAINED_COUNT_ENEMY_TYPE = 1111, MAX_ENEMY_COUNT = 25 };

    if (mSync)
        mSync->Update();
    std::vector<MapDescription>::const_iterator di = mOptions->maps.begin();
    for (std::vector<Map *>::iterator i = mMaps.begin(), e = mMaps.end(); i != e; ++i)
    {
        if (*i && (*di).events) (*i)->Update(context, time);
        ++di;
    }
    static_cast<ObjectTileSet *>(mMaps[BASE_MAP]->GetMapModel()->GetTileSet())->SetMode(pause.characters ? ObjectTileSet::OVERHEAD_MAP : ObjectTileSet::WORLD_MAP);
    if (!pause.update && !pause.characters)
    {
        UpdateDynamicMap(mMaps[BASE_MAP], mDynamic, time);
        core::Rectangle range = mMaps[BASE_MAP]->GetRenderRange();
        if (mDebrisMap)
            mDebrisMap->Update(time, range);
        if (mSync)
            mSync->SendCharacterUpdates(mMaps[BASE_MAP]);
        int offset = 0;
        for (int j = -offset; j < range.height + offset * 2; j++)
        {
            for (int i = -offset; i < range.width + offset * 2; i++)
            {
                core::Point p(i + range.x, j + range.y);
                if (!mVisited->get(p.x, p.y))
                {
                    if (!mVisitedMap->get(p.x, p.y))
                        FirstVisit(p, listener, "OnTrueFirstVisit");					
                    FirstVisit(p, listener, "OnFirstVisit");
                    mVisited->set(p.x, p.y, true);
                }
            }
        }
        int margin = 4, outermargin = margin + 4;
        int enemyCount = mDynamic->GetCountOfType(CONSTRAINED_COUNT_ENEMY_TYPE, 
                          core::Rectangle(range.x - outermargin, range.y - outermargin,
                                          range.width + outermargin * 2, range.height + outermargin * 2));
        // set this back to zero out of a certain range (only if number of baddies is below a certain threshold)
        if (enemyCount < MAX_ENEMY_COUNT)
            SetPerimeter(mVisited, range, margin, false);
        offset = 0;		
        for (int j = offset; j < range.height - offset * 2; j++)
        {
            for (int i = offset; i < range.width - offset * 2; i++)
            {
                core::Point p(i + range.x, j + range.y);
                mVisitedMap->set(p.x, p.y, true);
            }
        }
    }
}

void Level::FirstVisit(const core::Point &p, IEventListener &listener, const char *eventName)
{
    // fire an event here if the tile is marked
    Vector3 position(v3(mMaps[BASE_MAP]->GetWorldLocationAtTile(p)));
    size_t eventNameSz = strlen(eventName);
    const char *event = HasTileEventAt(eventName, position);
    if (event && strncmp(event, eventName, eventNameSz) == 0)
    {
        // fire an event with the data contained in *event and a string version of the tile location
        char data[1024];
        sprintf(data, "%s %d %d", event + eventNameSz + 1, p.x, p.y);
        listener.Notify(eventName, data);
    }
}

bool Level::UncollideVisible()
{
    return mDynamic->Uncollide(mMaps[BASE_MAP]->GetRenderRange());
}

int Level::GetFreeCharacterId() const
{ 
    enum { LOWEST_RESERVED_CHARID = 32 };
    int id = mDynamic->GetNextFreeId(-1), playerId = mSync->GetPlayerId();
    if (playerId < 0)
        playerId = 0;
    while (id != -1 && id < LOWEST_RESERVED_CHARID && (id % Multiplayer::MAX_PLAYERS) != playerId)
    {
        id = mDynamic->GetNextFreeId(id);
    }
    return id;
}

int Level::GetCharacterIdOwner(int id)
{
    return id % Multiplayer::MAX_PLAYERS;
}

static void DrawDynamicMap(Map *m, DynamicMap *map, RenderContext &context, const GameTime &time, bool staticOnly)
{
    core::Rectangle r(m->GetRenderRange());
    DynamicMapIterator iterator(map, r);
    while (iterator.HasNext())
    {
        Character *ch = (Character *)iterator.GetNext();
        if (!staticOnly || ch->IsStatic())
            ch->Draw(context, time);
    }
}

void Level::Draw(RenderContext &context, const GameTime &time, const PauseState &pause)
{
    if (!pause.render)
    {
        for (int i = 0; i < mOptions->maps.size(); i++)
            if (i < mMaps.size() && mOptions->maps[i].render)
                mMaps[i]->Draw(context, time, i == BASE_MAP, pause.characters ? mVisitedMap : 0);
        DrawDynamicMap(mMaps[BASE_MAP], mDynamic, context, time, pause.characters);
        if (!pause.characters)
        {
           if (mDebrisMap)
               mDebrisMap->Render(context, mMaps[BASE_MAP]->GetRenderRange());
        }
    }
}

void Level::RunEvents(const Vector3 &position, IEventListener &listener)
{
    int index = 0;
    std::vector<MapDescription>::const_iterator di = mOptions->maps.begin();
    for (std::vector<Map *>::iterator i = mMaps.begin(), e = mMaps.end(); i != e; ++i)
    {
        ITile *tile = GetTileAt(*i, position);
        if (tile && (*di).events)
        {
            IGameObject *r = tile->GetGameObject();
            if (r)
            {
                IEventList &elist = r->GetEventList();
                Event **events = elist.GetEvents();
                int     count  = elist.GetEventCount();
                if (count)
                {
                    std::string identifier;
                    for (int i = 0; i < count; i++)
                    {
                        if (!events[i]->text.empty())
                        {
                            const char *pe = events[i]->text.c_str();
                            identifier = read_identifier(&pe);
                            skip_space(&pe);
                            listener.Notify(identifier.c_str(), pe);
                        }
                    }
                }
            }
        }
        index++;
        ++di;
    }
}

void Level::DestroyCharacters(std::vector<Character *> &persistent)
{
    DynamicMapIterator i = mDynamic->GetAll();
    std::vector<Character *> destroy;
    while (i.HasNext())
    {
        Character *character = static_cast<Character *>(i.GetNext());
        if (character)
        {
            if (character->IsPersistent())
                persistent.push_back(character);
            else
                destroy.push_back(character);
        }
    }
    for (std::vector<Character *>::iterator i = destroy.begin(), e = destroy.end(); i != e; ++i)
    {
        RemoveCharacter(*i);
        delete *i;
    }
    for (std::vector<Character *>::iterator i = persistent.begin(), e = persistent.end(); i != e; ++i)
        RemoveCharacter(*i);
    destroy.clear();
}

bool Level::AddFrameToDebrisMap(void *leveluser, IGameObject *frame, const Vector3 &position, float angle, float scale)
{
    Level *level = static_cast<Level *>(leveluser);
    if (!level || !level->mDebrisMap)
        return false;
    level->mDebrisMap->Add(frame, position, angle, scale);
    return true;
}