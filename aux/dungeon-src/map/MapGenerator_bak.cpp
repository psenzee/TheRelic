/*
 *  MapGenerator.cpp
 *  GLGravity
 *
 *  Created by Paul Senzee on 12/29/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include "Map.h"

#include "TileMap.h"
#include "MapGenerator.h"
#include "Maze.h"
#include "DeviceTileSet.h"

#include "core/random.h"
#include "core/global.h"
#include "render/LocalGameServices.h"

MapGenerator::MapGenerator(LocalGameServices *services) : Services(services)
{
}

#ifdef LITE_VERSION
Map *MapGenerator::GenerateMap(int level, const core::Size &mapsz, XmlElement *levelConfig, const char *mapTypeName, int seed, int spiralLevel)
{
    MapFile   options(levelConfig, mapTypeName);
    ITileMap *map     = 0;
    // generate level
    const char *tilesets[]        = { 
        "tileset_default101.xml",
        "tileset_default102.xml",
        "tileset_default103.xml",
        "tileset_default104.xml",
        /*
        "tileset_default105.xml",
        "tileset_default106.xml",
         */
        "tileset_default116.xml",
        "tileset_default117.xml",        
        "tileset_default107.xml",        
        "tileset_default108.xml",        
      //"tileset_default109.xml",
        "tileset_default107.xml",        
      //"tileset_default110.xml",        
        "tileset_default111.xml",
        "tileset_default112.xml",
      //"tileset_default113.xml",
        "tileset_default114.xml",        
      //"tileset_default115.xml",
//      "tileset_default116.xml",
//      "tileset_default117.xml",   
        "tileset_default118.xml",
//      "tileset_default118.xml",        
        "tileset_default119.xml",
      //"tileset_default119.xml",        
      //"tileset_default120.xml",
        "tileset_default121.xml",
        "tileset_default123.xml",
        "tileset_default110.xml",
        "tileset_default116.xml",
        "tileset_default117.xml",
        /*
        "tileset_default125.xml",
        "tileset_default126.xml",
        "tileset_default128.xml",        
        "tileset_default129.xml",
        "tileset_default127.xml",
        "tileset_default107.xml",
         */
//        "tileset_default101.xml",    
            "tileset_default107b.xml",    
            "tileset_default107b.xml",    
            "tileset_default107b.xml",            
        "tileset_default101.xml",    
        "tileset_default101.xml",    
        "tileset_default101.xml",    
        "tileset_default101.xml",    
        "tileset_default101.xml",            
        
        "tileset_default107.xml",        
        "tileset_default129.xml",        
        "tileset_default128.xml",
        "tileset_default126.xml",        
        "tileset_default125.xml",        
        "tileset_default110.xml",        
        /*
        "tileset_default123.xml",        
        "tileset_default121.xml",        
        "tileset_default119.xml",        
        "tileset_default118.xml",        
        "tileset_default114.xml",                
         */
        "tileset_default112.xml",        
        "tileset_default108.xml",                
        "tileset_default104.xml",                
        "tileset_default103.xml",        
        "tileset_default102.xml",        
        "tileset_default101.xml",        

        "tileset_default101.xml", "tileset2x.xml",  // gardens
        "tileset3x.xml",                    // glass loo
        "tileset5x.xml",  "tileset5x.xml",  // test tracks
        //   "tileset4x.xml",  "tileset16x.xml", // warehouses
        "tileset7x.xml",                    // the loo
        "tileset10x.xml",                   // the cocina
        "tileset9x.xml",                    // technicolor lawn        
        "tileset1x.xml",                      // backyard
        "tileset5x.xml",                    // fallout shelter (test track)
        "tileset12x.xml",                   // FPS
        "tileset6x.xml", "tileset15x.xml",  // castillos
        "tileset14x.xml",                   // incarceration
        "tileset13x.xml",                   // ice palace        
    };
    const char *tilesets_common[] = { "tilesets.xml", "tilesets_brick.xml",
                                      "tilesets.xml",
                                      "tilesets.xml", "tilesets.xml",
                                  //  "tilesets.xml", "tilesets.xml",        
                                      "tilesets.xml",
                                      "tilesets.xml",
                                      "tilesets.xml",
                                      "tilesets_superball.xml",
                                       "tilesets.xml",
                                       "tilesets.xml",
                                      "tilesets_brick.xml",    "tilesets_brick.xml",
                                      "tilesets.xml",
                                      "tilesets.xml"        
                                    };
    MazeGenerator mg;
    core::Random r(level + seed);
    map = mg.CreateMapWithWallTypes(mapsz, r.integer(), 6, false);
    core::Size sz = map->GetSizeInTiles();
        
    SetCheckeredFloor(map);
    core::Point start = GetSpiral8MapStart(map, spiralLevel);
    //SetTrees(map, r, start);
    
    printf("Map Size %u, %u\n", sz.width, sz.height);
/*    
    if (sz.width > 16 && sz.height > 16)
        MakeParkType1(map, core::Rectangle(3, 3, 10, 10), r, start, false, 0);
*/    
    SetSpecialTilesForSpiral8Map(map, spiralLevel);
    SetRandomPickups(map, r);
    
    const int TILESET_COUNT        = sizeof(tilesets)        / sizeof(tilesets[0]),
              TILESET_COMMON_COUNT = sizeof(tilesets_common) / sizeof(tilesets_common[0]);
    
    int tileset_index        = (level/* + seed*/) % TILESET_COUNT,
        tileset_common_index = tileset_index % TILESET_COMMON_COUNT;

    printf("Creating map real level %d, faux level %d, tileset %d (%s), tileset common %d (%s), size %d\n", spiralLevel, level, tileset_index, tilesets[tileset_index], tileset_common_index, tilesets_common[tileset_common_index], sz.width);
    
    options.tiles = tilesets[tileset_index];
    ((CoreTileMap *)map)->Reverse();
    ITileMapModel *model = 
        new SimpleTileMapModel(DeviceTileSetLoader::GetTileSet(options.tiles, tilesets_common[tileset_common_index], Services), map);
    
    return new Map(Services, model, &options);
}
#else
Map *MapGenerator::GenerateMap(int level, const core::Size &mapsz, XmlElement *levelConfig, const char *mapTypeName, int seed, int spiralLevel)
{
    MapFile   options(levelConfig, mapTypeName);
    ITileMap *map     = 0;
    // generate level
    const char *tilesets[]        = { 
        "tileset_default107.xml",   
        "tileset_default107.xml",   
        "tileset_default107.xml",           
        "tileset_default107.xml", 
        /*
        "tileset_default105.xml",
        "tileset_default106.xml",
         */
        "tileset_default116.xml",
        "tileset_default117.xml",        
        "tileset_default107.xml",        
        "tileset_default108.xml",        
      //"tileset_default109.xml",
        "tileset_default107.xml",        
      //"tileset_default110.xml",        
        "tileset_default111.xml",
        "tileset_default112.xml",
      //"tileset_default113.xml",
        "tileset_default114.xml",        
      //"tileset_default115.xml",
//      "tileset_default116.xml",
//      "tileset_default117.xml",   
        "tileset_default118.xml",
//      "tileset_default118.xml",        
        "tileset_default119.xml",
      //"tileset_default119.xml",        
      //"tileset_default120.xml",
        "tileset_default121.xml",
        "tileset_default123.xml",
        "tileset_default110.xml",
        "tileset_default116.xml",
        "tileset_default117.xml",        
        "tileset_default125.xml",
        "tileset_default126.xml",
        "tileset_default128.xml",        
        "tileset_default129.xml",
        "tileset_default127.xml",
        "tileset_default107.xml",
        
        "tileset_default107.xml",        
        "tileset_default129.xml",        
        "tileset_default128.xml",
        "tileset_default126.xml",        
        "tileset_default125.xml",        
        "tileset_default110.xml",        
        /*
        "tileset_default123.xml",        
        "tileset_default121.xml",        
        "tileset_default119.xml",        
        "tileset_default118.xml",        
        "tileset_default114.xml",                
         */
        "tileset_default112.xml",        
        "tileset_default108.xml",                
        "tileset_default104.xml",                
        "tileset_default103.xml",        
        "tileset_default102.xml",        
        "tileset_default101.xml",        

        "tileset_default101.xml", "tileset2x.xml",  // gardens
        "tileset3x.xml",                    // glass loo
        "tileset5x.xml",  "tileset5x.xml",  // test tracks
        //   "tileset4x.xml",  "tileset16x.xml", // warehouses
        "tileset7x.xml",                    // the loo
        "tileset10x.xml",                   // the cocina
        "tileset9x.xml",                    // technicolor lawn        
        "tileset1x.xml",                      // backyard
        "tileset5x.xml",                    // fallout shelter (test track)
        "tileset12x.xml",                   // FPS
        "tileset6x.xml", "tileset15x.xml",  // castillos
        "tileset14x.xml",                   // incarceration
        "tileset13x.xml",                   // ice palace        
    };
    const char *tilesets_common[] = { "tilesets_brick.xml", "tilesets_brick.xml",
        /*
                                      "tilesets.xml",
                                      "tilesets.xml", "tilesets.xml",
                                  //  "tilesets.xml", "tilesets.xml",        
                                      "tilesets.xml",
                                      "tilesets.xml",
                                      "tilesets.xml",
                                      "tilesets_brick.xml",
                                       "tilesets.xml",
                                       "tilesets.xml",
                                      "tilesets_brick.xml",    "tilesets_brick.xml",
                                      "tilesets.xml",
                                      "tilesets.xml"        */
                                    };
    MazeGenerator mg;
    core::Random r(level + seed);
    map = mg.CreateMapWithWallTypes/*AndOrnateFloor*/(core::Size(20, 20), r.integer(), 6, false);
    core::Size sz = map->GetSizeInTiles();
        
           
  //  SetCheckeredFloor(map);
    ITileMap *pattern = TileMaps::CreateMap(core::Size(4, 4));
    for (int i = 0; i < 16; i++)
        pattern->SetTileId(i % 4, i / 4, 1040 + (15 - i));
    SetPatternedFloor(map, core::Rectangle(0, 0, sz.width, sz.height), pattern);
    core::Point start = GetSpiral8MapStart(map, level);
    //SetTrees(map, r, start);
    
    printf("Map Size %u, %u\n", sz.width, sz.height);
/*    
    if (sz.width > 16 && sz.height > 16)
        MakeParkType1(map, core::Rectangle(3, 3, 10, 10), r, start, false, 0);
*/    
    SetSpecialTilesForSpiral8Map(map, level);
    SetRandomPickups(map, r);
    
    const int TILESET_COUNT        = sizeof(tilesets)        / sizeof(tilesets[0]),
              TILESET_COMMON_COUNT = sizeof(tilesets_common) / sizeof(tilesets_common[0]);
    
    int tileset_index        = (level/* + seed*/) % TILESET_COUNT,
        tileset_common_index = tileset_index % TILESET_COMMON_COUNT;
    
    options.tiles = tilesets[tileset_index];
    ((CoreTileMap *)map)->Reverse();
    ITileMapModel *model = 
        new SimpleTileMapModel(DeviceTileSetLoader::GetTileSet(options.tiles, tilesets_common[tileset_common_index], Services), map);
    
    for (int j = 0; j < sz.height; j++)
    {
        for (int i = 0; i < sz.width; i++)
            printf("%4d ", map->GetTileId(i, j));
        printf("\n");        
    }    
    
    return new Map(Services, model, &options);
}
#endif

void MapGenerator::SetCheckeredFloor(ITileMap *map)
{
    core::Size sz = map->GetSizeInTiles();
    for (int j = 0; j < sz.height; j++)
        for (int i = 0; i < sz.width; i++)
            if (((i + j) % 2) == 0 && map->GetTileId(i, j) == (int)Map::FLOOR_TILE1)
                map->SetTileId(i, j, (int)Map::FLOOR_TILE2);
}
    
float MapGenerator::StartDistance(const core::Point &start, const core::Point &at)
{
    int dx = at.x - start.x, dy = at.y - start.y;
    return sqrtf((float)(dx * dx + dy * dy));
}
    
void MapGenerator::SetTrees(ITileMap *map, core::Random &r, const core::Point &start)
{
    core::Size sz = map->GetSizeInTiles();
    for (int j = 0; j < sz.height; j++)
    {
        for (int i = 0; i < sz.width; i++)
        {
            if (Map::IsFloorTile(map->GetTileId(i, j))) // 1 is the main floor tile
            {
                int rand = r.integer() % (int)RANDOM_TREE;
                if (StartDistance(start, core::Point(i, j)) > 2.5) // don't put any trees close to the start
                {
                    if      (rand == 0) map->SetTileId(i, j, (int)Map::TREE_TILE1);
                    else if (rand == 1) map->SetTileId(i, j, (int)Map::TREE_TILE2);
                }
            }
        }
    }
}
        
void MapGenerator::SetRandomPickups(ITileMap *map, core::Random &r)
{
    Map::SpecialTile choice[] = { Map::SWIRL };
    core::Size sz = map->GetSizeInTiles();
    for (int j = 0; j < sz.height; j++)
    {
        for (int i = 0; i < sz.width; i++)
        {
            Map::SpecialTile pickup = choice[r.integer() % (sizeof(choice) / sizeof(choice[0]))];
            if ((r.integer() % (int)RANDOM_PICKUP) == 0 &&
                Map::IsFloorTile(map->GetTileId(i, j)))
                map->SetTileId(i, j, (int)pickup);
        }
    }
}
        
core::Point MapGenerator::GetSpiral8MapStart(ITileMap *map, int level)
{
    core::Size size = map->GetSizeInTiles();
    int  sx = 1, sy = 1,
         ex = size.width  - 2,
         ey = size.height - 2;
        
    core::Point starts[] = 
    {
        core::Point(sx, ey),
        core::Point(sx, sy),
        core::Point(sx, sy),
        core::Point(ex, sy),
        core::Point(ex, sy),
        core::Point(ex, ey),
        core::Point(ex, ey),
        core::Point(sx, ey)
    };
            
    return starts[level % 8];
}
        
void MapGenerator::SetSpecialTilesForSpiral8Map(ITileMap *map, int level)
{
    core::Size size = map->GetSizeInTiles();
    int  sx = 1, sy = 1,
         ex = size.width  - 2,
         ey = size.height - 2;
            
    core::Point starts[] = 
    {
        core::Point(sx, ey),
        core::Point(sx, sy),
        core::Point(sx, sy),
        core::Point(ex, sy),
        core::Point(ex, sy),
        core::Point(ex, ey),
        core::Point(ex, ey),
        core::Point(sx, ey)
    };
            
    core::Point ends[] =
    {
        core::Point(ex, ey),
        core::Point(ex, ey),
        core::Point(sx, ey),
        core::Point(sx, ey),
        core::Point(sx, sy),
        core::Point(sx, sy),
        core::Point(ex, sy),
        core::Point(ex, sy)
    };
            
    core::Point pickup2x[] =
    {
        core::Point(sx, sy),
        core::Point(ex, sy),
        core::Point(ex, sy),
        core::Point(sx, sy),
        core::Point(ex, ey),
        core::Point(ex, sy),
        core::Point(sx, ey),
        core::Point(sx, sy)
    };
            
    core::Point pickup3x[] =
    {
        core::Point(ex, sy),
        core::Point(ex, sy),
        core::Point(ex, ey),
        core::Point(ex, ey),
        core::Point(sx, ey),
        core::Point(sx, ey),
        core::Point(sx, sy),
        core::Point(ex, ey)
    };
            
    level %= 8;
            
    core::Point s  = starts  [level],
                e  = ends    [level],
                x2 = pickup2x[level],
                x3 = pickup3x[level];
            
    map->SetTileId(s.x, s.y,   (int)Map::START_TILE);
    map->SetTileId(e.x, e.y,   (int)Map::END_TILE);
    map->SetTileId(x2.x, x2.y, (int)Map::PICKUP_2X);
    map->SetTileId(x3.x, x3.y, (int)Map::PICKUP_3X);
}

void MapGenerator::SetTrees(ITileMap *map, const core::Rectangle &area, core::Random &r, const core::Point &start)
{
    for (int j = area.y; j < area.height; j++)
    {
        for (int i = area.x; i < area.width; i++)
        {
            if (Map::IsFloorTile(map->GetTileId(i, j))) // 1 is the main floor tile
            {
                int rand = r.integer() % (int)RANDOM_TREE;
                if (StartDistance(start, core::Point(i, j)) > 2.5f) // don't put any trees close to the start
                {
                    if      (rand == 0) map->SetTileId(i, j, (int)Map::TREE_TILE1);
                    else if (rand == 1) map->SetTileId(i, j, (int)Map::TREE_TILE2);
                }
            }
        }
    }
}

void MapGenerator::Clear(ITileMap *map, const core::Rectangle &area, int clear)
{
    for (int j = area.y; j < area.height; j++)
        for (int i = area.x; i < area.width; i++)
            map->SetTileId(i, j, clear);
}

void MapGenerator::SetPatternedFloor(ITileMap *map, const core::Rectangle &area, ITileMap *pattern)
{
    core::Size sz = pattern->GetSizeInTiles();
    for (int j = area.y; j < area.height; j++)
        for (int i = area.x; i < area.width; i++)
            if (map->GetTileId(i, j) == (int)Map::FLOOR_TILE1)
                map->SetTileId(i, j, pattern->GetTileId(i % sz.width, j % sz.height));
}

void MapGenerator::SetCheckeredFloor(ITileMap *map, const core::Rectangle &area)
{
    for (int j = area.y; j < area.height; j++)
        for (int i = area.x; i < area.width; i++)
            if (((i + j) % 2) == 0 && map->GetTileId(i, j) == (int)Map::FLOOR_TILE1)
                map->SetTileId(i, j, (int)Map::FLOOR_TILE2);
}

void MapGenerator::MakeFence(ITileMap *map, const core::Rectangle &area)
{
    int x0 = area.x, y0 = area.y, x1 = area.width - 1, y1 = area.height;
    for (int i = area.x; i < area.width; i++)
    {
        map->SetTileId(i, y0, MazeGenerator::HORIZONTAL);
        map->SetTileId(i, y1, MazeGenerator::HORIZONTAL);
    }
    for (int i = area.y; i < area.height; i++)
    {
        map->SetTileId(x0, i, MazeGenerator::VERTICAL);
        map->SetTileId(x1, i, MazeGenerator::VERTICAL);
    }    
    map->SetTileId(x0, y0, MazeGenerator::TEE);
    map->SetTileId(x1, y1, MazeGenerator::VERTICAL_END_BOTTOM);
//    map->SetTileId(x0, y1, MazeGenerator::HORIZONTAL_END_LEFT);
//    map->SetTileId(x1, y0, MazeGenerator::VERTICAL_END_TOP);        
//    map->SetTileId(x1, y1, MazeGenerator::HORIZONTAL_END_RIGHT); // or
//    map->SetTileId(x1, y1, MazeGenerator::VERTICAL_END_BOTTOM);  // ?   
}

void MapGenerator::MakeParkType1(ITileMap *map, const core::Rectangle &area, core::Random &r, const core::Point &start, bool treesOutside, ITileMap *pattern)
{
    Clear(map, area, Map::FLOOR_TILE1);
    if (!pattern) SetCheckeredFloor(map, area);
    else          SetPatternedFloor(map, area, pattern);
    core::Rectangle fence(area.x + 1, area.y + 1, area.width - 2, area.height - 2);        
    SetTrees(map, treesOutside ? area : fence, r, start);
    MakeFence(map, fence);
}