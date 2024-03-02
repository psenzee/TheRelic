#include "UiButton.h"
#include "UiCore.h"

UiBounds UiButton::GetFocusedBounds() const
{
    UiBounds bounds = GetBounds();
    Vector2 size(20.f, 20.f);
    bounds.minimum -= size * 0.5f;
    bounds.maximum += size * 0.5f;
    return bounds;
}

int UiButton::Render(UiCore &core)
{
    if (ShouldRender())
    {
        UiBounds bounds = UiCore::GetAbsoluteBounds(this);
        float textScale = 1.f;
        if (GetState().IsFocused())
        {
          //Vector2 size(bounds.maximum - bounds.minimum);
            Vector2 size(20.f, 20.f);
            bounds.minimum -= size * 0.5f;
            bounds.maximum += size * 0.5f;
            textScale = 1.5f;
        }
        float alpha = mAlpha * GetTransitionAlpha();
        float size  = mSize * textScale + GetTransitionAdditiveScale();
        core.DrawButton(mFilename.c_str(), GetText(), GetJustification(),
                        alpha, size, 
                        bounds.minimum + GetMargin(), 
                        bounds.maximum - GetMargin(), 2.f * 3.75f, 0.4f);
        return 1;
    }
    return 0;
}