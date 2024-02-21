#ifndef _OBJECT_H
#define _OBJECT_H

#include "core/core.h"
#include "core/aabox.h"

#include "render/RenderContext.h"

#include "gamecore/IGameObject.h"
#include "gamecore/IRenderable.h"
#include "gamecore/ICollidable.h"
#include "gamecore/CoreObjectList.h"
#include "gamecore/CorePropertySet.h"
#include "gamecore/CoreEventList.h"
#include "gamecore/ListUtils.h"

#include "Transform.h"

#include <vector>
#include <string>
#include <map>

#include "core/aabox.h"

class Object : public IGameObject, public IRenderable, public IPropertySet
{
public:
        
    Object(std::vector<IGameObject *> objects,
           const Matrix &transform, std::vector<Event *> events, ICollidable *collider);

    void          Retain();
    void          Release();

    void          Update(const GameTime &time)                  { ListUtils::Update(mObjects, time); }

    IGameObject  *Copy();
    bool          IsMutable()                                   { return ListUtils::IsAnyMutable(mObjects); }

    void          Render(RenderContext &context)
    {
        ListUtils::Render(mObjects, GetContext(context));
    }
    void          RenderImmediate(RenderContext &context)       { ListUtils::RenderImmediate(mObjects, GetContext(context)); }
    bool          IsVisible(RenderContext &context)             { return ListUtils::IsAnyVisible(mObjects, GetContext(context)); }
    AABox         GetBounds()                                   { return mBounds; }

    IRenderable  &GetRenderable()                               { return *static_cast<IRenderable *>(this); }
    IObjectList  &GetObjectList()                               { return mObjects; }
    IPropertySet &GetProperties()                               { return *static_cast<IPropertySet *>(this); }
    IEventList   &GetEventList()                                { return mEvents; }
    
    ICollidable  *GetCollidable()                               { return mCollidable; }
    
    bool          SetProperty(const char *key, const char *v)   { return mProperties.SetProperty(key, v) | ListUtils::SetProperty(mObjects, key, v); }
    const char   *GetProperty(const char *key) const            { return mProperties.GetProperty(key); }

private:

    inline RenderContext GetContext(RenderContext &context)
    {
        RenderContext rc(context);
        //rc.transform = rc.transform * mTransform;
        rc.transform = mTransform * rc.transform;
        return rc;
    }

    int             mRef;
    CoreObjectList  mObjects;
    Matrix          mTransform;
    CoreEventList   mEvents;
    AABox           mBounds; // cached
    CorePropertySet mProperties;
    ICollidable    *mCollidable;
};

#endif // _OBJECT_H