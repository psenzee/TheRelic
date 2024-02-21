/*
 *  Maze_.cpp
 *  GLGravity
 *
 *  Created by Paul Senzee on 12/29/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */


#include "Maze.h"
#include "core/random.h"
#include "TileMap.h"

#include <stdio.h>
#include <stdlib.h>

ITileMap *MazeGenerator::CreateEmptyMap(const core::Size &sz)
{
    return TileMaps::CreateMap(sz);
}

ITileMap *MazeGenerator::CopyMap(ITileMap *map, const core::Rectangle &rect)
{
    return TileMaps::Copy(map, rect);
}

void MazeGenerator::Create(ITileMap *maze, int seed, bool exits)
{
    core::Random random(seed);
    core::Size   size   = maze->GetSizeInTiles();
    
    int x = 0, y = 0, n = 0, d = 0;
    
    int  dx[]    = { 0, 0, -1, 1 };
    int  dy[]    = { -1, 1, 0, 0 };
    int *todo    = new int [size.width * size.height];
    int  todonum = 0;
    
    // We start with a grid full of walls. 
    for (x = 0; x < size.width; ++x)
    {
        for (y = 0; y < size.height; ++y)
        {
            bool inside = (x == 0 || x == (size.width - 1) || y == 0 || y == (size.height - 1));
            maze->SetTileId(x, y, inside ? INSIDE : ALL);
        }
    }
    
    // Select any square of the grid, to start with. 
    x = (int) (1 + random.real() * (size.width  - 2));
    y = (int) (1 + random.real() * (size.height - 2));
    
    // Mark this square as connected to the maze. 
    AndTileId(maze, x, y, ~(QUEUED | INSIDE));
    
    // Remember the surrounding squares, as we will 
    for (d = 0; d < 4; ++d)
    {
        if ((maze->GetTileId(x + dx[d], y + dy[d]) & QUEUED) != 0)
        {
            // want to connect them to the maze. 
            
            /* alternately, you could use a struct to store the two integers
             * this would result in easier to read code, though not as speedy
             * of course, if you were worried about speed, you wouldn't be using Java
             * you could also use a single integer which represents (x + y * width)
             * this would actually be faster than the current approach
             * - quin/10-24-06
             *    Actually, the former wouldn't work in Java- there's no such thing as a
             *    struct. It's a class or nothing, I'm afraid.
             *    - Jae Armstrong/23-03-07
             */                    
            todo[todonum++] = ((x + dx[d]) << 16) | (y + dy[d]);
            AndTileId(maze, x + dx[d], y + dy[d], ~QUEUED);
        }
    }
    
    // We won't be finished until all is connected. 
    while (todonum > 0)
    {
        // We select one of the squares next to the maze. 
        n = (int) (random.real() * todonum);
        x = todo[n] >> 16; // the top 2 bytes of the data 
        y = todo[n] & 65535; // the bottom 2 bytes of the data 
        
        // We will connect it, so remove it from the queue. 
        todo[n] = todo[--todonum];
        
        // Select a direction, which leads to the maze. 
        do { d = (int) (random.real() * 4); }
        while ((maze->GetTileId(x + dx[d], y + dy[d]) & INSIDE) != 0);
        
        // Connect this square to the maze. 
        AndTileId(maze, x,         y,         ~((1 << d) | INSIDE));
        AndTileId(maze, x + dx[d], y + dy[d], ~( 1 << (d ^ 1)));
        
        // Remember the surrounding squares, which aren't 
        for (d = 0; d < 4; ++d)
        {
            if ((maze->GetTileId(x + dx[d], y + dy[d]) & QUEUED) != 0)
            {                       
                // connected to the maze, and aren't yet queued to be. 
                todo[todonum++] = ((x + dx[d]) << 16) | (y + dy[d]);
                AndTileId(maze, x + dx[d], y + dy[d], ~QUEUED);
            }
        }
        // Repeat until finished.
    }
    
    if (exits)
    {
        // One may want to add an entrance and exit. 
        AndTileId(maze, 1, 1, ~ABOVE); // we'll want to put these in different spots..
        AndTileId(maze, size.width - 2, size.height - 2, ~BELOW);
    }
    
    delete [] todo;
    
    // Clean up everything..
    for (x = 0; x < size.width; ++x)
        for (y = 0; y < size.height; ++y)
            AndTileId(maze, x, y, WALLS);
}

ITileMap *MazeGenerator::CreateExpandedMap(const core::Size &size, int seed, bool exits)
{
    enum { MULTIPLE = 6 };
    
    core::Size  fsz(size.width * MULTIPLE, size.height * MULTIPLE);
    ITileMap   *small = CreateEmptyMap(size),
    *maze  = CreateEmptyMap(fsz);
    Create(small, seed, exits);
    for (int y = 0; y < size.height; y++)
        if ((small->GetTileId(size.width - 2, y) & RIGHT) > 0)
            small->SetTileId(size.width - 1, y, LEFT);
    for (int x = 0; x < size.width; x++)
        if ((small->GetTileId(x, size.height - 2) & BELOW) > 0)
            small->SetTileId(x, size.height - 1, ABOVE);
    for (int x = 0; x < fsz.width; ++x)
        for (int y = 0; y < fsz.height; ++y)
            maze->SetTileId(x, y, 0);
    for (int x = 0; x < size.width; ++x)
    {
        for (int y = 0; y < size.height; ++y)
        {
            int  id   = small->GetTileId(x, y);
            long mask = ((id & 2) > 0 ? 0L/*0xfc0000000L*/ : 0) |
            ((id & 1) > 0 ? 0x00000003fL : 0) |
            ((id & 8) > 0 ? 0L/*0x820820820L*/ : 0) |
            ((id & 4) > 0 ? 0x041041041L : 0);
            for (int k = 0; k < MULTIPLE * MULTIPLE; k++)
                maze->SetTileId((k % MULTIPLE) + x * MULTIPLE, 
                                (k / MULTIPLE) + y * MULTIPLE,
                                (int)((mask >> k) & 1));
        }
    }
    delete small; small = 0;
    for (int x = 0; x < fsz.width; x += MULTIPLE)
    {
        for (int y = 0; y < fsz.height; y += MULTIPLE)
        {
            int xL = x,                yU = y,
            xR = x + MULTIPLE - 1, yD = y + MULTIPLE - 1;
            if (maze->GetTileId(xL, yU) == 0 &&
                maze->GetTileId(xL - 1, yU) == 1 &&
                maze->GetTileId(xL, yU - 1) == 1)
                maze->SetTileId(xL, yU, 1);
        }
    }
    return maze;
}

void MazeGenerator::FillMapSegment(ITileMap &map, int x, int y, int multiple, int flags)
{
    if (flags & ABOVE)
         for (int i = 0; i <= multiple; i++)
             map.SetTileId(x + i, y, 1);
    if (flags & BELOW)
         for (int i = 0; i <= multiple; i++)
             map.SetTileId(x + i, y + multiple, 1);
    if (flags & LEFT)
         for (int i = 0; i <= multiple; i++)
             map.SetTileId(x, y + i, 1);
    if (flags & RIGHT)
         for (int i = 0; i <= multiple; i++)
             map.SetTileId(x + multiple, y + i, 1);
}

ITileMap *MazeGenerator::CreateExpandedMap(const core::Size &size, int seed, int multiple, bool exits)
{
    core::Size  fsz(size.width * multiple + 1, size.height * multiple + 1);
    ITileMap   *small = CreateEmptyMap(size),
               *maze  = CreateEmptyMap(fsz);
    
    Create(small, seed, exits);
    /*
    // $FROM HERE
    for (int y = 0; y < size.height; y++)
        if ((small->GetTileId(size.width - 2, y) & RIGHT) > 0)
            small->SetTileId(size.width - 1, y, LEFT);
    for (int x = 0; x < size.width; x++)
        if ((small->GetTileId(x, size.height - 2) & BELOW) > 0)
            small->SetTileId(x, size.height - 1, ABOVE);
    // $TO HERE - is this necessary/useful?
*/            
    // clear out destination maze
    for (int y = 0; y < fsz.height; ++y)
        for (int x = 0; x < fsz.width; ++x)
            maze->SetTileId(x, y, 0);

    int flags = 0;
    for (int y = 0; y < size.height; ++y)            
        for (int x = 0; x < size.width; ++x)
            FillMapSegment(*maze, x * multiple, y * multiple, multiple, small->GetTileId(x, y));
    delete small; small = 0;
    /*
    for (int x = 0; x < fsz.width; x += multiple)
    {
        for (int y = 0; y < fsz.height; y += multiple)
        {
            int xL = x,                yU = y,
            xR = x + multiple - 1, yD = y + multiple - 1;
            if (maze->GetTileId(xL, yU) == 0 &&
                maze->GetTileId(xL - 1, yU) == 1 &&
                maze->GetTileId(xL, yU - 1) == 1)
                maze->SetTileId(xL, yU, 1);
        }
    }
    */
    return maze;
}

ITileMap *MazeGenerator::CreateMapWithWallTypes(const core::Size &size, int seed, bool exits)
{
    ITileMap *map = PostProcess5x5(CreateExpandedMap(size, seed, exits));
    core::Size sz = map->GetSizeInTiles();
    ITileMap *crop = CopyMap(map, core::Rectangle(2, 2, sz.width - 3, sz.height - 3));
    delete map; map = 0;
    return crop;
}

ITileMap *convert(ITileMap *map)
{
    core::Size sz = map->GetSizeInTiles();    
    for (int y = 0; y < sz.height; ++y)
        for (int x = 0; x < sz.width; ++x)
            map->SetTileId(x, y, map->GetTileId(x, y) ? MazeGenerator::VERTICAL : 1);
    return map;
}

ITileMap *MazeGenerator::PostProcess(ITileMap *map)
{
    core::Size sz = map->GetSizeInTiles();    
    ITileMap *newmap = CreateEmptyMap(sz);
    for (int y = 0; y < sz.height; ++y)
    {
        for (int x = 0; x < sz.width; ++x)
        {
            int at = map->GetTileId(x, y);
            if (!at)
                newmap->SetTileId(x, y, MazeGenerator::NONE);
            else if (map->GetTileId(x - 1, y) && map->GetTileId(x, y + 1) && !map->GetTileId(x + 1, y))            
                newmap->SetTileId(x, y, MazeGenerator::VERTICAL);            
            else if (map->GetTileId(x - 1, y) && map->GetTileId(x, y - 1) && !map->GetTileId(x + 1, y))
                newmap->SetTileId(x, y, MazeGenerator::CORNER);                        
            else if (map->GetTileId(x + 1, y) && map->GetTileId(x, y + 1))            
                newmap->SetTileId(x, y, MazeGenerator::TEE);
            else if (map->GetTileId(x + 1, y) || map->GetTileId(x - 1, y))
                newmap->SetTileId(x, y, MazeGenerator::HORIZONTAL);
            else if (map->GetTileId(x, y + 1) || map->GetTileId(x, y + 1))
                newmap->SetTileId(x, y, MazeGenerator::VERTICAL);
            else
                newmap->SetTileId(x, y, MazeGenerator::VERTICAL);
        }
    }
    delete map; map = 0;
    return newmap;
}

ITileMap *MazeGenerator::CreateMapWithWallTypes(const core::Size &size, int seed, int multiple, bool exits)
{
//  ITileMap *map = PostProcess5x5(CreateExpandedMap(size, seed, multiple, exits));
    ITileMap *map = PostProcess(CreateExpandedMap(size, seed, multiple, exits));
    core::Size sz = map->GetSizeInTiles();
    ITileMap *crop = CopyMap(map, core::Rectangle(multiple, multiple, sz.width - multiple - multiple - 4, sz.height - multiple - multiple - 4));
    delete map; map = 0;
    return crop;
}

ITileMap *MazeGenerator::CreateMapWithWallTypesAndOrnateFloor(const core::Size &size, int seed, int multiple, bool exits)
{
    ITileMap *map = CreateMapWithWallTypes(size, seed, multiple, exits);
    core::Size sz = map->GetSizeInTiles();
    ITileMap *floor = CreateEmptyMap(sz);
    Create(floor, seed, exits);
    for (int y = 0; y < sz.height; ++y)
        for (int x = 0; x < sz.width; ++x)
            if (map->GetTileId(x, y) == NONE)
                map->SetTileId(x, y, floor->GetTileId(x, y) + FLOOR_TILE_BASE);
    delete floor;
    return map;    
}

int MazeGenerator::GetBitPattern(ITileMap *map, int size, int x, int y)
{
    int pattern = 0;
    for (int j = 0; j < size; j++)
    {
        for (int i = 0; i < size; i++)
        {
            pattern <<= 1;
            if (map->GetTileId(x + i, y + j) > 0)
                pattern |= 1;
        }
    }
    return pattern;
}

MazeGenerator::WallType MazeGenerator::Lookup5x5(int bits)
{
    WallType id = NONE;
    switch (bits & 0x00739c0)
    {
        case 0x0040000: id = CORNER;     break; 
        case 0x0042100: id = VERTICAL;   break; 
        case 0x0070000: id = HORIZONTAL; break; 
        case 0x0072100: id = TEE;        break; 
    }
    if (id == VERTICAL   && (bits & 0x0800000) == 0) id = VERTICAL_END_TOP;
    if (id == VERTICAL   && (bits & 0x0000008) == 0) id = VERTICAL_END_BOTTOM;
    if (id == HORIZONTAL && (bits & 0x0080000) == 0) id = HORIZONTAL_END_LEFT;
    if (id == HORIZONTAL && (bits & 0x0008000) == 0) id = HORIZONTAL_END_RIGHT;
    return id;
}

ITileMap *MazeGenerator::PostProcess5x5(ITileMap *map)
{
    core::Size originalSize = map->GetSizeInTiles(),
    size         = core::Size((originalSize.width  + 2) / 3,
                              (originalSize.height + 2) / 3);
    ITileMap *newMap = CreateEmptyMap(size);
    for (int y = 0; y < size.height; y++)
        for (int x = 0; x < size.width; x++)
            newMap->SetTileId(x, y, (int)Lookup5x5(GetBitPattern(map, 5, x * 3 - 1, y * 3 - 1)));
    delete map;
    return newMap;
}

int MazeGenerator::AndTileId(ITileMap *map, int x, int y, int value)
{
    int v = map->GetTileId(x, y) & value;
    map->SetTileId(x, y, v);
    return v;
}
