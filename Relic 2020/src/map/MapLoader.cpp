#include "Map.h"

#include "TileMap.h"
#include "MapLoader.h"
#include "ObjectTileSet.h"
#include "MapDescription.h"

#include "core/random.h"
#include "core/global.h"

#include <stdio.h>

Map *MapLoader::Create(const MapDescription &description, ContentLoader *loader)
{
    printf("Loading map file..\n");        
    CoreTileMap *map = (CoreTileMap *)TileMaps::CreateMap(core::Size(1, 1));
    if (!map->Read(globalTranslatePath(description.filename.c_str())))
    {
        printf("Unable to load map file '%s'!\n", description.filename.c_str());
        return 0;
    }
    printf("Map file loaded!\n");
    core::Size sz = map->GetSizeInTiles();
//  FillSequentialPattern(map, 1, core::Rectangle(0, 0, sz.width, sz.height), 1040, core::Size(4, 4));        
//  FillSequentialPattern(map, 2, core::Rectangle(0, 0, sz.width, sz.height), 1060, core::Size(4, 4));        
    ((CoreTileMap *)map)->Reverse();
    printf("Map Size %u, %u\n", sz.width, sz.height);

    ITileMapModel *model = 
        new SimpleTileMapModel(ObjectTileSetLoader::GetTileSet(description.tileset.c_str(), loader), map);
    
    return new Map(model);
}

void MapLoader::FillSequentialPattern(ITileMap *map, int id, const core::Rectangle &area, int startid, const core::Size &patternSize)
{
    ITileMap *pattern = CreateSequentialPattern(startid, patternSize);
    FillPattern(map, id, area, pattern);
    delete pattern;
}

ITileMap *MapLoader::CreateSequentialPattern(int startid, const core::Size &size)
{
    ITileMap *pattern = TileMaps::CreateMap(size);
    for (int i = 0, sz = size.width * size.height; i < sz; i++)
        pattern->SetTileId(i % size.width, i / size.width, startid + i);
    return pattern;
}

void MapLoader::Clear(ITileMap *map, const core::Rectangle &area, int clear)
{
    for (int j = area.y; j < area.height; j++)
        for (int i = area.x; i < area.width; i++)
            map->SetTileId(i, j, clear);
}

void MapLoader::FillPattern(ITileMap *map, int id, const core::Rectangle &area, ITileMap *pattern)
{
    core::Size sz = pattern->GetSizeInTiles();
    for (int j = area.y; j < area.height; j++)
        for (int i = area.x; i < area.width; i++)
            if (map->GetTileId(i, j) == id)
                map->SetTileId(i, j, pattern->GetTileId(i % sz.width, j % sz.height));
}