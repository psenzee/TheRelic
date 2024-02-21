#ifndef _SELECTOR_H
#define _SELECTOR_H

#include "core/core.h"

class GameTime;
class RenderContext;
class Drawable;

class Selector
{
public:

    Selector(Drawable *drawable, float rotate);
    ~Selector();

    void SetDrawable(Drawable *drawable);

    void Update(const GameTime &time);

    void Render(RenderContext &context);

private:

    Drawable *mDrawable;
    float     mAngle; // currently we rotate it
    float     mRotate;
};

#endif // _SELECTOR_H