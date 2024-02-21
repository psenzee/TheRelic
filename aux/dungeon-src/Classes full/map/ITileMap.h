/*
 *  ITileMap_.h
 *  GLGravity
 *
 *  Created by Paul Senzee on 12/17/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

/*
 */

#ifndef _ITILEMAP_H
#define _ITILEMAP_H

#include "core/xna.h"

class IRenderable;
class ITileInstance;

class ITile
    {
    public:    
        
        virtual ~ITile() {}
        
        virtual IRenderable   *GetRenderableObject()              = 0;
        virtual ITileInstance *CreateTileInstance()               = 0;
        virtual void           Update(const GameTime &time)       = 0;
        virtual int            GetId()                      const = 0;
    };

class ITileInstance
    {
    public:
        
        virtual ~ITileInstance() {}
        
        virtual IRenderable   *GetRenderableObject()        = 0; 
        virtual void           Update(const GameTime &time) = 0;
        virtual ITile         *GetTile()                    = 0;
    };

class ITileSet
    {
    public:
        
        virtual ~ITileSet() {}    
        
        virtual int    GetCount()          const = 0;
        virtual core::Size   GetTileSize()       const = 0;
        virtual ITile *GetTile(int index)        = 0;
    };

class ITileMap
    {
    public:
        
        virtual ~ITileMap() {}
        
        virtual int  GetTileId(int x, int y)         const = 0;
        virtual bool SetTileId(int x, int y, int id)       = 0;
        virtual core::Size GetSizeInTiles()                const = 0;
    };

class ITileMapModel
    {
    public:
        
        virtual ~ITileMapModel() {}    
        
        virtual ITileSet      *GetTileSet()                               = 0;
        virtual ITileMap      *GetTileMap()                               = 0;
        
        virtual ITileInstance *GetTileInstance(int x, int y)              = 0;
        virtual ITileInstance *GetTileInstance(const core::Point &p)            = 0;
        
        virtual int            GetTileId(int x, int y)              const = 0;
        virtual int            GetTileId(const core::Point &p)            const = 0;
        virtual bool           SetTileId(int x, int y, int index)         = 0;
        virtual bool           SetTileId(const core::Point &p, int id)                 = 0;
        
        virtual core::Size     GetTileSize()                        const = 0;
        virtual core::Size     GetMapSizeInTiles()                  const = 0;
        virtual core::Size     GetMapSizeInPixels()                 const = 0;
        
        virtual void           Update(const GameTime &time)               = 0;
        
        virtual IRenderable   *GetRenderableObject(int x, int y)          = 0;
    };

#endif // _ITILEMAP_H