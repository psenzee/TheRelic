#ifndef _TILEMAPS_H
#define _TILEMAPS_H

#include "core/core.h"
#include "ITileMap.h"

class TileMaps
{
public:

    typedef bool (*TilePredicateCallback)(ITileMapModel *model, const core::Point &point, void *context);
    
    static ITileMap      *CreateMap(const core::Size &size);
    static void           Copy(ITileMap *src, ITileMap *dst);
    static ITileMap      *ReadMap(const char *filename);
    static core::Point    GetWorldLocationAtTile(const core::Size &tileSize, const core::Point &at);
    static core::Point    GetTileLocationAtWorld(const core::Size &tileSize, const core::Point &at);
    static core::Point    GetTileLocationAtWorld(ITileMapModel *model, const core::Point &at);
    static ITile         *GetTileAtWorld(ITileMapModel *model, const core::Point &at);
    static int32_t        GetTileIdAtWorld(ITileMapModel *model, const core::Point &at);
    static void           SetTileIdAtWorld(ITileMapModel *model, const core::Point &at, int32_t id);
    static void           Copy(ITileMap *src, const core::Rectangle &from, ITileMap *dst, const core::Rectangle &to);
    static ITileMap      *Copy(ITileMap *src, const core::Rectangle &from);
    static core::Point    FindEvent(ITileMapModel *map, const char *name);
    static bool           FindClosestTile(ITileMapModel *model, const core::Point &start, core::Point &point, TilePredicateCallback f, void *context);
        
private:
        
    static void           CopyRaw(ITileMap *src, const core::Point &from, ITileMap *dst, const core::Point &to, const core::Size &sz);
};

#endif // _TILEMAPS_H
