#ifndef _CHARACTER_H
#define _CHARACTER_H

#include "core/core.h"
#include "core/random.h"
#include "animation/Animation.h"
#include "map/DynamicMap.h"

#include <map>

class Map;
class XmlElement;
class XmlReadContext;
class GameTime;
class RenderContext;
class LuaInterpreter;
class Drawable;
class Selector;

#include "luautil/LuaThread.h"
#include "luautil/ILuaObject.h"
#include "fast/FixedString.h"

class Character : public ILuaObject, public Movable
{
public:

    typedef FixedString<64> String;

    Character(const Vector3 &position, XmlReadContext *renderables, const char *type, const char *luaFunctionName);
    ~Character();

    void             SetMap(Map *map);

    void             SetLuaReference(int id)              { mLuaReference = id; }
    int              GetLuaReference()              const { return mLuaReference; }

    LuaThread       *GetLuaThread()                       { return mLuaThread; }
    LuaTable         GetLuaTable()                  const { return mLuaTable; }
    void             SetLuaTable(const LuaTable &t)       { mLuaTable = t; }

    float            GetOrientationAngle()          const { return mAngle + mAngleOffset; }
    void             SetOrientationAngle(float a)         { mAngle = a; }
    float            CalculateOrientationAngle();

    const char      *GetType() const;
    
    void             Reset(const Vector3 &position);

    void             Draw(RenderContext &context, const GameTime &time);

    void             SetAlpha(float alpha) { mAnimation->SetAlpha(alpha); }

    void             Update(const GameTime &time);
    
    bool             IsFacing(const Vector3 &position, float tolerance = math::PI * 0.25f);

    Character       *GetClosestAttackable(float radius);
    
    bool             IsVisible() const;
    void             SetVisible(bool v);
    bool             IsCollidable() const;
    void             SetCollidable(bool v);
    bool             IsMovable() const;
    void             SetMovable(bool v);
    bool             IsFixed() const;
    void             SetFixed(bool v);

    void             SetScale(float s)              { mScale = s; }
    void             SetRotation(float r)           { mRotation = r; }
    void             SetTranslate(const Vector3 &v) { mTranslate = v; }
    void             SetAngleOffset(float offset);
    float            GetAngleOffset() const;

    void             SetSelector(Selector *selector) { mSelector = selector; }
    Selector        *GetSelector()                   { return mSelector; }
    void             SetEffect(Drawable *effect)     { mEffect   = effect; }
    
    void             SetAnimation(const char *name);
    const char      *GetAnimationName() const;
    void             ResetAnimation();

    Animation       *GetAnimation()       { return mAnimation; }
    const Animation *GetAnimation() const { return mAnimation; }

    int              CollidesAt(const Vector3 &p, float radius);
    bool             StaticCollides(const Vector3 &p, float radius, Vector3 &resolve);

    // static
    static bool      AnimationTagCallback(void *user, const char *data);

private:    

    void             Render(RenderContext &context, const Vector3 &position);
    
    std::map<String, Animation *> mAnimations;

    Animation      *mAnimation; // currently active animation
    Selector       *mSelector;
    Drawable       *mEffect;
    String          mAnimationName;
    Map            *mMap;

	LuaThread      *mLuaThread;
    String          mLuaFunctionName;
    int             mLuaReference;
    LuaTable        mLuaTable;
    XmlReadContext *mRenderables;

    bool            mVisible,
                    mCollidable,
                    mMovable,
                    mFixed;

    float           mAngle,
                    mAngleOffset,
                    mScale,
                    mRotation;

    Vector3         mTranslate;
	Matrix          mTransform;
    
    String          mType;
};

#endif // _CHARACTER_H