#include "Character.h"

#include "core/core.h"
#include "core/random.h"
#include "core/strs.h"
#include "core/Dictionary.h"

#include "serialize/LuaSerialize.h"
#include "serialize/Pack.h"

#include "animation/Animation.h"
#include "map/Map.h"
#include "map/ITileMap.h"

#include "math/MathUtil.h"
#include "render/RenderContext.h"
#include "render/Drawable.h"
#include "render/CreateGeometry.h"

#include "gamecore/ICollidable.h"

#include "luautil/LuaInterpreter.h"
#include "luautil/LuaThread.h"
#include "luautil/LuaCall.h"

#include "Selector.h"
#include "Meter.h"
#include "IBehavior.h"

const char *IDLE_ANIMATION = "Idle";

#include <algorithm>
// validity hash BEGIN
#include <google/dense_hash_set>

class Character;

float GetCharacterWeight(Movable *movable)
{
    return static_cast<Character *>(movable)->GetWeight();
}

void SetCharacterWeight(Movable *movable, float weight)
{
    static_cast<Character *>(movable)->SetWeight(weight);
}

inline unsigned char GetNBitUIntFromFloatAngle(float radians, int n)
{
    const float INV_2PI = (1.f / math::TWOPIf) * ((1 << n) - 1.f);
    return (unsigned char)(MathUtil::NormalizeAngle2Pi(radians) * INV_2PI);
}

inline float GetFloatFromNBitUIntAngle(unsigned char angle, int n)
{
    static const float INV_2PI = (1.f / ((1 << n) - 1.f)) * math::TWOPIf;
    return angle * INV_2PI;
}

int GetCharacterIdFromPositionPacket(const CharacterPositionPacket &packet)
{
    short id = 0;
    memcpy(&id, &(packet.data[0]), sizeof(short));
    return id;
}

CharacterPositionPacket GetCharacterPositionPacket(Character *ch)
{
    short         id     = (short)ch->GetId();
    unsigned char animid = (unsigned char)(Serialize::GetDictionary()->GetId(ch->GetAnimationName()) & 0x3f);
    unsigned char frame  = (unsigned char)ch->GetAnimation()->GetFrameNumber();
    unsigned char angle  = GetNBitUIntFromFloatAngle(ch->GetOrientationAngle(), 8);

    unsigned char flags1 = 0;
    unsigned char flags2 = 0;
    
    Vector3 position(ch->GetPosition());
    short x = (int)position.x;
    short y = (int)position.y;
    
    CharacterPositionPacket packet;
    memset(&packet, 0, sizeof(packet));
    memcpy(&(packet.data[0]), &id, sizeof(short));
    packet.data[2] = animid;
    packet.data[3] |= (frame << 2);
    packet.data[3] |= flags1;
    packet.data[4] |= angle;
    memcpy(&(packet.data[5]), &x, sizeof(short));
    memcpy(&(packet.data[7]), &y, sizeof(short));

    return packet;
}

void SetCharacterPositionPacket(Character *ch, const CharacterPositionPacket &packet)
{
    unsigned char animid =  packet.data[2];
    unsigned char frame  = (packet.data[3] >> 2) & 0x3f;
    unsigned char flags1 =  packet.data[3] & 0x03;
    unsigned char angle  = packet.data[4];
    
    short id = 0, x = 0, y = 0;
    
    memcpy(&id, &(packet.data[0]), sizeof(short));
    memcpy(&x,  &(packet.data[5]), sizeof(short));
    memcpy(&y,  &(packet.data[7]), sizeof(short));

 // ch->GetId() == id ; // check this!
    ch->SetPosition(Vector3((float)x, (float)y, 0.f));
    ch->SetAnimation(Serialize::GetDictionary()->GetText(animid));
    ch->GetAnimation()->SetFrame(frame);
    ch->SetOrientationAngle(GetFloatFromNBitUIntAngle(angle, 8));
}

template <typename T>
struct aligned_4_pointer_hash
{
    inline size_t operator()(T v) const { return (size_t)(v) >> 2; }
};

typedef google::dense_hash_set<const Character *, aligned_4_pointer_hash<const Character *> > CharacterValidHash;

static CharacterValidHash gCharacterValidHash;
static bool               gCharacterValidHashInitialized = false;

static void InitCharacterHash()
{
    if (!gCharacterValidHashInitialized)
    {
        gCharacterValidHash.set_empty_key(0); 
        gCharacterValidHash.set_deleted_key((Character *)(~0u));
        gCharacterValidHashInitialized = true;
    }
}

void Character::SetCharacterValid(const Character *c, bool valid)
{ 
    InitCharacterHash();
    if (valid) gCharacterValidHash.insert(c);
    else       gCharacterValidHash.erase(c);
}

bool Character::IsCharacterValid(const Character *c)
{ 
    InitCharacterHash();
    CharacterValidHash::const_iterator i = gCharacterValidHash.find(c);
    bool r = i != gCharacterValidHash.end();
    return r;
}
// validity hash END

int Character::sCharacterCount = 0;

Character::Character(const Vector3 &position, XmlReadContext *renderables, const char *type, const char *luaFunctionName) 
  : Movable(position), mAnimation(0), mSelector(0), mMeter(0), mMap(0), mVisible(true), mCollidable(true), mMovable(true), mFixed(false),
    mLuaThread(0), mLuaFunctionName(luaFunctionName), mAngle(0.f), mScale(2.0f), mRenderables(renderables), mFilteredAngle(0.f, 0.1f),
    mFilterAngle(true), mAddFrameToDebrisMapCallback(0), mAddFrameToDebrisMapUser(0), mFilterPosition(false),
    mLuaReference(0), mLuaTable(LuaTable::Null()), mRotation(0.f), mAlpha(1.0f), mLean(0.f, 0.f), mWeight(1.f), mType(type), mPersistent(false), mStatic(false), mSeed(0), mUpdateFrequency(17)
{
    memset(mEffects, 0, sizeof(mEffects));
    mLuaThread = LuaInterpreter::GetInstance()->CreateThread(this);
    Reset(position);
    SetAnimation(IDLE_ANIMATION);
    SetCharacterValid(this, true);
    sCharacterCount++;
    printf("Cs:%d ", sCharacterCount);
}

extern "C" FILE *GetDebugFile();

Character::~Character()
{
#ifdef WIN32
fprintf(GetDebugFile(), "Destroying id %d\n", GetId());
fflush(GetDebugFile());
#endif
    SetCharacterValid(this, false);
    ClearSignalHandlers();
    for (std::vector<IBehavior *>::iterator i = mBehaviors.begin(), e = mBehaviors.begin(); i != e; ++i)
        delete (*i);
    mBehaviors.clear();
    LuaInterpreter::GetInstance()->DestroyThread(mLuaThread);
    for (std::map<String, Animation *>::iterator i = mAnimations.begin(), e = mAnimations.end(); i != e; ++i)
        delete (*i).second;
    mAnimations.clear();
    mAnimation = 0; // this is deleted in the loop above
    mLuaReference = 0; // this is unref'd in DestroyThread(..) above
    // we're going to assume ownership if these haven't been destroyed otherwise
    for (int i = 0; i < MAX_EFFECTS; i++)
    {
        if (mEffects[i])
        {
            delete mEffects[i];
            mEffects[i] = 0;
        }
    }
    if (mMeter)
    {
        delete mMeter;
        mMeter = 0;
    }
    if (mSelector)
    {
        delete mSelector;
        mSelector = 0;
    }
    sCharacterCount--;
    printf("Cs:%d ", sCharacterCount);
}

const char *Character::GetType() const
{
    return mType.c_str();    
}

extern void DestroyCharacterDeferred(Character *character);

bool Character::AnimationTagCallback(void *user, const char *data)
{
    Character *character = static_cast<Character *>(user);

    char       buffer[1024];
    if (character && data)
    {
        if (GetTagStringArgument(data, "GoTo", buffer))
        {
            character->SetAnimation(buffer);
            return true;
        }
        else if (GetTagStringArgument(data, "Script", buffer))
        {
            LuaValue arg[1] = { LuaValue::UserData(character->GetLuaReference()) };
            LuaCall(character->mLuaThread->GetLuaState(), buffer, arg, 1);
            return true;
        }
        else if (strcmp(data, "AddFrameToDebrisMap") == 0)
        {
            if (character->mAddFrameToDebrisMapCallback && character->mAddFrameToDebrisMapUser)
                character->mAddFrameToDebrisMapCallback(
                  character->mAddFrameToDebrisMapUser,
                  character->mAnimation->GetFrame(), character->GetPosition(), character->GetRenderableAngle(), character->mScale);
            return true;
        }
        else if (strcmp(data, "Destroy") == 0)
        {
            DestroyCharacterDeferred(character);
            return true;
        }
        else
        {
            // $TODO ..
        }
    }
    return false;
}

void Character::SetAnimation(const char *name)
{
    String nameStr(name);
    mAnimation = mAnimations[nameStr];
    if (!mAnimation)
    {
        char tag[1024];
        sprintf(tag, "%s.%s", mType.c_str(), name);
        mAnimation = new Animation;
        mAnimation->Load(mRenderables, tag);
        mAnimations[nameStr] = mAnimation;
        mAnimation->SetTagCallback(Character::AnimationTagCallback, this);
    }
    mAnimation->SetAlpha(mAlpha); 
    mAnimation->SetActive();
    mAnimationName = nameStr;
}

void Character::SetMap(Map *map)
{
    mMap = map;
}

bool Character::IsVisible() const
{
    return mVisible;
}

void Character::SetVisible(bool v)
{ 
    mVisible = v;
}

bool Character::IsCollidable() const
{
    return mCollidable;
}

void Character::SetCollidable(bool v)
{ 
    mCollidable = v;
}

bool Character::IsMovable() const
{
    return mMovable;
}

void Character::SetMovable(bool v)
{ 
    mMovable = v;
}

void Character::SetFixed(bool v)
{ 
    mFixed = v;
}

bool Character::IsFixed() const
{
    return mFixed;
}

void Character::Reset(const Vector3 &position)
{
    mVisible = true;
    mAngle = 0.f;
    mFilteredAngle.Set(mAngle);
    SetAnimation(IDLE_ANIMATION);
    SetPosition(position);
}

bool Character::AddEffect(IEffect *effect)
{ 
    for (int i = 0; i < MAX_EFFECTS; i++)
    {
        if (!mEffects[i])
        {
            mEffects[i] = effect;
            return true;
        }
    }
    delete effect;
    return false;
}

void Character::CompleteEffects()
{ 
    for (int i = 0; i < MAX_EFFECTS; i++)
        if (mEffects[i])
            mEffects[i]->Complete();
}

void Character::Update(const GameTime &time)
{
//    if (IsPersistent()) // $HACK persistent is the same as player right now
//        FaceClosestAttackable(GetRadius() * 2.5f, math::PIf * 0.5f);
    // $NOTE filtering!
    if (mFilteredPosition.Get().lengthsq() < 1.f) // hasn't been inited yet
        mFilteredPosition.Set(GetPosition());
    else
        mFilteredPosition.Input(GetPosition()); // this really will be for enemies only
    mPrevPosition = GetPosition();
    UpdateBehaviorList();
    if (IsPersistent() || (rand() % mUpdateFrequency) == 0)
        mLuaThread->Execute(mLuaFunctionName.c_str()); // hey - do we only need to call this every other frame?
    for (std::vector<IBehavior *>::iterator i = mBehaviors.begin(), e = mBehaviors.end(); i != e; ++i)
        if (!(*i)->IsSuspended())
            (*i)->Update(time);
    mAnimation->Update(time);
    if (mSelector)
        mSelector->Update(time);
    for (int i = 0; i < MAX_EFFECTS; i++)
    {
        if (mEffects[i])
        {
            mEffects[i]->SetPosition(GetPosition());
            mEffects[i]->Update(time);
            if (mEffects[i]->IsComplete())
            {
                delete mEffects[i];
                mEffects[i] = 0;
            }
        }
    }
}

bool Character::StaticCollides(const Vector3 &p, float radius, Vector3 &resolve)
{
    resolve = Vector3();
    ICollidable::Classification classify = mMap->Collision(p, radius, resolve);
    switch (classify)
    {
    case ICollidable::CLASS_IN:  resolve = Vector3(); return true;  // we don't know how to get out..
    case ICollidable::CLASS_OUT: resolve = Vector3(); return false; // we don't NEED to get out..
    case ICollidable::CLASS_ON:  return true;                       // the resolve vector will show us the way out..
    }
    return false;
}

int Character::CollidesAt(const Vector3 &p, float radius)
{
    int collision = 0;
    Vector3 collidesAt;

    ICollidable::Classification classify = mMap->Collision(p, radius, collidesAt);
    switch (classify)
    {
    case ICollidable::CLASS_IN:  collision = 511; break;  // all inside bits set
    case ICollidable::CLASS_OUT: collision = 0;   break;  // no inside bits set
    case ICollidable::CLASS_ON:
        {
                // $TODO
            collision = 511;
        }
        break;
    }

    if (GetDynamicMap()->Collides(this))
        collision |= 512;

    return collision;
}     
        
void Character::Draw(RenderContext &context, const GameTime &time)
{
//  if (mVisible)
    {
        Vector3 p;
        if (mFilterPosition) p = (mFilteredPosition.Get() + mTranslate);
        else                 p = (GetPosition() + mTranslate);
        Render(context, p);
        if (mVisible)
            for (std::vector<IBehavior *>::iterator i = mBehaviors.begin(), e = mBehaviors.end(); i != e; ++i)
                if (*i) (*i)->Draw(context, p, time);
    }
}

void Character::ResetAnimation()
{
    mAnimation->Reset();
}

float Character::GetFilteredAngle() const
{
    return mFilteredAngle.Get();
}

bool Character::IsFacing(const Vector3 &position, float tolerance)
{
    const Vector3 NORTH(0.f, 1.f, 0.f);
    Vector3 toward(position - GetPosition());
    float angle = MathUtil::GetAngle(NORTH, toward, 0.0f);
    return MathUtil::AreAnglesClose(angle, GetOrientationAngle(), tolerance);
}

static bool Faces(void *user, Movable *other)
{
    Character *self = (Character *)user;
    return self->IsFacing(other->GetPosition(), 1.0f);
}

extern bool IsKillable(Character *c); // this is a cheat..
extern bool IsDestroyed(Character *c); // this is a cheat..
extern void Turn(Character *c, float angle, float force);
extern void SetForce(Character *c, const Vector3 &force);

static bool Attackable(void *user, Movable *other)
{
    Character *self = (Character *)user;
    Character *en   = (Character *)other;
    return self && en &&
           self->IsFacing(en->GetPosition(), 1.0f) &&
           IsKillable(self) && !IsDestroyed(self) && self->IsVisible() &&
           IsKillable(en) && !IsDestroyed(en) && en->IsVisible();
}

static bool AttackableOnly(void *user, Movable *other)
{
    Character *self = (Character *)user;
    Character *en   = (Character *)other;
    return self && en &&
           !en->IsPersistent() &&
           IsKillable(self) && !IsDestroyed(self) && self->IsVisible() &&
           IsKillable(en) && !IsDestroyed(en) && en->IsVisible();
}

static bool FacesAndAttackable(void *user, Movable *other)
{
    return Faces(user, other) && Attackable(user, other);
}

static bool FacesAndAttackableOnly(void *user, Movable *other)
{
    return Faces(user, other) && AttackableOnly(user, other);
}

float Character::GetAngleTo(const Vector3 &position) const
{
    const Vector3 NORTH(0.f, 1.f, 0.f);
    Vector3 toward(position - GetPosition());
    float angle = MathUtil::GetAngle(NORTH, toward, 0.0f);
    return MathUtil::GetAngleDifference(GetOrientationAngle(), angle);
}

bool Character::FaceClosestAttackable(float radius, float maxAngle)
{
    DynamicMapIterator iterator = GetDynamicMap()->GetAllClosest(this, radius);
    
    Vector3  thisAt(GetPosition());
    float    minAngle   = math::TWOPIf;
    float    minDist    = radius;
    Movable *found      = 0;
    
    while (iterator.HasNext())
    {
        Movable *item = iterator.GetNext();    
        if (item != this)
        {            
            float angle    = GetAngleTo(item->GetPosition());
            float distance = GetPosition().distance(item->GetPosition());
            /*
            if (distance <= radius && fabsf(angle) < fabsf(minAngle) && item->IsCollidable() && AttackableOnly(this, item))
            {
                minAngle = angle;
                found = item;
            }
            */
            if (distance <= minDist && item->IsCollidable() && AttackableOnly(this, item))
            {
                minDist = distance;
                minAngle = angle;
                found = item;
            }
            
        }
    }

    float absAngle = fabsf(minAngle);
    if (!found || absAngle >= maxAngle || absAngle < 0.3f)
        return false;
    
    const Vector3 NORTH(0.f, radius * 0.1f, 0.f);
    Matrix r;
    r.rotationz(GetOrientationAngle() - minAngle);
    SetForce(this, r * NORTH);
    return true;
}

Character *Character::GetClosestFacing(float radius)
{
    return (Character *)GetDynamicMap()->GetClosest(this, radius, Faces, this);
}

Character *Character::GetClosestAttackable(float radius)
{
    return (Character *)GetDynamicMap()->GetClosest(this, radius, Attackable, this);
}

Character *Character::GetClosestFacingAndAttackable(float radius)
{
    return (Character *)GetDynamicMap()->GetClosest(this, radius, FacesAndAttackable, this);
}

const char *Character::GetAnimationName() const
{
    return mAnimationName.c_str();
}

float Character::GetRenderableAngle() const
{
    return (mFilterAngle ? GetFilteredAngle() : GetOrientationAngle()) + mRotation;
}

Matrix Character::GetRotationMatrix() const
{
    Matrix rot, rotz;
    rot.rotationx(math::HALFPIf + mLean.x);            // halfpi to turn the character upright (thanks blender! :S ), then lean forward/back
    if (mLean.y != 0.0f)
    {
        Matrix roty;
        roty.rotationy(mLean.y);                       // then lean left/right (ie, for turns)
        rot = rot * roty;
    }
    rotz.rotationz(GetRenderableAngle()); // then head in the appropriate direction
    return rot * rotz;
}

void Character::Render(RenderContext &context, const Vector3 &position)
{
    bool visible = IsVisible();
    static Matrix rot;
    Matrix translate;
    translate.translate(position);
    Matrix scale;
    scale.scale(mScale);
    Matrix n = scale * GetRotationMatrix();
    mTransform = n * translate;
    context.transform = Matrix();
    if (visible)
        mAnimation->Render(context, mTransform);
    bool anyEffect = false;
    for (int i = 0; i < MAX_EFFECTS; i++) anyEffect |= mEffects[i] != 0;
    if (mSelector || anyEffect)
    {
        RenderContext rc(context);
        rc.transform = translate;
        if (visible && mSelector) mSelector->Render(rc);
        if (mMeter)
        {
            mMeter->SetPosition(position);
            if (visible)
                mMeter->Render(rc);
        }
        //if (visible)
        {
            for (int i = 0; i < MAX_EFFECTS; i++)
                if (mEffects[i]) mEffects[i]->Render(rc);
        }
    }
}

// IBehavior support
IBehavior *Character::GetBehavior(int behaviorTypeId)
{
    for (std::vector<IBehavior *>::iterator i = mBehaviors.begin(), e = mBehaviors.end(); i != e; ++i)
        if (*i && (*i)->IsOfType(behaviorTypeId))
            return *i;
    return 0;
}

// IBehavior support
int Character::GetBehaviorIndex(int behaviorTypeId, int startIndex)
{
    for (int i = startIndex, sz = mBehaviors.size(); i < sz; i++)
        if (mBehaviors[i] && mBehaviors[i]->IsOfType(behaviorTypeId))
            return i;
    return -1;
}

void Character::UpdateBehaviorList()
{
    mBehaviors.erase(std::remove(mBehaviors.begin(), mBehaviors.end(), (IBehavior *)0), mBehaviors.end());
}

void Character::AddBehavior(IBehavior *behavior)
{
    if (behavior)
    { 
        bool found = false;
        for (std::vector<IBehavior *>::iterator i = mBehaviors.begin(), e = mBehaviors.end(); i != e; ++i)
        {
            if (!(*i))
            {
                (*i) = behavior;
                found = true;
            }
        }
        if (!found)
        {
            mBehaviors.push_back(behavior);
        }
        behavior->Start(this);
    }
}

bool Character::HasBehavior(int behaviorTypeId)
{
    return GetBehavior(behaviorTypeId) != 0;
}

void Character::RemoveBehavior(int behaviorTypeId)
{ 
    int i = GetBehaviorIndex(behaviorTypeId, 0);
    while (i != -1)
    {
        if (mBehaviors[i])
        {
            IBehavior *behavior = mBehaviors[i];
            mBehaviors[i] = 0;
            behavior->Stop();
            delete behavior;
        }
        i = GetBehaviorIndex(behaviorTypeId, i + 1);
    }
}

void Character::SuspendBehavior(int behaviorTypeId)
{
    IBehavior *behavior = GetBehavior(behaviorTypeId);
    if (behavior)
        behavior->Suspend();
}

void Character::ResumeBehavior(int behaviorTypeId)
{
    IBehavior *behavior = GetBehavior(behaviorTypeId);
    if (behavior)
        behavior->Resume();
}

void Character::ResetBehavior(int behaviorTypeId)
{
    IBehavior *behavior = GetBehavior(behaviorTypeId);
    if (behavior)
        behavior->Reset();
}

void Character::ResetBehaviors()
{
    for (std::vector<IBehavior *>::iterator i = mBehaviors.begin(), e = mBehaviors.end(); i != e; ++i)
        if (*i) (*i)->Reset();
}

void Character::Signal(int signal)
{
    for (std::vector<IBehavior *>::iterator i = mBehaviors.begin(), e = mBehaviors.end(); i != e; ++i)
        if (*i) (*i)->Signal(signal);
    mSignalHandler.Signal(signal);
}

void Character::AddSignalHandler(int signal, bool (*function)(void *), void *user)
{
    mSignalHandler.Add(signal, function, user);
}

void Character::AddLuaSignalHandler(int signal, const char *function)
{
    mSavedCallbackContexts.push_back(new LuaCharacterCallback(function, this));
    AddSignalHandler(signal, LuaCharacterCallback::callback, mSavedCallbackContexts.back());
}

void Character::ClearSignalHandlers()
{
    mSignalHandler.Clear();
    for (std::vector<LuaCharacterCallback *>::iterator i = mSavedCallbackContexts.begin(), e = mSavedCallbackContexts.end(); i != e; ++i)
        delete *i;
    mSavedCallbackContexts.clear();
}

LuaCharacterCallback::LuaCharacterCallback(const char *function, Character *character)
    : function(function), character(character)
{
}

LuaCharacterCallback::LuaCharacterCallback(const LuaCharacterCallback &callback)
    : function(callback.function), character(callback.character)
{
}

LuaCharacterCallback &LuaCharacterCallback::operator=(const LuaCharacterCallback &other)
{
    if (this != &other)
    {
        character = other.character;
        function  = other.function;
    }
    return *this;
}

bool LuaCharacterCallback::callback(void *user)
{
    LuaCharacterCallback *charCallback = (LuaCharacterCallback *)user;
    bool success = false;
    if (charCallback && charCallback->character)
    {
        Character *character = charCallback->character;
        LuaValue arg[1] = { LuaValue::UserData(character->GetLuaReference()) };
        LuaCall(character->GetLuaThread()->GetLuaState(), charCallback->function.c_str(), arg, 1);
        success = true;
    }
    return success;
}