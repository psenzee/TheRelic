/*
 *  Level.h
 *  GLGravity
 *
 *  Created by Paul Senzee on 12/30/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _LEVEL_H
#define _LEVEL_H

#include "core/core.h"
#include "core/random.h"

#include "LevelOptions.h"

class XmlElement;
class Map;
class DynamicMap;
class ContentLoader;
class RenderContext;
class GameTime;
class OverheadCamera;
class MapDescription;
class IEventListener;
class ITileInstance;
class Character;
class Multiplayer;
class CharacterSync;

#include <vector>

class Level
{
    enum { BASE_MAP = 0 };

public:    
        
    Level(ContentLoader *loader, const char *path, int level, Multiplayer *multiplayer);
    ~Level();    
    
    core::Size            GetSize();
    
    inline Map           *GetMap(int i = BASE_MAP)       { return maps[i]; }
    inline DynamicMap    *GetDynamicMap()                { return dynamic; }
    inline int            GetMapCount()            const { return (int)maps.size(); }
    inline LevelOptions  *GetOptions()                   { return options; }
    inline int            GetLevelNumber()         const { return levelNumber; }

    Character            *GetCharacter(int i);
    int                   GetFreeCharacterId()     const;

    static int            GetCharacterIdOwner(int id);

    Vector3               GetSpawnablePoint(int index);
    Vector3               GetStartPoint();

    CharacterSync        *GetCharacterSync()             { return mSync; }

    int                   GetNamedPointCount(const char *name);
    Vector3               GetNamedPoint(const char *name, int index);

    void                  AddCharacter(Character *ch);
    void                  RemoveCharacter(Character *ch);

    bool                  UncollideVisible();

    void                  Update(RenderContext &context, const GameTime &time, bool paused);
    void                  Draw(RenderContext &context, const GameTime &time);
    
    void                  RunEvents(const Vector3 &position, IEventListener &listener);
    
    static ITileInstance *GetTileAt(Map *map, const Vector3 &position);
    ITileInstance        *GetTileAt(int index, const Vector3 &position) { return GetTileAt(maps[index], position); }

    int                   GetTileEventCountAt(const Vector3 &position);
    const char           *GetTileEventAt(const Vector3 &position, int index);
    const char           *HasTileEventAt(const char *name, const Vector3 &position);

private:

    struct Items
    {
        bool                     searched;
        std::vector<core::Point> items;

        inline Items() : searched(false) {}
    };

    std::vector<Map *>           maps;
    DynamicMap                  *dynamic;
    LevelOptions                *options;
    int                          levelNumber;
    std::map<std::string, Items> namedItems;

    Multiplayer                 *mMultiplayer;
    CharacterSync               *mSync;

    const std::vector<core::Point> &GetItems(const char *name);
};

#endif // _LEVEL_H