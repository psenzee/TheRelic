#ifndef _UICOLLECTION_H
#define _UICOLLECTION_H

#include "core/core.h"
#include "fast/Allocator.h"
#include "UiBasicControl.h"
#include <vector>

class UiCollection : public UiBasicControl
{
public:
    
    CLASS_NEW_DELETE()

    UiCollection(const char *name) : UiBasicControl(name), mLastNotified(0) {}
    ~UiCollection();

    bool        Notify(UiCore &core, const UiEvent &event);

    int         RenderRenderable(UiCore &core);
    int         Render(UiCore &core);
    void        Update(/*..*/);

    bool        IsCollection()                        const { return true; }
    void        AddChild(IUiControl *control);
    bool        RemoveChild(IUiControl *control);
    int         GetChildrenCount()                    const { return (int)mItems.size(); }
    IUiControl *GetChildAt(int at)                          { return mItems[at]; }
    void        DestroyChildren();

    void        NotifyVisible(bool visible);

private:

    std::vector<IUiControl *>  mItems;
    IUiControl                *mLastNotified;
};

#endif // _UICOLLECTION_H