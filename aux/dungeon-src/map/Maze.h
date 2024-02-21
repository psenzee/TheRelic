/*
 *  Maze_.h
 *  GLGravity
 *
 *  Created by Paul Senzee on 12/29/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _MAZE_H
#define _MAZE_H

#include "core/xna.h"

class ITileMap;

class MazeGenerator
{    
public:
    
    enum WallType
    {
        NONE                 = 1,
        CORNER               = 20,
        VERTICAL             = 193,
        VERTICAL_END_TOP     = 17,
        VERTICAL_END_BOTTOM  = 22,
        HORIZONTAL           = 161,
        HORIZONTAL_END_LEFT  = 18,
        HORIZONTAL_END_RIGHT = 19,
        TEE                  = 353
    };
    
    enum FloorTileType
    {
        FLOOR_TILE_BASE       = 1024,
    };
    
    // ornate floor types
    // FLOOR_BLANK                  = 1024
    // FLOOR_ABOVE                  = 1025
    // FLOOR_BELOW                  = 1026
    // FLOOR_ABOVE_BELOW            = 1027
    // FLOOR_LEFT                   = 1028
    // FLOOR_LEFT_ABOVE             = 1029
    // FLOOR_LEFT_BELOW             = 1030
    // FLOOR_LEFT_ABOVE_BELOW       = 1031
    // FLOOR_RIGHT                  = 1032
    // FLOOR_RIGHT_ABOVE            = 1033
    // FLOOR_RIGHT_BELOW            = 1034
    // FLOOR_RIGHT_ABOVE_BELOW      = 1035
    // FLOOR_RIGHT_LEFT             = 1036
    // FLOOR_RIGHT_LEFT_ABOVE       = 1037
    // FLOOR_RIGHT_LEFT_BELOW       = 1038
    // FLOOR_RIGHT_LEFT_ABOVE_BELOW = 1039
    
private:
    
    enum WallMask 
    { 
        ABOVE  = 1, 
        BELOW  = 2, 
        LEFT   = 4, 
        RIGHT  = 8, 
        QUEUED = 16, 
        INSIDE = 32,
        WALLS  = ABOVE | BELOW | LEFT | RIGHT,
        ALL    = WALLS | QUEUED | INSIDE
    };
        
public:
       
    void             Create(ITileMap *maze, int seed, bool exits);
          
    ITileMap        *CreateMapWithWallTypes(const core::Size &size, int seed, bool exits);
    ITileMap        *CreateMapWithWallTypes(const core::Size &size, int seed, int multiple, bool exits);
    ITileMap        *CreateMapWithWallTypesAndOrnateFloor(const core::Size &size, int seed, int multiple, bool exits);
        
private:
        
    ITileMap        *CreateExpandedMap(const core::Size &size, int seed, bool exits);
    ITileMap        *CreateExpandedMap(const core::Size &size, int seed, int multiple, bool exits);        
        
    static void      FillMapSegment(ITileMap &map, int x, int y, int multiple, int flags);
        
    static ITileMap *CreateEmptyMap(const core::Size &sz);
    static ITileMap *CopyMap(ITileMap *map, const core::Rectangle &rect);

    static ITileMap *PostProcess(ITileMap *map);
    
    static int       GetBitPattern(ITileMap *map, int size, int x, int y);
    static WallType  Lookup5x5(int bits);
    static ITileMap *PostProcess5x5(ITileMap *map);
    static int       AndTileId(ITileMap *map, int x, int y, int value);
};

#endif // _MAZE_H