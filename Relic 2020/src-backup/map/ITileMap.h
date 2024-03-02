#ifndef _ITILEMAP_H
#define _ITILEMAP_H

#include "core/core.h"

class IGameObject;

class ITile
{
public:    
    
    virtual ~ITile() {}
    
    virtual IGameObject   *GetGameObject()                    = 0;
    virtual void           Update(const GameTime &time)       = 0;
    virtual int32_t        GetId()                      const = 0;
    virtual bool           HasEvent(const char *name)   const = 0;
};

class ITileSet
{
public:
    
    virtual ~ITileSet() {}    
    
    virtual int32_t      GetCount()             const = 0;
    virtual core::Size   GetTileSize()          const = 0;
    virtual ITile       *GetTile(int32_t index)       = 0;
    virtual ITile       *GetDefaultTile()             = 0;
};

class ITileMap
{
public:
    
    virtual ~ITileMap() {}
    
    virtual int32_t    GetTileId(int32_t x, int32_t y)             const = 0;
    virtual bool       SetTileId(int32_t x, int32_t y, int32_t id)       = 0;
    virtual core::Size GetSizeInTiles()                            const = 0;
};

class ITileMapModel
{
public:
    
    virtual ~ITileMapModel() {}    
    
    virtual ITileSet      *GetTileSet()                                         = 0;
    virtual ITileMap      *GetTileMap()                                         = 0;
    
    virtual ITile         *GetTile(int32_t x, int32_t y)                        = 0;
    virtual ITile         *GetTile(const core::Point &p)                        = 0;
    virtual ITile         *GetTile(int32_t index)                               = 0;
    
    virtual int32_t        GetTileId(int32_t x, int32_t y)                const = 0;
    virtual int32_t        GetTileId(const core::Point &p)                const = 0;
    virtual bool           SetTileId(int32_t x, int32_t y, int32_t index)       = 0;
    virtual bool           SetTileId(const core::Point &p, int32_t id)          = 0;
    
    virtual core::Size     GetTileSize()                                  const = 0;
    virtual core::Size     GetMapSizeInTiles()                            const = 0;
    virtual core::Size     GetMapSizeInPixels()                           const = 0;
    
    virtual void           Update(const GameTime &time, const core::Rectangle &range) = 0;
    
    virtual IGameObject   *GetGameObject(int32_t x, int32_t y)                  = 0;
};

#endif // _ITILEMAP_H
