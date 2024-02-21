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
        
    ITileMap        *CreateExpandedMap(const core::Size &size, int seed, bool exits);
    ITileMap        *CreateMapWithWallTypes(const core::Size &size, int seed, bool exits);
        
private:
        
    static ITileMap *CreateEmptyMap(const core::Size &sz);
    static ITileMap *CopyMap(ITileMap *map, const core::Rectangle &rect);
        
    static int       GetBitPattern(ITileMap *map, int size, int x, int y);
    static WallType  Lookup5x5(int bits);
    static ITileMap *PostProcess5x5(ITileMap *map);
    static int       AndTileId(ITileMap *map, int x, int y, int value);
};

#endif // _MAZE_H