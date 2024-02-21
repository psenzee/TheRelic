/*
 *  MapLoader.h
 *  GLGravity
 *
 *  Created by Paul Senzee on 12/29/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _MAPLOADER_H
#define _MAPLOADER_H

#include "IMapFactory.h"

namespace core
{
class Random;
class Size;
class Rectangle;
}

class Map;
class ITileMap;

class MapLoader : public IMapFactory
{
public:
       
    Map *Create(const MapDescription &description, ContentLoader *loader);
    
private:    

    ITileMap *CreateSequentialPattern(int startid, const core::Size &size);
    void      Clear(ITileMap *map, const core::Rectangle &area, int clear);
    void      FillSequentialPattern(ITileMap *map, int id, const core::Rectangle &area, int startid, const core::Size &patternSize);
    void      FillPattern(ITileMap *map, int id, const core::Rectangle &area, ITileMap *pattern);
};

#endif // _MAPLOADER_H