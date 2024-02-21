/*
 *  SimpleTileMapModel.h
 *  GLGravity
 *
 *  Created by Paul Senzee on 12/17/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _SIMPLETILEMAP_H
#define _SIMPLETILEMAP_H

#include "core/core.h"
#include "ITileMap.h"

class IRenderable;
class TileInstanceMap;

class SimpleTileMapModel : public ITileMapModel
{
public:
        
    SimpleTileMapModel(ITileSet *set, ITileMap *map);
    ~SimpleTileMapModel();
        
    ITileSet      *GetTileSet();
    ITileMap      *GetTileMap();

    ITile         *GetTile(int x, int y);
    ITile         *GetTile(const core::Point &p);
    ITile         *GetTile(int index);

    ITileInstance *GetTileInstance(int x, int y);
    ITileInstance *GetTileInstance(const core::Point &p);

    int            GetTileId(int x, int y)                 const;
    bool           SetTileId(int x, int y, int index);
    int            GetTileId(const core::Point &p)         const;
    bool           SetTileId(const core::Point &p, int id);

    core::Size     GetTileSize()                           const;
    core::Size     GetMapSizeInTiles()                     const;
    core::Size     GetMapSizeInPixels()                    const;
    
    void           Update(const GameTime &time);
        
    IGameObject   *GetGameObject(int x, int y);
        
private:
        
    void CreateInstanceMap();
    void UpdateInstance(int x, int y);
        
    ITileSet        *set;
    ITileMap        *map;
    TileInstanceMap *instances;
};

#endif // _SIMPLETILEMAP_H