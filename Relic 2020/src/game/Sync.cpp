#include "Sync.h"

#include "Character.h"
#include "Multiplayer.h"
#include "core/Dictionary.h"
#include "core/FixedBitMap.h"
#include "serialize/LuaSerialize.h"
#include "map/Map.h"
#include "map/ITileMap.h"
#include "map/DynamicMap.h"
#include "serialize/Pack.h"
#include "net/NetGame.h"

const float CharacterSyncInfo::LOWEST_PRIORITY = 32000.0f;
const bool IGNORE_PRIORITY = false;

bool USE_MAP_UPDATER = true;

enum { MAX_CHARACTER_ID = 65534 };

bool CharacterSync::OnReceiveCharacterUpdates(const char *message, int length, void *user)
{
    if (!user)
        return false;
    CharacterSync *sync = static_cast<CharacterSync *>(user);
    return sync->ReceiveCharacterUpdates(message, length);
}

CharacterSync::CharacterSync(Multiplayer *multiplayer, DynamicMap *map) : mMultiplayer(multiplayer), mDynamicMap(map), mPlayer(0), mDictionary(0)
{
    if (mMultiplayer)
    {
        /*
        MessageListener listener(CharacterSync::NotifyCallback, this);
        mMultiplayer->AddMessageListener(listener);
        */
        mMultiplayer->SetOnReceiveCharacterUpdates(OnReceiveCharacterUpdates, this);
    }
    memset(mPlayerUpdated, 0, sizeof(mPlayerUpdated));
    mDictionary = Serialize::GetDictionary();
}

CharacterSync::~CharacterSync()
{
    if (mMultiplayer)
    {
        mMultiplayer->SetOnReceiveCharacterUpdates(0, 0);
        mMultiplayer->ClearListeners();
    }
}
    
Character *CharacterSync::GetCharacter(int index)
{
    return static_cast<Character *>(index >= MAX_CHARACTERS ? 0 : mDynamicMap->GetMovableById(index));
}

void CharacterSync::SetPlayer(Character *player)
{
    mPlayer = player;
}

void CharacterSync::SetCharacterId(Character *character, int id)
{
    if (character->GetId() != id)
        character->SetId(id);
}

void CharacterSync::AddSyncInfo(const CharacterSyncInfo &syncInfo)
{
    mSyncInfo.push_back(syncInfo);
}

void CharacterSync::AddSyncInfo(const std::vector<CharacterSyncInfo> &syncInfo)
{
    std::copy(syncInfo.begin(), syncInfo.end(), std::back_inserter(mSyncInfo));
}

void CharacterSync::UpdateCharacter(const CharacterSyncInfo &sync)
{
    Character *character = GetCharacter(sync.id);
    bool isServer = mMultiplayer->GetConnectionManager()->IsServer();
    if (character)
    {
        if (character->GetId() != sync.id)
        {
            // error!
            return;
        }
        float priority = sync.priority;
//        if (!isServer)
//            priority -= 100.0f; // give preference to the server
        if (!IGNORE_PRIORITY && mPlayer && priority >= mPlayer->GetPosition().distance(character->GetPosition()))
        {            
            character->SetPositionRaw((sync.position + character->GetPosition()) * 0.5f);
            return;
        }

        const char *text = mDictionary->GetText(sync.animation);
        if (text)
        {
            character->SetAnimation(text);
        }
        Animation *anim = character->GetAnimation();
        anim->SetFrame(sync.animationFrame);
        if (!sync.fixed)
        {
            character->SetPositionRaw(sync.position);
            character->SetVelocity(sync.velocity);
            character->SetOrientationAngle(sync.orientation);
        }
        character->SetVisible(sync.visible);
        character->SetMovable(sync.movable);
        character->SetFixed(sync.fixed);
        character->SetCollidable(sync.collidable);
    }
}

void CharacterSync::UpdateCharacterSync(int id, CharacterSyncInfo &sync)
{
    Character *character = GetCharacter(id);
    if (character)
    {
        sync.visible     = character->IsVisible();
        sync.movable     = character->IsMovable();
        sync.fixed       = character->IsFixed();
        sync.collidable  = character->IsCollidable();
        if (!sync.fixed)
        {
            sync.position    = character->GetPosition();
            sync.velocity    = character->GetVelocity();
            sync.orientation = character->GetOrientationAngle();
        }
    }
}

static void PackCharacterSyncCompact(char **to, const CharacterSyncInfo &sync)
{
    Pack     (to, static_cast<unsigned short>(sync.id));
    Pack     (to, FloatToS16(sync.priority, 1.0f));
    PackBools(to, sync.collidable, sync.isPlayer, sync.visible, sync.movable, sync.fixed);
    Pack     (to, static_cast<unsigned char>(sync.animation));
    Pack     (to, static_cast<unsigned char>(sync.animationFrame));
    if (!sync.fixed)
    {
        PackPositionXY(to, sync.position);
        PackVelocity  (to, sync.velocity);
        PackAngleS16  (to, sync.orientation);
    }
}

static void UnpackCharacterSyncCompact(const char **from, CharacterSyncInfo &sync)
{
    sync.id             = UnpackInt16(from);
    sync.priority       = S16ToFloat(UnpackUInt16(from), 1.0f);
    UnpackBools(from, &sync.collidable, &sync.isPlayer, &sync.visible, &sync.movable, &sync.fixed);
    sync.animation      = UnpackUChar(from);
    sync.animationFrame = UnpackUChar(from);
    if (!sync.fixed)
    {
        sync.position    = UnpackPositionXY(from);
        sync.velocity    = UnpackVelocity(from);
        sync.orientation = UnpackAngleS16(from);
    }
}

extern "C" const char *GetPlatformDeviceName();

static FILE *DEBUG_FILE = 0;

extern "C" FILE *GetDebugFile()
{
    if (!DEBUG_FILE)
    {
        char filename[256];
        snprintf(filename, sizeof(filename) - 1, "%s.txt", GetPlatformDeviceName());
        DEBUG_FILE = fopen(filename, "w");
    }
    return DEBUG_FILE;
}

void CharacterSync::Update()
{
    if ((!USE_MAP_UPDATER && !mSyncInfo.empty()) || (USE_MAP_UPDATER && !mReceived.empty()))
    {
        for (std::map<int, CharacterSyncInfo>::const_iterator i = mReceived.begin(), e = mReceived.end(); i != e; ++i)
            AddSyncInfo((*i).second);
        mReceived.clear();

        FixedBitMap<MAX_CHARACTER_ID> found;
        CharacterSyncInfo::Comparator comparator;
        std::sort(mSyncInfo.begin(), mSyncInfo.end(), comparator); // we sort the best close to the top
#ifdef WIN32
fprintf(GetDebugFile(), "%c", mMultiplayer->GetConnectionManager()->IsServer() ? 'S' : 'C');
#endif
        for (std::vector<CharacterSyncInfo>::const_iterator i = mSyncInfo.begin(), e = mSyncInfo.end(); i != e; ++i)
        {
            int id = (*i).id;
            if (id > MAX_CHARACTER_ID || /*id < 0*/ id < 5)
                continue;
            if (id >= 0 && !found.get(id))
            {
#ifdef WIN32
fprintf(GetDebugFile(), " >%d", id);
#endif
                found.set(id, true);
                UpdateCharacter(*i);
            }
        }
        mSyncInfo.clear();
    }
#ifdef WIN32
fprintf(GetDebugFile(), "\n");
fflush(GetDebugFile());
#endif
}

bool CharacterSync::IsSendable(const Movable *movable) const 
{
    return IsSendable(static_cast<const Character *>(movable)); // upcast
}

bool CharacterSync::IsSendable(const Character *character) const
{
    // return true if this should be sent..
    // so:    
    // if it's within a certain radius, send
    // other rules
    return true;
}

bool CharacterSync::IsReceivable(const CharacterSyncInfo &sync) const
{
    // return true if this should be updated..
    // so:
    // if it's from self, ignore
    // other rules
    return true;
}

int CharacterSync::GetPlayerId() const
{
//  printf("***\x07trying to CharacterSync::GetPlayerId() when mPlayer is null!***\n");
    return mPlayer ? mPlayer->GetId() : -1;
}

void CharacterSync::PackCharacter(char **to, Character *c, bool immediate)
{
    CharacterSyncInfo sync;  
    sync.priority = CharacterSyncInfo::LOWEST_PRIORITY;
if (!mPlayer)
{
//    printf("***\x07 mPlayer not set!***\n");
}
    if (!IGNORE_PRIORITY && mPlayer)
    {
        if (immediate)
            sync.priority = 0.f;
        else
        {
            float distance = mPlayer->GetPosition().distance(c->GetPosition());
            if (distance < sync.priority)
                sync.priority = distance;
        }
    }
    sync.animation = mDictionary->GetId(c->GetAnimationName());
    Animation *anim = c->GetAnimation();
    sync.animationFrame = anim->GetFrameNumber();
    sync.id = c->GetId();
    sync.position = c->GetPosition();
    sync.movable = c->IsMovable();
    sync.fixed = c->IsFixed();
    sync.visible = c->IsVisible();
    sync.collidable = c->IsCollidable();
    sync.orientation = c->GetOrientationAngle();
    sync.velocity = c->GetVelocity();
    PackCharacterSyncCompact(to, sync);
}

int CharacterSync::PackCharacters(char **to, Map *map)
{
    int count = 0;
    enum { BUFFER_SIZE = 65536 };
    char data[BUFFER_SIZE], *pd = data;
#ifdef WIN32
fprintf(GetDebugFile(), "%c", mMultiplayer->GetConnectionManager()->IsServer() ? 'S' : 'C');
#endif
    if (map)
    {
        FixedBitMap<MAX_CHARACTER_ID> sent;
        MarkList &changed = mDynamicMap->GetChanged();
        MovableRegistry &registry = mDynamicMap->GetRegistry();
        int id = changed.GetNext(-1);
        while (id != -1 && id > 4)
        {
            Character *c = static_cast<Character *>(registry.GetAt(id));
            if (Character::IsCharacterValid(c))
            {
                sent.set(c->GetId(), true);
                PackCharacter(&pd, c, true);
#ifdef WIN32
fprintf(GetDebugFile(), " <%d", c->GetId());
#endif
                id = changed.GetNext(id);
                count++;
            }
        }
        changed.Clear();
        core::Rectangle r(map->GetRenderRange());
        //core::Size sz(map->GetMapModel()->GetMapSizeInTiles());
        //core::Rectangle r(0, 0, sz.width, sz.height);
//      r.width--; r.height--;
        DynamicMapIterator iterator(mDynamicMap, r);
        while (iterator.HasNext())
        {
            Character *c = static_cast<Character *>(iterator.GetNext());
            if (IsSendable(c) && !sent.get(c->GetId()) && c->GetId() > 4) // don't serialize what's already been serialized
            {
                PackCharacter(&pd, c);
#ifdef WIN32
fprintf(GetDebugFile(), " <%d", c->GetId());
#endif
                count++;
            }
        }
    }
    else
    {
        MovableRegistry &registry = mDynamicMap->GetRegistry();
        int id = registry.GetNext(-1);
        while (id != -1)
        {
            Character *c = static_cast<Character *>(registry.GetAt(id));
            if (IsSendable(c) && c->GetId() > 4)
            {
                PackCharacter(&pd, c);
#ifdef WIN32
fprintf(GetDebugFile(), " <%d", c->GetId());
#endif
                count++;
            }
            id = registry.GetNext(id);            
        }
    }
    if (count)
    {
#ifdef WIN32
fprintf(GetDebugFile(), "\n");
fflush(GetDebugFile());
#endif
    }
    static int frame = 0;
//  if (frame % 30 == 0)
//      printf("Sending data for %d characters\n", count);
    frame++;
    int selfId = GetPlayerId();
    if (!(count > 0 && selfId >= 0 && selfId < MAX_PLAYERS))
        count = 0;
    Pack(to, static_cast<unsigned short>(selfId));
    Pack(to, static_cast<unsigned short>(count));
    if (count)
    {
        memcpy(*to, data, pd - data);
        (*to) += pd - data;
    }
    return count;
}

bool CharacterSync::SendCharacterUpdates(Map *map)
{
    if (!mMultiplayer->IsActiveConnection())
        return false;
    char buffer[MAX_MESSAGE_SIZE];
    memset(buffer, 0, sizeof(buffer));
    char *pb = buffer;
    PackUInt16(&pb, (unsigned short)CHARACTER_UPDATE);
    if (PackCharacters(&pb, map))
        mMultiplayer->SendGameData(buffer, static_cast<int>(pb - buffer), false, false);
    return true;
}

bool CharacterSync::NotifyCallback(void *userdata, const char *message, int length)
{
    CharacterSync *sync = static_cast<CharacterSync *>(userdata);
    return sync ? sync->ReceiveMessage(message, length) : false;
}

bool CharacterSync::ReceiveMessage(const char *message, int length)
{
    if (!mMultiplayer->IsActiveConnection())
        return false;
    unsigned short type = UnpackUInt16(&message);
    length -= sizeof(type);
    switch (type)
    {
    case CHARACTER_UPDATE: return ReceiveCharacterUpdates(message, length);
    case LUA_MESSAGE:      return ReceiveMessage(LUA_MESSAGE_QUEUE, message, length);
    case RAW_DATA:         return ReceiveMessage(DATA_MESSAGE_QUEUE, message, length);
    default:
        // unrecognized message
        break;
    }
    return false;
}

bool CharacterSync::SendTypedData(unsigned short type, const char *message, int length, bool reliable, bool sequenced)
{
    if (!mMultiplayer->IsActiveConnection())
        return false;
    char buffer[MAX_MESSAGE_SIZE];
    char *pb = buffer;
    PackUInt16(&pb, type);
    memcpy(pb, message, length);
    mMultiplayer->SendGameData(buffer, length + sizeof(unsigned short), reliable, sequenced);
    return true;
}

bool CharacterSync::SendDataMessage(const char *message, int length, bool reliable, bool sequenced)
{
    return SendTypedData(static_cast<unsigned short>(RAW_DATA), message, length, reliable, sequenced);
}

bool CharacterSync::SendLuaMessage(const char *message, int length)
{
    return SendTypedData(static_cast<unsigned short>(LUA_MESSAGE), message, length, /*true, true*/false, false);
}

const std::string &CharacterSync::NextMessage(Queue queue) const
{
    static const std::string empty;
    return mMessageQueues[queue].empty() ? empty : mMessageQueues[queue].front();
}

void CharacterSync::RemoveMessage(Queue queue)
{
    if (!mMessageQueues[queue].empty())
        mMessageQueues[queue].pop_front();
}

bool CharacterSync::HasMessage(Queue queue) const
{
    return !mMessageQueues[queue].empty();
}

void CharacterSync::ClearMessages(Queue queue)
{
    mMessageQueues[queue].clear();
}

bool CharacterSync::ReceiveMessage(Queue queue, const char *message, int length)
{
    mMessageQueues[queue].push_back(""); // do it this way to avoid allocating and copying twice
    mMessageQueues[queue].back().assign(message, length);
    return false;
}

bool CharacterSync::ReceiveCharacterUpdates(const char *message, int length)
{
    unsigned short playerId = UnpackUInt16(&message);
    if (playerId >= MAX_PLAYERS || playerId == GetPlayerId())
    {
        printf("bad player id, ignore character update\n");
        return false; // bad player id, ignore message
    }
//  if (mPlayerUpdated[playerId])
//      return true;
    mPlayerUpdated[playerId] = true;
    unsigned short count = UnpackUInt16(&message);
    if (count == 0)
        return true;
    // $TODO, we need to do something different for the server case
    if (!USE_MAP_UPDATER)
        mSyncInfo.clear();
    for (int i = 0; i < count; i++)
    {
        CharacterSyncInfo sync;
        UnpackCharacterSyncCompact(&message, sync);
        if (IsReceivable(sync))
        {
            if (USE_MAP_UPDATER)
                mReceived[(playerId << 16) | sync.id] = sync;
            else
                AddSyncInfo(sync);
        }
    }
    return true;
}
