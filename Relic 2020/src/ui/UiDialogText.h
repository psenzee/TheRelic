#ifndef _UIDIALOGTEXT_H
#define _UIDIALOGTEXT_H

#include "core/core.h"
#include "UiBasicControl.h"
#include "fast/Allocator.h"

class UiDialogText : public UiBasicControl
{
public:
    
    CLASS_NEW_DELETE()
    
    inline UiDialogText(const char *name, float alpha, float size, bool center) : UiBasicControl(name ? name : ""), mAlpha(alpha), mSize(size), mCenter(center), mFrames(0), mStringOffset(0), mFramesToWait(10)
      { SetTransition(new UiTransition(this)); }

    int Render(UiCore &core);

private:

    float  mAlpha;
    float  mSize;
    int    mFrames,
           mStringOffset,
           mFramesToWait;
    bool   mCenter;
};

#endif // _UIDIALOGTEXT_H