/*
 *  TileMap_.h
 *  GLGravity
 *
 *  Created by Paul Senzee on 12/17/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _TILEMAP_H
#define _TILEMAP_H

#include "core/xna.h"
#include "core/aabox.h"

#include "ITileMap.h"

#include <vector>

class IRenderable;
class Renderable;
class GraphicsDevice;
class OverheadCamera;

namespace core
{
class Rectangle;
};

class SimpleTileInstance : public ITileInstance
{
public:    
        
    SimpleTileInstance(ITile *tile);
    ~SimpleTileInstance();
        
    ITile       *GetTile();
    void         Update(const GameTime &time);
    IRenderable *GetRenderableObject();
        
private:
        
    ITile       *tile;
    IRenderable *instance;
    bool         owner;
};

class RenderableTile : public ITile
{
public:
        
    RenderableTile(Renderable *renderable, int id);
    ~RenderableTile();
        
    IRenderable   *GetRenderableObject();
    int            GetId() const;
    ITileInstance *CreateTileInstance();
    void           Update(const GameTime &time);
        
private:
        
    Renderable *renderable;
    int         id;
};

class RenderableTileSet : public ITileSet
{
public:
        
    RenderableTileSet(std::vector<Renderable *> objects, const core::Size &size);
    ~RenderableTileSet();
        
    int         GetCount()         const;
    core::Size  GetTileSize()      const;
    ITile      *GetTile(int index);
        
private:
        
    core::Size           size;
    std::vector<ITile *> tiles;
};

class TileMaps
{
public:

    typedef bool (*TilePredicateCallback)(ITileMapModel *model, const core::Point &point, void *context);    
    
    static ITileMap      *CreateMap(const core::Size &size);
    static void           Copy(ITileMap *src, ITileMap *dst);
    static ITileMap      *ReadMap(const char *filename);
    static core::Point    GetTileLocationAtWorld(const core::Size &tileSize, const core::Point &at);
    static core::Point    GetTileLocationAtWorld(ITileMapModel *model, const core::Point &at);
    static int            GetTileIdAtWorld(ITileMapModel *model, const core::Point &at);
    static ITileInstance *GetTileInstanceAtWorld(ITileMapModel *model, const core::Point &at);
    static void           Copy(ITileMap *src, const core::Rectangle &from, ITileMap *dst, const core::Rectangle &to);
    static ITileMap      *Copy(ITileMap *src, const core::Rectangle &from);
    static core::Point    FindEvent(ITileMapModel *map, const char *name);
    static bool           FindClosestTile(ITileMapModel *model, const core::Point &start, core::Point &point, TilePredicateCallback f, void *context);
        
private:
        
    static void           CopyRaw(ITileMap *src, const core::Point &from, ITileMap *dst, const core::Point &to, const core::Size &sz);
};

class CoreTileMap : public ITileMap
{
public:
        
    CoreTileMap(const core::Size &size);
    CoreTileMap(const core::Size &size, int initialValue);
    ~CoreTileMap();
        
    int        GetTileId(int x, int y)         const;
    bool       SetTileId(int x, int y, int id);
    core::Size GetSizeInTiles()                const;
    
    void       Reverse();
        
private:
        
    core::Size  size;
    int        *data;
        
    void Read(const char *filename);    
    void Create(const core::Size &size, int value);
};

class TileInstanceMap
{
public:
    
    TileInstanceMap(const core::Size &size);
    ~TileInstanceMap();
        
    ITileInstance *GetTileInstance(int x, int y);
    bool           SetTileInstance(int x, int y, ITileInstance *instance);
    core::Size     GetSizeInTiles() const;
    
private:
        
    core::Size       size;
    ITileInstance  **data;
        
    void Create(const core::Size &size);
};

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
    int            GetTileId(int x, int y)             const;
    bool           SetTileId(int x, int y, int index);
    int            GetTileId(const core::Point &p)           const;
    bool           SetTileId(const core::Point &p, int id);
    core::Size     GetTileSize()                       const;
    core::Size     GetMapSizeInTiles()                 const;
    core::Size     GetMapSizeInPixels()                const;
    
    void           Update(const GameTime &time);
        
    IRenderable   *GetRenderableObject(int x, int y);
        
private:
        
    void CreateInstanceMap();
    void UpdateInstance(int x, int y);
        
    ITileSet        *set;
    ITileMap        *map;
    TileInstanceMap *instances;
};

class LocalGameServices;

class TileMapRenderer
{
public:
    
    static void  Render(GraphicsDevice &device, ITileMapModel *model, const Vector3 &at, const OverheadCamera &camera);
    static void  Render(GraphicsDevice &device, ITileMapModel *model, const Vector3 &at, const AABox &bounds, const OverheadCamera &camera);
    
    static AABox GetVisibleMapRegion(ITileMapModel *model, const Vector3 &at, const OverheadCamera &camera);
    static AABox GetVisibleMapRegion(ITileMapModel *model, const AABox &bounds, const OverheadCamera &camera);
    
    // not to be used every frame!
    static AABox GetMapBoundsSlow(ITileMapModel *model, const Vector3 &at, const core::Rectangle &area);
};

#endif // _TILEMAP_H