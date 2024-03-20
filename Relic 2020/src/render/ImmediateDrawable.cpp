#include "ImmediateDrawable.h"

#include "core/core.h"
#include "core/aabox.h"

#include "IDrawable.h"
#include "RenderImmediate.h"
#include "RenderContext.h"
#include "GraphicsDevice.h"
#include "OverheadCamera.h"
#include "Visibility.h"

class RenderContext;
class Texture;

ImmediateDrawable::ImmediateDrawable(const ImmediateVertex *vertices, int count) : mVertices(0), mCount(count)
{
    mVertices = reinterpret_cast<ImmediateVertex *>(soa_malloc(sizeof(ImmediateVertex) * mCount));
    memcpy(mVertices, vertices, mCount * sizeof(ImmediateVertex));
    // calculate bounds
    AABox bounds;
    for (int i = 0; i < mCount; i++) {
        ImmediateVertex &v = mVertices[i];
        bounds.insert(Vector3(v.x, v.y, v.z));
    }
    mBounds = bounds;
}

ImmediateDrawable::~ImmediateDrawable()
{
    if (mVertices) {
        soa_free(mVertices);
    }
    mVertices = 0;
    mCount = 0;
    mBounds = AABox();
}

void ImmediateDrawable::RenderImmediate(RenderContext &context)
{
//  context.device.EnableCullFace   (mCull);
//  context.device.EnableDepthWrite (mDepthWrite);
//  context.device.EnableDepthTest  (mDepthTest);
    ::RenderImmediate(context, mVertices, mCount);
//  context.device.EnableCullFace   (true);
//  context.device.EnableDepthWrite (true);
//  context.device.EnableDepthTest  (true);
}

bool ImmediateDrawable::IsVisible(const RenderContext &context) const
{
    return Visibility::IsVisible(context.camera.GetViewProjection(), context.transform, mBounds);
}

const AABox &ImmediateDrawable::GetBounds() const
{
    return mBounds;
}

void ImmediateDrawable::Retain()
{
    mRef++;
}

void ImmediateDrawable::Release() 
{
    if (!--mRef) {
        delete this;
    }
}
