#ifndef _LEVELMANAGER_H
#define _LEVELMANAGER_H

#include "core/core.h"
#include "core/random.h"
#include "core/core_assert.h"
#include "Level.h"

class XmlElement;
class Map;
class GameTime;
class OverheadCamera;
class RenderContext;
class IEventListener;
class Multiplayer;
class LuaThread;

class LevelManager
{       
public:

    LevelManager(Multiplayer *multiplayer);
    
    inline void           SetLuaThread(LuaThread *luaThread) { mLuaThread = luaThread; }

    inline bool           IsLevelLoaded() const  { return mLevel != 0; }
    inline Level         *GetLevel()             { core_assert(mLevel);      return mLevel; }
    inline int32_t        GetLevelNumber() const { core_assert(mIndex >= 0); return mIndex; }
        
    void                  RunEvents(const Vector3 &position, IEventListener &listener);
        
    void                  LoadLevel(int32_t index);
    void                  ReloadLevel();
    void                  UnloadLevel();
        
    void                  Draw   (RenderContext &context, const GameTime &time, const PauseState &paused);
    void                  Update (RenderContext &context, const GameTime &time, const PauseState &paused, IEventListener &listener);
    
    // here we have calls that forward to the current Level if possible
    inline Map           *GetMap(int32_t map = Level::BASE_MAP)                     { return !mLevel ? 0 : mLevel->GetMap(map); }
    inline DynamicMap    *GetDynamicMap()                                           { return !mLevel ? 0 : mLevel->GetDynamicMap(); }
    inline ITile         *GetTileAt(int32_t map, const Vector3 &position)           { return !mLevel ? 0 : mLevel->GetTileAt(map, position); }
    inline ITile         *GetTileAt(const Vector3 &position)                        { return GetTileAt(Level::BASE_MAP, position); }
    inline int32_t        GetTileIdAt(int32_t map, const Vector3 &position) const   { return !mLevel ? 0 : mLevel->GetTileIdAt(map, position); }
    inline int32_t        GetTileIdAt(const Vector3 &position) const                { return GetTileIdAt(Level::BASE_MAP, position); }
    inline core::Point    GetTileLocationAt(int32_t map, const Vector3 &position) const { return !mLevel ? core::Point() : GetTileLocationAt(map, position); }
    inline core::Point    GetTileLocationAt(const Vector3 &position) const          { return GetTileLocationAt(Level::BASE_MAP, position); }
    inline core::Size     GetSize() const                                           { return !mLevel ? core::Size() : mLevel->GetSize(); }
    inline const char    *GetPlaceName(const Vector3 &position) const               { return !mLevel ? "" : mLevel->GetPlaceName(position); }
    inline CharacterSync *GetCharacterSync()                                        { return !mLevel ? 0 : mLevel->GetCharacterSync(); }
    inline void           RemoveCharacter(Character *c)                             { if (mLevel) mLevel->RemoveCharacter(c); }
    inline size_t         GetMovableCount()                                   const { return !mLevel ? 0 : mLevel->GetMovableCount(); }
        
private:

    int32_t                   mIndex;
    Level                    *mLevel;
    core::Random              mRandom;
    Multiplayer              *mMultiplayer;
    LuaThread                *mLuaThread;
    CharacterSync            *mSync;
    std::vector<Character *>  mPersistent;
        
    Level          *Load(int32_t index);
};

#endif // _LEVELMANAGER_H
