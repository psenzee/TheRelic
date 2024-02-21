#include "Character.h"

#include "core/core.h"
#include "core/random.h"
#include "core/strs.h"

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

#include "Selector.h"

const char *IDLE_ANIMATION = "Idle";

Character::Character(const Vector3 &position, XmlReadContext *renderables, const char *type, const char *luaFunctionName) 
  : Movable(position), mAnimation(0), mSelector(0), mEffect(0), mMap(0), mVisible(true), mCollidable(true), mMovable(true), mFixed(false),
    mLuaThread(0), mLuaFunctionName(luaFunctionName), mAngle(0.f), mScale(2.0f), mRenderables(renderables),
    mLuaReference(0), mLuaTable(LuaTable::Null()), mAngleOffset(0.0f), mRotation(0.0f), mType(type)
{
	mLuaThread = LuaInterpreter::GetInstance()->CreateThread(this);
    Reset(position);
    SetAnimation(IDLE_ANIMATION);
}

Character::~Character()
{
    LuaInterpreter::GetInstance()->DestroyThread(mLuaThread);
    for (std::map<String, Animation *>::iterator i = mAnimations.begin(), e = mAnimations.end(); i != e; ++i)
        delete (*i).second;
    mAnimations.clear();
    mAnimation = 0;
}

const char *Character::GetType() const
{
    return mType.c_str();    
}

bool Character::AnimationTagCallback(void *user, const char *data)
{
    Character *character = static_cast<Character *>(user);
    char       buffer[1024];
    if (character && data)
    {
        if (GetTagStringArgument(data, "GoTo", buffer))
        {
//          printf("Animation/Character: GoTo (Animation) '%s'\n", buffer);
            character->SetAnimation(buffer);
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
    mAngleOffset = mAngle = 0.f;
    SetAnimation(IDLE_ANIMATION);
	SetPosition(position);
}

void Character::Update(const GameTime &time)
{
	mLuaThread->Execute(mLuaFunctionName.c_str());
    mAnimation->Update(time);
    if (mSelector)
        mSelector->Update(time);
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
    if (mVisible)
	{
        Vector3 p(GetPosition());
        Render(context, Vector3(p.x, p.y, p.z) + mTranslate);
	}
}

void Character::ResetAnimation()
{
    mAnimation->Reset();
}

void Character::SetAngleOffset(float offset)
{
    mAngleOffset = offset;
}

float Character::GetAngleOffset() const
{
    return mAngleOffset;
}

bool Character::IsFacing(const Vector3 &position, float tolerance)
{
    const Vector3 NORTH(0.f, 1.f, 0.f);
    Vector3 toward(position - GetPosition());
    float angle = MathUtil::GetAngle(NORTH, toward, 0.0f);
    return MathUtil::AreAnglesClose(angle, mAngle, tolerance);
}

static bool Faces(void *user, Movable *enemy)
{
    Character *self = (Character *)user;
    return self->IsFacing(enemy->GetPosition(), 1.0f);
}

Character *Character::GetClosestAttackable(float radius)
{
    return (Character *)GetDynamicMap()->GetClosest(this, radius, Faces, this);
}

const char *Character::GetAnimationName() const
{
    return mAnimationName.c_str();
}

float Character::CalculateOrientationAngle()
{
    const Vector3 NORTH(0.f, 1.f, 0.f);
    return MathUtil::GetAngle(NORTH, GetVelocity(), mAngle);
}

void Character::Render(RenderContext &context, const Vector3 &position)
{
    static Matrix rot;
    Matrix translate;
    translate.translate(position);
	Matrix upright;
	upright.rotationx(math::HALFPIf);
    rot.rotationz(GetOrientationAngle() + mRotation);
	Matrix scale;
    scale.scale(mScale);
	Matrix n = (scale * upright) * rot;
    mTransform = n * translate;
    context.transform = Matrix();
	mAnimation->Render(context, mTransform);
    if (mSelector || mEffect)
    {
        RenderContext rc(context);
        rc.transform = translate;
        if (mSelector) mSelector->Render(rc);
        if (mEffect)   mEffect->Render(rc);
    }
}