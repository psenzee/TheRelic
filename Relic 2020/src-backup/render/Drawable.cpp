#include "Drawable.h"

#include "core/core.h"
#include "core/aabox.h"

#include "RenderContext.h"
#include "Texture.h"
#include "Material.h"
#include "DrawList.h"
#include "RenderSet.h"
#include "ContentLoader.h"
#include "DeviceMesh.h"
#include "ImmediateDrawable.h"

#include "gamecore/CoreEventList.h"
#include "gamecore/CoreObjectList.h"

Drawable *CreateMesh(const String            &renderName,
                     ContentLoader           *loader,
                     const char              *file,
                     Texture                 *texture,
                     const Matrix            &transform,
                     const Matrix            &uvtransform,
                     const RenderStates      &states,
                     const Material          &material)
{
    if (!loader || !file)
        return 0;
    DeviceMesh *mesh = loader->GetMesh(file);
    return new Drawable(renderName, mesh, transform, uvtransform, material, states, texture);
}

Drawable *CreateImmediate(const String            &renderName,
                          const ImmediateVertex   *vertices,
                          int                      vertexCount,
                          Texture                 *texture,
                          const Matrix            &transform,
                          const RenderStates      &states,
                          const Material          &material)
{
    ImmediateDrawable *imd = new ImmediateDrawable(vertices, vertexCount);
    return new Drawable(renderName, imd, transform, Matrix(), material, states, texture);
}

Drawable::Drawable(const String &renderName, IDrawable *drawable, const Matrix4f &transform, const Matrix4f &uvtransform, const Material &material, const RenderStates &states, Texture *texture) :
    mRenderName(renderName), mDrawable(0), mMaterial(material), mTransform(transform), mStates(states), mTexture(0), mRef(1)
{
    SetDrawable(drawable);
    SetTexture(texture);
    const AABox bounds(GetBounds());
    mCenter = (bounds.minimum + bounds.maximum) * 0.5f;
    mStates.UvTransform = uvtransform;
}

Drawable::~Drawable()
{
    SetDrawable(0);
    SetTexture(0);
}

IGameObject *Drawable::Copy()
{
    this->Retain();
    return this; // $TODO
}

IDrawable *Drawable::GetDrawable()
{
    return mDrawable;
}

void Drawable::SetDrawable(IDrawable *drawable)
{
    if (drawable)
        drawable->Retain();
    if (mDrawable)
        mDrawable->Release();
    mDrawable = drawable;
}

Texture *Drawable::GetTexture()
{
    return mTexture;
}

void Drawable::SetTexture(Texture *texture)
{
    if (texture)
        texture->Retain();
    if (mTexture)
        mTexture->Release();
    mTexture = texture;
}    

const Matrix4f &Drawable::GetTransform()
{
    return mTransform;
}

Material Drawable::GetMaterial() const
{
    return mMaterial;
}

RenderStates *Drawable::GetStates()
{
    return &mStates;
}

void Drawable::RenderImmediate(RenderContext &context)
{
    if (mDrawable)
    {
        RenderContext rc(context);
        //rc.transform = mTransform * context.transform;        
        if (mTexture)
            mTexture->Set(rc.device);
        mDrawable->RenderImmediate(rc);
    }
}

void Drawable::Update(const GameTime &time)
{
}

void Drawable::Render(RenderContext &context)
{
    if (mDrawable)
    {
        RenderContext rc(context);
        rc.transform = mTransform * context.transform;
        if (!mStates.TestVisible || IsVisible(rc))
        {
            Vector3 center(rc.transform * mCenter);
            DrawItem item(rc.transform, mDrawable, mTexture, &mStates, &mMaterial, rc.color, context.blur, center.z);
            item.breakFunction = rc.breakFunction;
            if (rc.transparent)
               item.transparent = true;
            RenderSet::GetInstance()->Add(mRenderName, item);
        }
    }
}

bool Drawable::IsVisible(RenderContext &context)
{
    return mDrawable ? mDrawable->IsVisible(context) : false;
}

AABox Drawable::GetBounds()
{
    return mDrawable ? mDrawable->GetBounds() : AABox();        
}

void Drawable::Retain()
{
    mRef++;        
}

void Drawable::Release()
{
    if (--mRef == 0)
        delete this;
}

IObjectList &Drawable::GetObjectList()
{
    return NullObjectList::instance;
}

IEventList &Drawable::GetEventList() 
{ 
    return NullEventList::instance;
}

bool Drawable::SetProperty(const char *key, const char *value)
{
    // intercept recognized properties
    // else
    if (mDrawable)
        return mDrawable->SetProperty(key, value);
    return false;
}

const char *Drawable::GetProperty(const char *key) const
{
    // intercept recognized properties
    // else
    //if (mDrawable)
    //    return mDrawable->GetProperty(key);
    return 0;
}