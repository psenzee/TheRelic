#include "MapDescription.h"

Level::Level(GameState *state, const char *path, const MapDescription &level) : state(state), level(level), locked(false), exitstate(EXIT_VALID)
{
    options = new LevelFile(path, "[random]");
    MapGenerator mg(state->services);
    map = mg.GenerateMap(level, options->configuration, "[random]");
}

Map *MapGenerator::GenerateMap(const MapDescription &level, XmlElement *levelConfig, const char *mapTypeName)
{
    MapFile   options(levelConfig, mapTypeName);
    ITileMap *map     = 0;
    MazeGenerator mg;
    core::Random r(level.seed);
    map = mg.CreateMapWithWallTypes(mapsz, r.integer(), false);
    core::Size sz = map->GetSizeInTiles();
    SetCheckeredFloor(map); // patterned
    core::Point start = GetSpiral8MapStart(map, spiralLevel);
    //SetTrees(map, r, start);
    
    printf("Map Size %u, %u\n", sz.width, sz.height);
/*    
    if (sz.width > 16 && sz.height > 16)
        MakeParkType1(map, core::Rectangle(3, 3, 10, 10), r, start, false, 0);
*/    
    SetSpecialTilesForSpiral8Map(map, spiralLevel);
    SetRandomPickups(map, r);
    printf("Creating map level %d, tileset '%s', size (%d, %d)\n", level.level, level.tileset.c_str(), sz.width, sz.height);   
    options.tiles = level.tileset.c_str();
    ((CoreTileMap *)map)->Reverse();
    ITileMapModel *model = 
        new SimpleTileMapModel(DeviceTileSetLoader::GetTileSet(options.tiles, Services), map);    
    return new Map(Services, model, &options);
}