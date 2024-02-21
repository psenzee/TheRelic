/*
 *  TileInstanceMap.h
 *  GLGravity
 *
 *  Created by Paul Senzee on 12/17/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _TILEINSTANCEMAP_H
#define _TILEINSTANCEMAP_H

#include "core/core.h"
#include "ITileMap.h"

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

#endif // _TILEINSTANCEMAP_H