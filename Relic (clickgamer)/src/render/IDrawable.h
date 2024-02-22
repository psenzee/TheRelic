#ifndef _IDRAWABLE_H
#define _IDRAWABLE_H

#include "core/core.h"
#include "core/aabox.h"

class RenderContext;

// This is an immutable leaf type - it does not contain a transform,
// a texture or a material. It does not have children or state.  
// It is directly renderable with RenderImmediate.
class IDrawable
{
public:

    virtual ~IDrawable() {}

    virtual void         RenderImmediate(RenderContext &context)         = 0;
    virtual bool         IsVisible(const RenderContext &context) const   = 0;
    virtual const AABox &GetBounds() const                               = 0;

    virtual void         Retain()                                        = 0;
    virtual void         Release()                                       = 0;

    virtual bool         SetProperty(const char *key, const char *value) = 0;
};

#endif // _IPROPERTYHOLDER_H
