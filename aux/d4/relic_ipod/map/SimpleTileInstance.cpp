/*
 *  SimpleTileInstance.cpp
 *  GLGravity
 *
 *  Created by Paul Senzee on 12/17/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include "ITileMap.h"
#include "SimpleTileInstance.h"
#include "gamecore/IGameObject.h"

SimpleTileInstance::SimpleTileInstance(ITile *tile) : tile(tile), owner(false), instance(0)
{
    if (!tile)
        return;
    IGameObject *r = tile->GetGameObject();
    if (!r)
        return;
    instance = r->Copy();
    if (r != instance)
        owner = true;
}

SimpleTileInstance::~SimpleTileInstance()
{
    if (owner)
        delete instance;
    instance = 0;    
}

void SimpleTileInstance::Retain()
{
    ref++;
}

void SimpleTileInstance::Release()
{
    if (--ref == 0)
        delete this;
}

ITile *SimpleTileInstance::GetTile()
{
    return tile;
}

void SimpleTileInstance::Update(const GameTime &time)
{
    if (instance)
        instance->Update(time);
}

IGameObject *SimpleTileInstance::GetGameObject()
{
    return instance;
}
