#ifndef _NULLRENDERABLE_H
#define _NULLRENDERABLE_H

#include "IRenderable.h"

class NullRenderable : public IRenderable
{
public:

    void          Render(RenderContext &context)                  {}
    void          RenderImmediate(RenderContext &context)         {}
    bool          IsVisible(RenderContext &context)               { return false; }
    AABox         GetBounds()                                     { return AABox(); }

    static NullRenderable instance;
};

#endif // _NULLRENDERABLE_H
