#include "TileMaps.h"
#include "ITileMap.h"
#include "CoreTileMap.h"

#include "core/strs.h"
#include "events/Event.h"
#include "gamecore/IGameObject.h"
#include "gamecore/IEventList.h"

ITileMap *TileMaps::CreateMap(const core::Size &size)
{
    return new CoreTileMap(size, 0);
}

void TileMaps::Copy(ITileMap *src, ITileMap *dst)
{
    core::Size ssz = src->GetSizeInTiles(),
    dsz = dst->GetSizeInTiles();
    Copy(src, core::Rectangle(0, 0, ssz.width, ssz.height),
         dst, core::Rectangle(0, 0, dsz.width, dsz.height));
}

core::Point TileMaps::GetWorldLocationAtTile(const core::Size &tileSize, const core::Point &at)
{
    return core::Point((at.x * tileSize.width)  - (tileSize.width  >> 1),
                       (at.y * tileSize.height) - (tileSize.height >> 1));
}

core::Point TileMaps::GetTileLocationAtWorld(const core::Size &tileSize, const core::Point &at)
{
    return core::Point((at.x + (tileSize.width  >> 1)) / tileSize.width, 
                       (at.y + (tileSize.height >> 1)) / tileSize.height);
}

core::Point TileMaps::GetTileLocationAtWorld(ITileMapModel *model, const core::Point &at)
{
    return GetTileLocationAtWorld(model->GetTileSize(), at);
}

int TileMaps::GetTileIdAtWorld(ITileMapModel *model, const core::Point &at)
{
    core::Point p     = GetTileLocationAtWorld(model->GetTileSize(), at);
    core::Size  mapsz = model->GetMapSizeInTiles();
    if (p.x < 0 || p.y < 0 || p.x >= mapsz.width || p.y >= mapsz.height)
        return -1;
    return model->GetTileId(p);
}

void TileMaps::SetTileIdAtWorld(ITileMapModel *model, const core::Point &at, int id)
{
    core::Point p     = GetTileLocationAtWorld(model->GetTileSize(), at);
    core::Size  mapsz = model->GetMapSizeInTiles();
    if (p.x < 0 || p.y < 0 || p.x >= mapsz.width || p.y >= mapsz.height)
        return;
    model->SetTileId(p, id);
}

ITile *TileMaps::GetTileAtWorld(ITileMapModel *model, const core::Point &at)
{
    core::Point p     = GetTileLocationAtWorld(model->GetTileSize(), at);
    core::Size  mapsz = model->GetMapSizeInTiles();
    if (p.x < 0 || p.y < 0 || p.x >= mapsz.width || p.y >= mapsz.height)
        return 0;
    return model->GetTile(p);
}

void TileMaps::Copy(ITileMap *src, const core::Rectangle &from, ITileMap *dst, const core::Rectangle &to)
{
    core::Rectangle f(from), t(to);
    if (f.x < 0) f.x = 0;
    if (f.y < 0) f.y = 0;
    if (t.x < 0) t.x = 0;
    if (t.y < 0) t.y = 0;
    core::Size ssz   = src->GetSizeInTiles(),
    dsz   = dst->GetSizeInTiles(),
    sz    = core::Size(math::min(math::min(ssz.width  - f.x, f.width),    math::min(dsz.width  - t.x, t.width)),
                       math::min(math::min(ssz.height - f.y, f.height), math::min(dsz.height - t.y, t.height)));
    CopyRaw(src, core::Point(f.x, f.y), dst, core::Point(t.x, t.y), sz);
}

ITileMap *TileMaps::Copy(ITileMap *src, const core::Rectangle &from)
{
    ITileMap *map = new CoreTileMap(core::Size(from.width, from.height));
    Copy(src, from, map, core::Rectangle(0, 0, from.width, from.height));
    return map;
}

void TileMaps::CopyRaw(ITileMap *src, const core::Point &from, ITileMap *dst, const core::Point &to, const core::Size &sz)
{
    for (int y = 0; y < sz.height; y++)
        for (int x = 0; x < sz.width; x++)
            dst->SetTileId(to.x + x, to.y + y, src->GetTileId(from.x + x, from.y + y));
}

bool TileMaps::FindClosestTile(ITileMapModel *model, const core::Point &start, core::Point &point, TilePredicateCallback f, void *context)
{
    int         distancesq = 0x7fffffff;
    bool        found      = false;
    core::Size  sz         = model->GetMapSizeInTiles();
    
    for (int j = 0; j < sz.height; j++)
    {
        for (int i = 0; i < sz.width; i++)
        {
            core::Point p(i, j);
            int dx  = start.x - i,
            dy  = start.y - j,
            dsq = dx * dx + dy * dy;                
            if (dsq < distancesq && f(model, p, context))
            {
                distancesq = dsq;
                point      = p;
                found      = true;
            }                
        }
    }
    return found;
}

core::Point TileMaps::FindEvent(ITileMapModel *map, const char *name)
{
    core::Size sz = map->GetMapSizeInTiles();
    for (int y = 0; y < sz.height; y++)
    {
        for (int x = 0; x < sz.width; x++)
        {
            ITile       *tile = map->GetTile(x, y);
            IGameObject *r    = tile->GetGameObject();
            if (r)
            {
                IEventList  &elist = r->GetEventList();
                Event     **events = elist.GetEvents();
                int         count  = elist.GetEventCount();
                if (count)
                    for (int i = 0; i < count; i++)
                        if (starts(events[i]->text.c_str(), name))
                            return core::Point(x, y);
            }
        }
    }
    return core::Point(-1, -1);
}
