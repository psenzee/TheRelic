#ifndef _MARKEDMAP_H
#define _MARKEDMAP_H

#include "core/xna.h"
#include "core/bitmap.h"

#include "ITileMap.h"
#include "Map.h"

class MarkedMap
{
public:

    MarkedMap(const ITileMap *map);
    
    inline void Mark(const core::Point &p)                  { data.set(p.x, p.y, true); }

    inline bool IsMarked(const core::Point &p)        const { return data.get(p.x, p.y); }
    inline bool IsFloor(const core::Point &p)         const { return Map::IsFloorTile(map->GetTileId(p.x, p.y)); }
    inline bool IsMarkedFloor(const core::Point &p)   const { return IsMarked(p) && IsFloor(p); }
    inline bool IsUnmarkedFloor(const core::Point &p) const { return !IsMarked(p) && IsFloor(p); }    
    
    bool        FindClosestUnmarkedFloor(const core::Point &start, core::Point &point);

private:
    
/*
    bool FindUnmarkedFloor(const core::Rectangle &area,  core::Point &point);
    bool FindUnmarkedFloor(const core::Point     &start, core::Point &point);
    bool FindUnmarkedFloorRing(const core::Rectangle &area, core::Point &point);
*/
    const ITileMap *map;
    BitMap          data;
};

#endif  // _MARKEDMAP_H