/*
 *  SimpleTileInstance.h
 *  GLGravity
 *
 *  Created by Paul Senzee on 12/17/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _SIMPLETILEINSTANCE_H
#define _SIMPLETILEINSTANCE_H

#include "ITileMap.h"

#include <vector>

class IRenderable;

class SimpleTileInstance : public ITileInstance
{
public:    
        
    SimpleTileInstance(ITile *tile);
    ~SimpleTileInstance();
        
    ITile       *GetTile();
    void         Update(const GameTime &time);
    IGameObject *GetGameObject();
    void         Release();
    void         Retain();
        
private:
        
    int          ref;
    ITile       *tile;
    IGameObject *instance;
    bool         owner;
};

#endif // _SIMPLETILEINSTANCE_H