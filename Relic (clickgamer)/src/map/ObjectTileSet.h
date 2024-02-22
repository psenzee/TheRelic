#ifndef _OBJECTTILESET_H
#define _OBJECTTILESET_H

#include "core/core.h"
#include "fast/Allocator.h"
#include "TileMap.h"

#include <map>
#include <string>

class XmlReadContext;
class ContentLoader;

class ObjectTileSet : public ITileSet
{
public:
    
    CLASS_NEW_DELETE()

    enum { DEFAULT_TILE_WIDTH = 128, DEFAULT_TILE_HEIGHT = 128 };

    enum Mode { WORLD_MAP, OVERHEAD_MAP };
        
    ObjectTileSet(XmlReadContext *context);
    ~ObjectTileSet();
        
    int         GetCount()         const;
    core::Size  GetTileSize()      const;
    ITile      *GetTile(int index);
    ITile      *GetDefaultTile()   { return tiles[mMode == WORLD_MAP ? defaultWorldTile : defaultOverheadMapTile]; }
    void        SetMode(Mode mode) { mMode = mode; }

private:

    core::Size            size;
    std::vector<ITile *>  tiles;
    XmlReadContext       *context;
    int                   defaultOverheadMapTile;
    int                   defaultWorldTile;
    Mode                  mMode;

    static std::vector<IGameObject *> CreateGameObjects(XmlReadContext *context);
};

class ObjectTileSetLoader
{
public:
               
    static ITileSet *GetTileSet(const char *name, ContentLoader *loader);
    static void      RemoveTileSet(const char *name);
	static void      RemoveAllTileSets();
        
private:
        
    static std::map<std::string, ITileSet *> cache;
};

#endif // _OBJECTTILESET_H