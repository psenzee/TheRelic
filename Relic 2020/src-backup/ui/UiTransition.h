#ifndef _UITRANSITION_H
#define _UITRANSITION_H

#include "core/core.h"
#include "IUiControl.h"
#include "fast/Allocator.h"

class UiTransition
{
    enum { DEFAULT_MAX_VISIBILITY_FRAMES = 10 };

public:
    
    CLASS_NEW_DELETE()
    
    inline UiTransition(IUiControl *control, int maxframes = DEFAULT_MAX_VISIBILITY_FRAMES) 
        : mControl(control), mMaxVisibilityFrames(maxframes), mVisibilityFrames(0), mMaxScale(20.f) {}

    void         NotifyVisible(bool visible);
    inline bool  IsTransitionComplete()  const { return mVisibilityFrames == 0; }

    inline bool  ShouldRender()          const { return IsVisible() || mVisibilityFrames != 0; }
    void         Update();

    inline float GetAdditiveScale()      const { return GetInterpolant() * mMaxScale; }
    inline void  SetMaxScale(float s)          { mMaxScale = s; }
    inline float GetAlpha()              const { return 1.f - GetInterpolant(); }

private:

    inline float GetInterpolant() const { return (abs(mVisibilityFrames) / (float)mMaxVisibilityFrames); }
    inline bool  IsVisible()      const { return mControl && mControl->IsVisible(); }

    IUiControl *mControl;
    int         mVisibilityFrames;
    int         mMaxVisibilityFrames;
    float       mMaxScale;
};

#endif // _UITRANSITION_H