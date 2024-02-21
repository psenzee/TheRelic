#ifndef _SIMPLETILEMAP_H
#define _SIMPLETILEMAP_H

#include "core/core.h"
#include "ITileMap.h"

class IRenderable;

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

    int            GetTileId(int x, int y)                 const;
    bool           SetTileId(int x, int y, int index);
    int            GetTileId(const core::Point &p)         const;
    bool           SetTileId(const core::Point &p, int id);

    core::Size     GetTileSize()                           const;
    core::Size     GetMapSizeInTiles()                     const;
    core::Size     GetMapSizeInPixels()                    const;
    
    void           Update(const GameTime &time, const core::Rectangle &range);
        
    IGameObject   *GetGameObject(int x, int y);
        
private:
        
    ITileSet        *mSet;
    ITileMap        *mMap;
};

#endif // _SIMPLETILEMAP_H