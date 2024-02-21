#include "UiImageButton.h"
#include "UiCore.h"

int UiImageButton::Render(UiCore &core)
{
    if (/*ShouldRender() */IsVisible())
    {
        UiBounds bounds = UiCore::GetAbsoluteBounds(this);
        if (GetState().IsFocused())
        {
            Vector2 size(bounds.maximum - bounds.minimum);
            bounds.minimum -= size * 0.5f;
            bounds.maximum += size * 0.5f;
        }
        float alpha = mAlpha * GetTransitionAlpha();
        float size  = mSize + GetTransitionAdditiveScale();
        core.DrawImageButton(mFilename.c_str(), GetText(), GetJustification(),
                             alpha, size,
                             bounds.minimum, bounds.maximum, mT0, mT1, mAngle);
        return 1;
    }
    return 0;
}