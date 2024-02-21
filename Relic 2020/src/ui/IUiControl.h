#ifndef _IUICONTROL_H
#define _IUICONTROL_H

#include "core/core.h"
#include "Ui.h"

class UiCore;
class UiTransition;

class IUiControl
{
public:

    virtual ~IUiControl() {}

    virtual const char    *GetName()                             const = 0;
    virtual void           SetName(const char *name)                   = 0;

    virtual const char    *GetText()                             const = 0;
    virtual void           SetText(const char *text)                   = 0;

    virtual UiTextJustify  GetJustification()                    const = 0;
    virtual void           SetJustification(UiTextJustify justify)     = 0;

    virtual void          *GetData()                                   = 0; // this may be a LuaValue
    virtual void           SetData(void *data)                         = 0;

    virtual UiState       &GetState()                                  = 0;
    virtual const UiState &GetState()                            const = 0;

    virtual void           SetAnimationTime(float value)               = 0;
    virtual float          GetAnimationTime() const                    = 0;

    virtual void           SetVisible(bool visible)                    = 0;
    virtual bool           IsVisible()                           const = 0;
    virtual void           SetActive(bool active)                      = 0;
    virtual bool           IsActive()                            const = 0;
//  virtual void           SetAlpha(float alpha)                       = 0; // $TODO implement these when you get a chance
//  virtual float          GetAlpha()                            const = 0;

    virtual UiBounds       GetBounds()                           const = 0;
    virtual UiBounds       GetFocusedBounds()                    const = 0;
    virtual void           RequestBounds(const UiBounds &bounds)       = 0;

    virtual void           SetMargin(const Vector2 &margin)            = 0;
    virtual Vector2        GetMargin()                           const = 0;

    virtual bool           Notify(UiCore &core, const UiEvent &event)  = 0;

    virtual int            RenderRenderable(UiCore &core)              = 0;
    virtual int            Render(UiCore &core)                        = 0;
    virtual void           Update()                                    = 0;

    virtual bool           IsCollection()                        const = 0;
    virtual void           AddChild(IUiControl *control)               = 0;
    virtual bool           RemoveChild(IUiControl *control)            = 0;
    virtual int            GetChildrenCount()                    const = 0;
    virtual IUiControl    *GetChildAt(int at)                          = 0;
    virtual void           SetParent(IUiControl *control)              = 0;
    virtual IUiControl    *GetParent()                                 = 0;

    virtual void           NotifyVisible(bool visible)                 = 0;
    virtual void           DestroyChildren()                           = 0;

    virtual UiTransition  *GetTransition()                             = 0;

protected:

    virtual void          SetBounds(const UiBounds &bounds)            = 0;
};

#endif // _IUICONTROL_H