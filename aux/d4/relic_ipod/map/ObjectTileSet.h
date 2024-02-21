#ifndef _OBJECTTILESET_H
#define _OBJECTTILESET_H

#include "core/core.h"
#include "TileMap.h"

#include <map>
#include <string>

class XmlReadContext;
class ContentLoader;

class ObjectTileSet : public ITileSet
{
public:

    enum { DEFAULT_TILE_WIDTH = 128, DEFAULT_TILE_HEIGHT = 128 };
        
    ObjectTileSet(XmlReadContext *context);
    ~ObjectTileSet();
        
    int         GetCount()         const;
    core::Size  GetTileSize()      const;
    ITile      *GetTile(int index);

private:

    core::Size            size;
    std::vector<ITile *>  tiles;
    XmlReadContext       *context;

    static std::vector<IGameObject *> CreateGameObjects(XmlReadContext *context);
};

class ObjectTileSetLoader
{
public:
               
    static ITileSet *GetTileSet(const char *name, ContentLoader *loader);
        
private:
        
    static std::map<std::string, ITileSet *> cache;
};

#endif // _OBJECTTILESET_H