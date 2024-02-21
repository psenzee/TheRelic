#include "Sync.h"

#include "Character.h"
#include "Multiplayer.h"
#include "core/Dictionary.h"
#include "core/FixedBitMap.h"
#include "serialize/LuaSerialize.h"
#include "map/Map.h"
#include "map/DynamicMap.h"
#include "serialize/Pack.h"

const float CharacterSyncInfo::LOWEST_PRIORITY = 32000.0f;
const bool IGNORE_PRIORITY = false;

enum { MAX_CHARACTER_ID = 65534 };

CharacterSync::CharacterSync(Multiplayer *multiplayer, DynamicMap *map) : mMultiplayer(multiplayer), mDynamicMap(map), mPlayer(0), mDictionary(0)
{
    if (mMultiplayer)
    {
        mMultiplayer->AddMessageListener(MessageListener(CharacterSync::NotifyCallback, this));
    }
    mDictionary = Serialize::GetDictionary();
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
    if (character)
    {
        if (character->GetId() != sync.id)
        {
            // error!
            return;
        }
        if (!IGNORE_PRIORITY && mPlayer && sync.priority >= mPlayer->GetPosition().distance(character->GetPosition()))
            return;

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

void CharacterSync::Update()
{
    if (!mSyncInfo.empty())
    {
        FixedBitMap<MAX_CHARACTER_ID> found;
        CharacterSyncInfo::Comparator comparator;
        std::sort(mSyncInfo.begin(), mSyncInfo.end(), comparator); // we sort the best close to the top
        for (std::vector<CharacterSyncInfo>::const_iterator i = mSyncInfo.begin(), e = mSyncInfo.end(); i != e; ++i)
        {
            int id = (*i).id;
            if (id > MAX_CHARACTER_ID || id < 0)
                continue;
            if (id >= 0 && !found.get(id))
            {
                found.set(id, true);
                UpdateCharacter(*i);
            }
        }
        mSyncInfo.clear();
    }
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

void CharacterSync::PackCharacter(char **to, Character *c, bool immediate)
{
    CharacterSyncInfo sync;  
    sync.priority = CharacterSyncInfo::LOWEST_PRIORITY;
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
    if (map)
    {
        FixedBitMap<MAX_CHARACTER_ID> sent;
        MarkList &changed = mDynamicMap->GetChanged();
        MovableRegistry &registry = mDynamicMap->GetRegistry();
        int id = changed.GetNext(-1);
        while (id != -1)
        {
            Character *c = static_cast<Character *>(registry.GetAt(id));
            sent.set(c->GetId(), true);
            PackCharacter(&pd, c, true);
            id = changed.GetNext(id);
            count++;
        }
        changed.Clear();
        core::Rectangle r(map->GetRenderRange());
        r.width--; r.height--;
        DynamicMapIterator iterator(mDynamicMap, r);
        while (iterator.HasNext())
        {
            Character *c = static_cast<Character *>(iterator.GetNext());
            if (IsSendable(c) && !sent.get(c->GetId())) // don't serialize what's already been serialized
            {
                PackCharacter(&pd, c);
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
            if (IsSendable(c))
            {
                PackCharacter(&pd, c);
                count++;
            }
            id = registry.GetNext(id);            
        }
    }
    static int frame = 0;
    if (frame % 30 == 0)
        printf("Sending data for %d characters\n", count);
    frame++;
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
    PackUInt16(&pb, CHARACTER_UPDATE);
    if (PackCharacters(&pb, map))
        mMultiplayer->SendGameData(buffer, static_cast<int>(pb - buffer));
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
    case LUA_MESSAGE:      return ReceiveLuaMessage      (message, length);
    default:
        // unrecognized message
        break;
    }
    return false;
}

bool CharacterSync::SendLuaMessage(const char *message, int length)
{
    if (!mMultiplayer->IsActiveConnection())
        return false;
    char buffer[MAX_MESSAGE_SIZE];
    char *pb = buffer;
    PackUInt16(&pb, LUA_MESSAGE);
    memcpy(pb, message, length);
    mMultiplayer->SendGameData(buffer, length + sizeof(unsigned short));
    return true;
}

const std::string &CharacterSync::NextLuaMessage() const
{
    static const std::string empty;
    return mLuaMessages.empty() ? empty : mLuaMessages.front();
}

void CharacterSync::RemoveLuaMessage()
{
    if (!mLuaMessages.empty())
        mLuaMessages.pop_front();
}

bool CharacterSync::HasLuaMessage() const
{
    return !mLuaMessages.empty();
}

void CharacterSync::ClearLuaMessages()
{
    mLuaMessages.clear();
}

bool CharacterSync::ReceiveLuaMessage(const char *message, int length)
{
    mLuaMessages.push_back(""); // do it this way to avoid allocating and copying twice
    mLuaMessages.back().assign(message, length);
    return false;
}

bool CharacterSync::ReceiveCharacterUpdates(const char *message, int length)
{
    unsigned short count = UnpackUInt16(&message);
    if (count == 0)
        return true;
    for (int i = 0; i < count; i++)
    {
        CharacterSyncInfo sync;
        UnpackCharacterSyncCompact(&message, sync);
        if (IsReceivable(sync))
            AddSyncInfo(sync);
    }
    return true;
}
