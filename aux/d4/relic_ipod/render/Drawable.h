#ifndef _DRAWABLE_H // DESPERATELY NEEDS A NAME CHANGE
#define _DRAWABLE_H

#include "core/core.h"
#include "core/aabox.h"
#include "core/matrix4f.h"

#include "Material.h"
#include "RenderStates.h"
#include "IDrawable.h"
#include "RenderSet.h"

#include "gamecore/IGameObject.h"
#include "gamecore/IRenderable.h"
#include "gamecore/IPropertySet.h"

#include "fast/Allocator.h"

class RenderContext;
class Texture;
class Drawable;
class ContentLoader;
struct ImmediateVertex;

Drawable *CreateMesh     (const RenderSet::String &renderName,
                          ContentLoader           *loader,
                          const char              *file,
                          Texture                 *texture,
                          const Matrix            &transform,
                          const RenderStates      &states,
                          const Material          &material);

Drawable *CreateImmediate(const RenderSet::String &renderName,
                          const ImmediateVertex   *vertices,
                          int                      vertexCount,
                          Texture                 *texture,
                          const Matrix            &transform,
                          const RenderStates      &states,
                          const Material          &material);

// This is a leaf type that wraps IDrawable.  It adds a transform, a texture, a material and renderstates.
// It does not have children or state.  It is enqueued into the render list with Render or rendered immediately with RenderImmediate
class Drawable : public IGameObject, public IRenderable, public IPropertySet
{
public:
    
    CLASS_NEW_DELETE()

    Drawable(const RenderSet::String &renderName,
             IDrawable *drawable,
             const Matrix4f &transform,
             const Material &material,
             const RenderStates &states,
             Texture *texture);

    ~Drawable();

    IDrawable         *GetDrawable();
    void               SetDrawable(IDrawable *drawable);

    IGameObject       *Copy();

    Texture           *GetTexture();
    void               SetTexture(Texture *texture);
    const Matrix4f    &GetTransform();
    Material           GetMaterial() const;
    RenderStates      *GetStates();

    void               Update(const GameTime &time);

    void               RenderImmediate(RenderContext &context);
    void               Render(RenderContext &context);
    bool               IsVisible(RenderContext &context);
    AABox              GetBounds();

    IRenderable       &GetRenderable() { return *this; }
    IObjectList       &GetObjectList();
    IPropertySet      &GetProperties() { return *this; }
    IEventList        &GetEventList();

    ICollidable       *GetCollidable() { return 0; }

    bool               IsMutable() { return true; } // $TODO

    void               Retain();
    void               Release();

    bool               SetProperty(const char *key, const char *value);
    const char        *GetProperty(const char *key) const;
    
private:
    
    RenderSet::String  mRenderName;
    IDrawable         *mDrawable;
    Material           mMaterial;
    Matrix4f           mTransform;
    RenderStates       mStates;
    Texture           *mTexture;
    Vector3            mCenter;
    int                mRef;
};

#endif // _DRAWABLE_H
