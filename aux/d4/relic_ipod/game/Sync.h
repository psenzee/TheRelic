#ifndef _CHARACTERSYNC_H
#define _CHARACTERSYNC_H

class Movable;
class Character;
class DynamicMap;
class Dictionary;
class Map;

#include "core/core.h"
#include "Multiplayer.h"

#include <float.h>
#include <vector>
#include <deque>
#include <algorithm>

class CharacterSyncInfo
{
public:

    static const float LOWEST_PRIORITY; // this is the highest number allowed by priority

    int        id;
    float      priority; // this will be the distance from the player to the character, the lower, the higher the priority
    int        player,
               time,
               animation,
               animationFrame;
    bool       visible,
               movable,
               isPlayer,
               collidable,
               fixed;
    Vector3    position,
               velocity;
    float      orientation;
               
    inline CharacterSyncInfo() : 
        id(-1), priority(LOWEST_PRIORITY), player(-1), time(0), animation(0), animationFrame(0),
        visible(false), movable(true), isPlayer(false), collidable(false), fixed(false), orientation(0.f) {}
    
    struct Comparator
    {
        inline bool operator()(const CharacterSyncInfo &a, const CharacterSyncInfo &b) const
        {
            if (a.priority < b.priority) return true;
            if (a.priority > b.priority) return false;                
            return a.time >= b.time; // else if equal .. then latest sorts higher
        }
    };
};

class CharacterSync
{
public:
    
    CharacterSync(Multiplayer *multiplayer, DynamicMap *map);
    
    Character         *GetCharacter(int index);
    void               SetCharacter(int index, Character *c);
    
    void               SetPlayer(Character *player);
    void               SetCharacterId(Character *character, int id);
    
    void               Update();
    bool               SendCharacterUpdates(Map *map);

    bool               SendLuaMessage(const char *message, int length);
    const std::string &NextLuaMessage() const;
    void               RemoveLuaMessage();
    bool               HasLuaMessage() const;
    void               ClearLuaMessages();

protected:

    void       AddSyncInfo(const CharacterSyncInfo &syncInfo);
    void       AddSyncInfo(const std::vector<CharacterSyncInfo> &syncInfo);

    bool       ReceiveCharacterUpdates(const char *message, int length);
    bool       ReceiveLuaMessage(const char *message, int length);
    bool       ReceiveMessage(const char *message, int length);

    typedef bool (*ServerReceiveCallback)(void *userdata, int player, const char *message, int length);
    typedef bool (*ClientReceiveCallback)(void *userdata, const char *message, int length);

private:

    enum { CHARACTER_UPDATE = 23, LUA_MESSAGE };

    bool       IsSendable(const Movable *movable) const;
    bool       IsSendable(const Character *character) const;
    bool       IsReceivable(const CharacterSyncInfo &sync) const;

    void       UpdateCharacter(const CharacterSyncInfo &sync);
    void       UpdateCharacterSync(int id, CharacterSyncInfo &sync);

    void       PackCharacter(char **to, Character *c, bool immediate = false);
    int        PackCharacters(char **to, Map *map);

    static bool NotifyCallback(void *userdata, const char *message, int length);

    enum { MAX_CHARACTERS = 32768 };
    
    std::vector<CharacterSyncInfo>  mSyncInfo;
    
    Multiplayer                    *mMultiplayer;
    DynamicMap                     *mDynamicMap;
    Character                      *mPlayer;
    const Dictionary               *mDictionary;

    std::deque<std::string>         mLuaMessages;
};

#endif // _CHARACTERSYNC_H