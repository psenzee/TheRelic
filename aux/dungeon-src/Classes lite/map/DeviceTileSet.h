/*
 *  DeviceTileSet_.h
 *  GLGravity
 *
 *  Created by Paul Senzee on 12/17/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _DEVICETILESET_H
#define _DEVICETILESET_H

#include "core/xna.h"

#include "TileMap.h"

#include <map>
#include <string>

class XmlReadContext;

class DeviceTileSet : public RenderableTileSet
{
public:
        
    XmlReadContext *context;
        
    DeviceTileSet(XmlReadContext *context, const core::Size &size);
        
private:
        
    static std::vector<Renderable *> CreateGameObjects(XmlReadContext *context);
};

class DeviceTileSetLoader
{
public:
        
    static ITileSet *GetTileSet(const char *name, const char *common_name, LocalGameServices *services);
        
private:
        
    static std::map<std::string, ITileSet *> cache;
};

#endif // _DEVICETILESET_H