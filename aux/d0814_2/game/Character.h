#ifndef _CHARACTER_H
#define _CHARACTER_H

#include "core/core.h"
#include "core/random.h"
#include "core/streams.h"
#include "math/Filters.h"
#include "animation/Animation.h"
#include "map/DynamicMap.h"
#include "fast/Allocator.h"

#include "behaviors/Signals.h"

#include "IEffect.h"

#include <map>
#include <vector>

class Map;
class XmlElement;
class XmlReadContext;
class GameTime;
class RenderContext;
class LuaInterpreter;
class Drawable;
class Selector;
class Meter;
class IBehavior;

#include "luautil/LuaThread.h"
#include "luautil/ILuaObject.h"

struct LuaCharacterCallback;

typedef bool (*AddFrameToDebrisMapCallback)(void *user, IGameObject *frame, const Vector3 &position, float angle, float scale);

struct CharacterPositionPacket
{
    unsigned char data[9];
};

class Character : public ILuaObject, public Movable
{
public:

    CLASS_NEW_DELETE()    

    Character(const Vector3 &position, XmlReadContext *renderables, const char *type, const char *luaFunctionName);

    ~Character();

    void              SetMap(Map *map);

    void              SetLuaReference(int id)              { mLuaReference = id; }
    int               GetLuaReference()              const { return mLuaReference; }

    LuaThread        *GetLuaThread()                       { return mLuaThread; }
    LuaTable          GetLuaTable()                  const { return mLuaTable; }
    void              SetLuaTable(const LuaTable &t)       { mLuaTable = t; }

    float             GetOrientationAngle()          const { return mAngle; }
    float             GetRenderableAngle()           const;
    void              SetOrientationAngle(float a)         { mAngle = a; mFilteredAngle.Input(a); }
    void              ForceOrientationAngle(float a)       { mAngle = a; mFilteredAngle.Set(a); }

    const char       *GetType() const;
    
    void              Reset(const Vector3 &position);

    void              Draw(RenderContext &context, const GameTime &time);

    void              SetAlpha(float alpha) { mAnimation->SetAlpha(alpha); }

    virtual void      Update(const GameTime &time);
    
    bool              IsFacing(const Vector3 &position, float tolerance = math::PI * 0.25f);
    bool              IsPersistent() const { return mPersistent; }
    void              SetPersistent(bool value) { mPersistent = value; }

    Character        *GetClosestAttackable(float radius);
    Character        *GetClosestFacing(float radius);
    
    bool              IsVisible() const;
    void              SetVisible(bool v);
    bool              IsCollidable() const;
    void              SetCollidable(bool v);
    bool              IsMovable() const;
    void              SetMovable(bool v);
    bool              IsFixed() const;
    void              SetFixed(bool v);
    void              SetStatic(bool v)              { mStatic = v; }
    bool              IsStatic() const               { return mStatic; }

    void              SetWeight(float v)             { if (v <= 0.f) v = 0.01f; mWeight = v; }
    float             GetWeight() const              { return mWeight; }

    void              SetScale(float s)              { mScale = s; }
    void              SetRotation(float r)           { mRotation = r; }
    float             GetRotation()            const { return mRotation; }
    void              SetLean(const Vector2 &v)      { mLean = v; }
    void              SetTranslate(const Vector3 &v) { mTranslate = v; }
    void              SetFilterAngle(bool v)         { mFilterAngle = v; }
    float             GetFilteredAngle() const;

    const Vector3    &GetPreviousPosition()     const { return mPrevPosition; }

    void              SetSelector(Selector *selector) { mSelector = selector; }
    Selector         *GetSelector()                   { return mSelector; }
    bool              AddEffect(IEffect *effect);
    void              CompleteEffects();

    void              SetMeter(Meter *meter)          { mMeter = meter; }
    Meter            *GetMeter()                      { return mMeter; }
    
    float             GetAngleTo(const Vector3 &position) const;
    bool              FaceClosestAttackable(float radius, float maxAngle);

    void              SetAnimation(const char *name);
    const char       *GetAnimationName() const;
    void              ResetAnimation();

    Animation        *GetAnimation()       { return mAnimation; }
    const Animation  *GetAnimation() const { return mAnimation; }

    int               CollidesAt(const Vector3 &p, float radius);
    bool              StaticCollides(const Vector3 &p, float radius, Vector3 &resolve);

    // Lua table access support (useful for behaviors)
    LuaValue          GetProperty(const String &property); // from associated lua table
    void              SetProperty(const String &property, LuaValue &value); // into associated lua table

    void              SetFilterPosition(bool value) { mFilterPosition = value; }

    // IBehavior support
    IBehavior        *GetBehavior(int behaviorTypeId);
    void              AddBehavior(IBehavior *behavior);
    bool              HasBehavior(int behaviorTypeId);
    void              RemoveBehavior(int behaviorTypeId);
    void              SuspendBehavior(int behaviorTypeId);
    void              ResumeBehavior(int behaviorTypeId);
    void              ResetBehavior(int behaviorTypeId);
    void              ResetBehaviors();
    void              Signal(int signal);
    void              AddSignalHandler(int signal, bool (*function)(void *), void *user);
    void              AddLuaSignalHandler(int signal, const char *function);
    void              ClearSignalHandlers();

    inline void       SetAddFrameToDebrisMapCallback(AddFrameToDebrisMapCallback fn, void *user)
    {
        mAddFrameToDebrisMapCallback = fn;
        mAddFrameToDebrisMapUser     = user;
    }

    // serialization
    void              Serialize(IOutStream &stream);
    void              DeserializeUpdate(int id, IInStream &s);
    static Character *DeserializeCreate(int id, IInStream &s);

    // static
    static bool       AnimationTagCallback(void *user, const char *data);

    static bool       IsCharacterValid(const Character *c);
    static void       InvalidateCharacter(Character *c) { SetCharacterValid(c, false); }

    inline static Character       *GetValidCharacter(Character *c)       { return (!c || !Character::IsCharacterValid(c)) ? 0 : c; }
    inline static const Character *GetValidCharacter(const Character *c) { return (!c || !Character::IsCharacterValid(c)) ? 0 : c; }
    
private:    

    void              Render(RenderContext &context, const Vector3 &position);
    int               GetBehaviorIndex(int behaviorTypeId, int startIndex);
    void              UpdateBehaviorList();
    Matrix            GetRotationMatrix() const;

    static void       SetCharacterValid(const Character *c, bool valid);

    static int        sCharacterCount;
   
    std::map<String, Animation *> mAnimations;
    std::vector<IBehavior *>      mBehaviors;

    std::vector<LuaCharacterCallback *> mSavedCallbackContexts;

    enum { MAX_EFFECTS = 8 }; // maximum number of effects at once..

    SignalHandler   mSignalHandler;

    Animation      *mAnimation; // currently active animation
    Selector       *mSelector;
    Meter          *mMeter;
    IEffect        *mEffects[MAX_EFFECTS];
    String          mAnimationName;
    Map            *mMap;

    AddFrameToDebrisMapCallback  mAddFrameToDebrisMapCallback;
    void                        *mAddFrameToDebrisMapUser;

    LuaThread      *mLuaThread;
    String          mLuaFunctionName;
    int             mLuaReference;
    LuaTable        mLuaTable;
    XmlReadContext *mRenderables;

    bool            mVisible,
                    mCollidable,
                    mMovable,
                    mFixed,

                    mFilterPosition,
                    mFilterAngle;

    float           mAngle,
                    mScale,
                    mRotation,
                    mWeight;

    FilterAngle     mFilteredAngle;
    FilterValue3    mFilteredPosition;

    Vector2         mLean;
    Vector3         mTranslate;
    Vector3         mPrevPosition;
    Matrix          mTransform;
    
    String          mType;
    bool            mPersistent;
    bool            mStatic;
};

CharacterPositionPacket GetCharacterPositionPacket(Character *ch);
void                    SetCharacterPositionPacket(Character *ch, const CharacterPositionPacket &packet);
int                     GetCharacterIdFromPositionPacket(const CharacterPositionPacket &packet);

template <typename T, int ID>
inline T *GetBehavior(Character *c)   { return static_cast<T *>(!c ? 0 : c->GetBehavior(ID)); }

struct LuaCharacterCallback
{
    CLASS_NEW_DELETE()
    
    String      function;
    Character  *character;

    typedef bool (*function_t)(void *data);

    LuaCharacterCallback(const char *function, Character *character);
    LuaCharacterCallback(const LuaCharacterCallback &callback);

    LuaCharacterCallback &operator=(const LuaCharacterCallback &other);

    static bool callback(void *user);
};

#endif // _CHARACTER_H