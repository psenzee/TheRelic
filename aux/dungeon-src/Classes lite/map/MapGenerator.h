/*
 *  MapGenerator.h
 *  GLGravity
 *
 *  Created by Paul Senzee on 12/29/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _MAPGENERATOR_H
#define _MAPGENERATOR_H

#include "core/xna.h"

namespace core
{
class Random;    
}

class Map;
class ITileMap;
class LocalGameServices;
class XmlElement;

class MapGenerator
{
public:
        
    LocalGameServices *Services;
        
    enum GeneratorConstants
    {
        RANDOM_PICKUP = 234, // the higher these numbers, the rarer the occurence
        //RANDOM_TREE   = 23
        RANDOM_TREE   = 5
    };
        
    MapGenerator(LocalGameServices *services);
        
    Map          *GenerateMap(int level, const core::Size &mapsz, XmlElement *levelConfig, const char *mapTypeName, int seed, int spiralL50vel);
    
private:    
    
    static float  StartDistance(const core::Point &start, const core::Point &at);

    void          Clear(ITileMap *map, const core::Rectangle &area, int clear);
    
    void          SetCheckeredFloor(ITileMap *map);    
    void          SetCheckeredFloor(ITileMap *map, const core::Rectangle &area);    
    void          SetPatternedFloor(ITileMap *map, const core::Rectangle &area, ITileMap *pattern);
    
    void          SetTrees(ITileMap *map, core::Random &r, const core::Point &start);
    void          SetTrees(ITileMap *map, const core::Rectangle &area, core::Random &r, const core::Point &start);

    void          SetRandomPickups(ITileMap *map, core::Random &r);    
    core::Point   GetSpiral8MapStart(ITileMap *map, int level);
    void          SetSpecialTilesForSpiral8Map(ITileMap *map, int level);

    void          MakeFence(ITileMap *map, const core::Rectangle &area);
    void          MakeParkType1(ITileMap *map, const core::Rectangle &area, core::Random &r, const core::Point &start, bool treesOutside = true, ITileMap *pattern = 0);
};

#endif // _MAPGENERATOR_H