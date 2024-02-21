#include "SimpleTileMapModel.h"
#include "ITileMap.h"

#include "time/Timer.h"

class IRenderable;

SimpleTileMapModel::SimpleTileMapModel(ITileSet *set, ITileMap *map) : mSet(set), mMap(map)
{
}

SimpleTileMapModel::~SimpleTileMapModel()
{
    delete mMap;
    mMap = 0;
    mSet = 0;
}

ITileSet *SimpleTileMapModel::GetTileSet()
{
    return mSet;
}

ITileMap *SimpleTileMapModel::GetTileMap()
{
    return mMap;
}

ITile *SimpleTileMapModel::GetTile(int x, int y)
{
    return mSet->GetTile(mMap->GetTileId(x, y));
}

ITile *SimpleTileMapModel::GetTile(const core::Point &p)
{
    return GetTile(p.x, p.y);
}

ITile *SimpleTileMapModel::GetTile(int index)
{
    return mSet->GetTile(index);
}

int SimpleTileMapModel::GetTileId(int x, int y) const
{
    return mMap->GetTileId(x, y);
}

bool SimpleTileMapModel::SetTileId(int x, int y, int index)
{
    if (index >= mSet->GetCount())
        return false;
    if (index == mMap->GetTileId(x, y))
        return true;
    return mMap->SetTileId(x, y, index);
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
    return mSet->GetTileSize();
}

core::Size SimpleTileMapModel::GetMapSizeInTiles() const
{
    return mMap->GetSizeInTiles();
}

core::Size SimpleTileMapModel::GetMapSizeInPixels() const
{
    core::Size mapsz = GetMapSizeInTiles(), tilesz = GetTileSize();
    return core::Size(mapsz.width * tilesz.width, mapsz.height * tilesz.height);
}

void SimpleTileMapModel::Update(const GameTime &time, const core::Rectangle &range)
{
    for (int j = 0; j < range.height; j++)
    {
        for (int i = 0; i < range.width; i++)
        {
            ITile *tile = GetTile(i + range.x, j + range.y);
            if (tile) tile->Update(time);
        }
    }
}

IGameObject *SimpleTileMapModel::GetGameObject(int x, int y)
{
    ITile *tile = GetTile(x, y);
    return tile ? tile->GetGameObject() : 0;
}