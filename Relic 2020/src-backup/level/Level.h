#ifndef _LEVEL_H
#define _LEVEL_H

#include "core/core.h"
#include "core/random.h"
#include "PauseState.h"

#include "LevelOptions.h"
#include "PlaceNameGenerator.h"

#include <vector>

class XmlElement;
class Map;
class DynamicMap;
class ContentLoader;
class RenderContext;
class GameTime;
class OverheadCamera;
class MapDescription;
class IEventListener;
class ITile;
class IGameObject;
class Character;
class Multiplayer;
class CharacterSync;
class CompactVectorFieldMap;
class BitMap2d;
class DebrisMap;

class Level
{
public:    
    
    enum { BASE_MAP = 0, LABEL_MAP = 4 };    
        
    Level(ContentLoader *loader, const char *path, int level, Multiplayer *multiplayer);
    ~Level();    
    
    core::Size            GetSize();

    const char           *GetPlaceName(const Vector3 &player) const;
    
    inline Map           *GetMap(int i = BASE_MAP)       { return mMaps[i]; }
    inline DynamicMap    *GetDynamicMap()                { return mDynamic; }
    inline int            GetMapCount()            const { return (int)mMaps.size(); }
    inline LevelOptions  *GetOptions()                   { return mOptions; }
    inline int            GetLevelNumber()         const { return mLevelNumber; }

    Character            *GetCharacter(int i);
    int                   GetFreeCharacterId()     const;

    static int            GetCharacterIdOwner(int id);

    Vector3               GetSpawnablePoint(int index);
    Vector3               GetStartPoint();

    Vector3               GetFieldVector(const Vector3 &at) const;

    CharacterSync        *GetCharacterSync()             { return mSync; }

    int                   GetNamedPointCount(const char *name);
    Vector3               GetNamedPoint(const char *name, int index);

    void                  AddCharacter(Character *ch);
    void                  RemoveCharacter(Character *ch);

    bool                  UncollideVisible();

    void                  Update (RenderContext &context, const GameTime &time, const PauseState &paused, IEventListener &listener);
    void                  Draw   (RenderContext &context, const GameTime &time, const PauseState &paused);
    
    void                  RunEvents(const Vector3 &position, IEventListener &listener);
    
    static ITile         *GetTileAt(Map *map, const Vector3 &position);
    static int            GetTileIdAt(Map *map, const Vector3 &position);
    static core::Point    GetTileLocationAt(Map *map, const Vector3 &position);

    ITile                *GetTileAt(int map, const Vector3 &position)               { return GetTileAt(mMaps[map], position); }
    int                   GetTileIdAt(int map, const Vector3 &position) const       { return GetTileIdAt(mMaps[map], position); }
    core::Point           GetTileLocationAt(int map, const Vector3 &position) const { return GetTileLocationAt(mMaps[map], position); }    

    int                   GetTileEventCountAt(const Vector3 &position);
    const char           *GetTileEventAt(const Vector3 &position, int index);
    const char           *HasTileEventAt(const char *name, const Vector3 &position);

    void                  DestroyCharacters(std::vector<Character *> &persistent);
    
    size_t                GetMovableCount() const;

    core::Rectangle       GetRenderRange() const;
    void                  Visit(const core::Rectangle &range, IEventListener &listener);
    char                 *GetVisitedData(int *size);
    bool                  SetVisitedData(const char *data, int size);

    static bool           AddFrameToDebrisMap(void *level, IGameObject *frame, const Vector3 &position, float angle, float scale);

private:

    struct Items
    {
        bool                     searched;
        std::vector<core::Point> items;

        inline Items() : searched(false) {}
    };

    std::vector<Map *>           mMaps;
    DynamicMap                  *mDynamic;
    LevelOptions                *mOptions;
    BitMap2d                    *mVisited;
    BitMap2d                    *mVisitedMap;
    DebrisMap                   *mDebrisMap;
    int                          mLevelNumber;
    std::map<std::string, Items> mNamedItems;

    Multiplayer                 *mMultiplayer;
    CharacterSync               *mSync;
    CompactVectorFieldMap       *mVectorField;
    PlaceNameGenerator           mPlaceNameGenerator;

    const std::vector<core::Point> &GetItems(const char *name);
    void                            FirstVisit(const core::Point &p, IEventListener &listener, const char *eventName);    
};

#endif // _LEVEL_H
