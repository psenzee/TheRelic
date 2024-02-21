#ifndef _IMMEDIATEDRAWABLE_H
#define _IMMEDIATEDRAWABLE_H

#include "core/core.h"
#include "core/aabox.h"

#include "IDrawable.h"
#include "RenderImmediate.h"
#include "fast/Allocator.h"

class RenderContext;
class Texture;

class ImmediateDrawable : public IDrawable
{
public:

    CLASS_NEW_DELETE()

    ImmediateDrawable(const ImmediateVertex *vertices, int count);
    ~ImmediateDrawable();

    void         RenderImmediate(RenderContext &context);
    bool         IsVisible(const RenderContext &context) const;
    const AABox &GetBounds() const;

    bool         SetProperty(const char *, const char *) { return false; }

    void         Retain();
    void         Release();

private:

    ImmediateVertex *mVertices;
    int              mCount;
    AABox            mBounds;
    int              mRef;
};

#endif // _IMMEDIATEDRAWABLE_H
