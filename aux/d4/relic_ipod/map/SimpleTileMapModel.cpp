#include "SimpleTileMapModel.h"
#include "ITileMap.h"
#include "TileInstanceMap.h"

#include "time/Timer.h"

class IRenderable;

SimpleTileMapModel::SimpleTileMapModel(ITileSet *set, ITileMap *map) : set(set), map(map)
{
    CreateInstanceMap();
}

SimpleTileMapModel::~SimpleTileMapModel()
{
    delete map;    
//  set->Release();
    delete instances;
}

ITileSet *SimpleTileMapModel::GetTileSet()
{
    return set;
}

ITileMap *SimpleTileMapModel::GetTileMap()
{
    return map;
}

ITile *SimpleTileMapModel::GetTile(int x, int y)
{
    return set->GetTile(map->GetTileId(x, y));
}

ITile *SimpleTileMapModel::GetTile(const core::Point &p)
{
    return GetTile(p.x, p.y);
}

ITile *SimpleTileMapModel::GetTile(int index)
{
    return set->GetTile(index);
}

ITileInstance *SimpleTileMapModel::GetTileInstance(int x, int y)
{
    return instances->GetTileInstance(x, y);
}

ITileInstance *SimpleTileMapModel::GetTileInstance(const core::Point &p)
{
    return GetTileInstance(p.x, p.y);
}

int SimpleTileMapModel::GetTileId(int x, int y) const
{
    return map->GetTileId(x, y);
}

bool SimpleTileMapModel::SetTileId(int x, int y, int index)
{
    if (index >= set->GetCount())
        return false;
    if (index == map->GetTileId(x, y))
        return true;
    bool r = map->SetTileId(x, y, index);
    if (r) UpdateInstance(x, y);
    return r;
}

int SimpleTileMapModel::GetTileId(const core::Point &p) const
{
    return GetTileId(p.x, p.y);
}

bool SimpleTileMapModel::SetTileId(const core::Point &p, int id)
{
    return SetTileId(p.x, p.y, id);
}

core::Size SimpleTileMapModel::GetTileSize() const
{
    return set->GetTileSize();
}

core::Size SimpleTileMapModel::GetMapSizeInTiles() const
{
    return map->GetSizeInTiles();
}

core::Size SimpleTileMapModel::GetMapSizeInPixels() const
{
    core::Size mapsz = GetMapSizeInTiles(), tilesz = GetTileSize();
    return core::Size(mapsz.width * tilesz.width, mapsz.height * tilesz.height);
}

void SimpleTileMapModel::Update(const GameTime &time)
{
    core::Size size = GetMapSizeInTiles();
    // we probably want to window this somehow..
    for (int j = 0; j < size.height; j++)
    {
        for (int i = 0; i < size.width; i++)
        {
            ITileInstance *tile = GetTileInstance(i, j);
            if (tile != 0)
                tile->Update(time);
        }
    }
}

IGameObject *SimpleTileMapModel::GetGameObject(int x, int y)
{
    ITileInstance *tile = GetTileInstance(x, y);
    return tile ? tile->GetGameObject() : 0;
}

void SimpleTileMapModel::CreateInstanceMap()
{
    printf("Create instance map .. ");
    unsigned start = GetCurrentTimeMs();
    core::Size sz = map->GetSizeInTiles();    
    instances = new TileInstanceMap(sz);
    for (int j = 0; j < sz.height; j++)
        for (int i = 0; i < sz.width; i++)
            UpdateInstance(i, j);
    printf("took %ums\n", GetCurrentTimeMs() - start);
}

void SimpleTileMapModel::UpdateInstance(int x, int y)
{
    ITile *tile = GetTile(x, y);
    if (tile != 0)
        instances->SetTileInstance(x, y, tile->CreateTileInstance());
}
