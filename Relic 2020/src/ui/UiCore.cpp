#include "core/core.h"

#include "UiCore.h"
#include "IUiControl.h"
#include "UiText.h"
#include "UiDialogText.h"
#include "UiButton.h"
#include "UiImageButton.h"
#include "UiRenderableButton.h"
#include "UiMenu.h"
#include "GameUI.h"
#include "GLStates.h"
#include "render/Drawable.h"
#include "render/RenderContext.h"
#include "render/QuadRenderer.h"
#include "render/ContentLoader.h"
#include "render/GraphicsDevice.h"
#include "render/RenderSet.h"
#include "render/DrawableUtils.h"
#include "gamecore/IGameObject.h"
#include "gamecore/IObjectList.h"
#include "gamecore/IRenderable.h"
#include "xml/XmlReadContext.h"

extern RenderContext &GetGlobalRenderContext();

UiCore::UiCore(const GameDimensions &dimensions, GameUI &gameUi, QuadRenderer &quads, EventDispatcher &dispatcher) : mGameUi(gameUi), mQuadRenderer(quads),
    mDimensions(dimensions), mDispatcher(dispatcher), mRootControl(0), mRenderables(0)
{
    mScreenSize            = dimensions.GetReferenceBounds().size().xy();
    mDrawLeftTop           = mScreenSize * 0.5;
    mDrawRightBottom       = mDrawLeftTop * -1.0;
    mDrawSize              = Vector2(mDrawLeftTop.x - mDrawRightBottom.x, mDrawLeftTop.y - mDrawRightBottom.y);
    mDrawToScreenSizeRatio = Vector2(mDrawSize.x / mScreenSize.x,         mDrawSize.y / mScreenSize.y);
}

RenderContext &UiCore::GetRenderContext()
{
    return GetGlobalRenderContext();
}

void UiCore::SetRootControl(IUiControl *control)
{ 
    mRootControl = control;
}

IUiControl *UiCore::GetRootControl()
{ 
    return mRootControl;
}

void UiCore::Render()
{ 
    if (mRootControl)
    {
        int count = mRootControl->RenderRenderable(*this);
        if (count) // don't do this unless you have to, it is expensive (especially ClearZBuffer())
        {
            //GraphicsDevice::ClearZBuffer();
            RenderSet::GetInstance()->Render("fx",   GetRenderContext());
            RenderSet::GetInstance()->Render("main", GetRenderContext());
        }
        mRootControl->Render(*this);
    }
}

void UiCore::Update()
{ 
    if (mRootControl)
        mRootControl->Update();
}

static GlyphWriter::TextJustify GetGlyphWriterJustification(UiTextJustify justify)
{
    switch (justify)
    {
    case JUSTIFY_LEFT:   return GlyphWriter::TEXT_JUSTIFY_LEFT;
    case JUSTIFY_CENTER: return GlyphWriter::TEXT_JUSTIFY_CENTER;
    case JUSTIFY_RIGHT:  return GlyphWriter::TEXT_JUSTIFY_RIGHT;
    }
    return GlyphWriter::TEXT_JUSTIFY_LEFT;
}
/*
void UiCore::DrawString(const char *text, const Vector2 &at, float alpha, float size)
{
    DrawString(text, at, JUSTIFY_LEFT, alpha, size);
}

void UiCore::DrawStringGlow(const char *text, const Vector2 &at, float alpha, float size)
{
    DrawStringGlow(text, at, JUSTIFY_LEFT, alpha, size);
}

void UiCore::DrawStringCentered(const char *text, const Vector2 &at, float alpha, float size)
{
    DrawString(text, at, JUSTIFY_CENTER, alpha, size);
}    

void UiCore::DrawStringCenteredGlow(const char *text, const Vector2 &at, float alpha, float size)
{
    DrawStringGlow(text, at, JUSTIFY_CENTER, alpha, size);
}
*/
void UiCore::DrawString(const char *text, const Vector2 &at, UiTextJustify justify, float alpha, float size)
{
	mGameUi.DrawString(GetGlobalRenderContext(), text, ScreenToDrawPosition(at), Vector4(1.f, 1.f, 1.f, alpha), size, true, GetGlyphWriterJustification(justify));
}

void UiCore::DrawStringGlow(const char *text, const Vector2 &at, UiTextJustify justify, float alpha, float size)
{
    mGameUi.DrawString(GetGlobalRenderContext(), text, ScreenToDrawPosition(at), Vector4(1.f, 1.f, 1.f, alpha), size, true, GetGlyphWriterJustification(justify), GameUI::TEXT_DROP_SHADOW | GameUI::TEXT_GLOW);
}

void UiCore::DrawQuad(const char *filename, const Vector4 &color,
                      const Vector2 &start, const Vector2 &end,
                      const Vector2 &t0,    const Vector2 &t1, float angle)
{
    DeviceTexture *texture = ContentLoader::GetInstance()->GetTexture(filename);
    GLStates::depthTest.Set(false);
    //GLStates::depthWrite.Set(false);
    if (angle == 0.f)
    {
        mQuadRenderer.RenderScreenAlignedQuad(
            texture, color,
            v3(ScreenToDrawPosition(start)),
            v3(ScreenToDrawPosition(end)), t0, t1);
    }
    else
    {
        mQuadRenderer.RenderScreenAlignedQuadRotate(
            texture, color,
            v3(ScreenToDrawPosition(start)),
            v3(ScreenToDrawPosition(end)), t0, t1, angle);            
    }
    //GLStates::depthWrite.Set(true);
    GLStates::depthTest.Set(true);
}

void UiCore::DrawScreenQuad(const char *filename, const Vector4 &color, const Vector2 &t0, const Vector2 &t1)
{
    Tuple3f min = mDimensions.GetBounds().minimum,
            max = mDimensions.GetBounds().maximum;
    GLStates::depthTest.Set(false);
    //GLStates::depthWrite.Set(false);
    DrawQuad(filename, color, min.xy(), max.xy(), t0, t1, 0.0f);
    //GLStates::depthWrite.Set(true);
    GLStates::depthTest.Set(true);
}

void UiCore::DrawRing(const char *filename, const Vector4 &color, const Vector3 &center,
                      float innerradius, float outerradius, float angle)
{
    //DeviceTexture *texture = ContentLoader::GetInstance()->GetTexture(filename);
    // $TODO
	Matrix translate, rotate;
    if (!math::zero(angle)) {
		rotate.rotationz(angle);
    }
	translate.translate(v3(ScreenToDrawPosition(v2(center)), center.z));
	Matrix transform(rotate);
	transform = transform * translate;
	Drawable *drawable = CreateRingTransform("fx", filename, 64, innerradius, outerradius, transform);
    GLStates::depthTest.Set(false);
    //GLStates::depthWrite.Set(false);
	drawable->RenderImmediate(GetRenderContext());
    //GLStates::depthWrite.Set(true);
    GLStates::depthTest.Set(true);
	drawable->Release();
}

void UiCore::DrawPanorama(const char *filename, const Vector4 &color,
                          const Vector2 &start, const Vector2 &end, float angle)
{
    DeviceTexture *texture = ContentLoader::GetInstance()->GetTexture(filename);
    mQuadRenderer.RenderScreenAlignedPanoramaRotate(
            texture, color,
            v3(FullScreenToDrawPosition(start)),
            v3(FullScreenToDrawPosition(end)), angle);
}

void UiCore::DrawScreenPanorama(const char *filename, const Vector4 &color, float offset, float angle)
{
    AABox bounds(mDimensions.GetBounds());
    Tuple2f min(bounds.minimum.xy()), max(bounds.maximum.xy());
    Tuple2f panoramaSize(4096.f, 256.f), screenSize(max.x - min.x, max.y - min.y);
    float ratio = panoramaSize.y / screenSize.y;
    float mappedWidth = screenSize.x * ratio;
    float maxx = panoramaSize.x - mappedWidth;
    Tuple2f start(-offset * maxx, 0.f), end((-offset * maxx) + (maxx / ratio), screenSize.y);
    //printf("pano2 %.2f, %.2f > %.2f, %.2f (min.x %.2f)\n", start.x, start.y, end.x, end.y, min.x);
    DrawPanorama(filename, color, start, end, angle);
}

void UiCore::Draw9SliceQuad(const char *filename, const Vector4 &color,
                            const Vector2 &start, const Vector2 &end,
                            float cornerSize, float middleRange)
{
    DeviceTexture *texture = ContentLoader::GetInstance()->GetTexture(filename);
    mQuadRenderer.Render9Slice(
        texture, color,
        v3(ScreenToDrawPosition(end)),
        v3(ScreenToDrawPosition(start)), cornerSize, middleRange);
}

void UiCore::DrawButton(const char *filename, const char *text, UiTextJustify justify, 
                        float alpha, float size,
                        const Vector2 &start, const Vector2 &end,
                        float cornerSize, float middleRange)
{
    if (filename && *filename)
        Draw9SliceQuad(filename, Vector4(1.f, 1.f, 1.f, alpha), start, end, cornerSize, middleRange);
    if (text && *text)
	{
		Vector2 center((start + end) * 0.5f);
		switch (justify)
		{
		case JUSTIFY_CENTER: DrawStringGlow(text, center, justify, alpha, size);	                 break;
		case JUSTIFY_LEFT:   DrawStringGlow(text, Vector2(start.x, center.y), justify, alpha, size); break;
		case JUSTIFY_RIGHT:  DrawStringGlow(text, Vector2(end.x, center.y), justify, alpha, size);   break;
		}
	}
}

void UiCore::DrawImageButton(const char *filename, const char *text, UiTextJustify justify, 
                             float alpha, float size,
                             const Vector2 &start, const Vector2 &end,
                             const Vector2 &t0, const Vector2 &t1,
                             float angle)
{
    if (filename && *filename)
        DrawQuad(filename, Vector4(1.f, 1.f, 1.f, alpha), start, end, t0, t1, angle);
    if (text && *text)
	{
		Vector2 center((start + end) * 0.5f);
		switch (justify)
		{
		case JUSTIFY_CENTER: DrawStringGlow(text, center, justify, alpha, size);	                 break;
		case JUSTIFY_LEFT:   DrawStringGlow(text, Vector2(start.x, center.y), justify, alpha, size); break;
		case JUSTIFY_RIGHT:  DrawStringGlow(text, Vector2(end.x, center.y), justify, alpha, size);   break;
		}
	}
}

static String Parse(const char *item, int *index)
{
    const char *p = item;
    char data[1024];
    memset(data, 0, sizeof(data));
    char *q = data;
    for (; *p && *p != ':'; p++)
    {
        *q = *p;
        *(q + 1) = '\0';
        q++;
    }
    *index = -1;
    if (*p == ':')
        *index = atoi(p + 1);
    return data;
}

void UiCore::DrawRenderable(const char *item, const Vector3 &at, float scale, float angle)
{
    if (item && *item)
    {
        XmlReadContext *renderables = GetRenderables();
        int index = -1;
        String itemName = Parse(item, &index);
        IGameObject *object = renderables ? renderables->objects[itemName] : 0;
        if (!object)
        {
            printf("Ui::DrawRenderable: No object for item '%s' found!\n", item);
            return;
        }
        if (object->GetObjectList().GetObjectCount() > 0 && index != -1)
            object = object->GetObjectList().GetObjects()[index]; // if it's a list, get the first one
        object->Retain();            
        RenderContext rc(GetRenderContext());
        Matrix ms;
        ms.scale(scale);
        Matrix rot;
        rot.rotationx(math::HALFPIf * 0.8);            // halfpi to turn the character upright (thanks blender! :S )
        if (angle)
        {
            Matrix r;
            r.rotationz(angle);
            rot = rot * r;
        }
        if (scale != 1.f)
            rc.transform = rc.transform * ms;
        rc.transform = rc.transform * rot;
        Matrix t;        
        t.translate(v3(ScreenToDrawPosition(v2(at)), at.z));
        rc.transform = rc.transform * t;
        IRenderable &r = object->GetRenderable();
        r.Render(rc);      // Render() enqueues into the drawlist, which Retain()s
        object->Release(); // So we can Release()
    }
}

Vector2 UiCore::GetStringSize(const char *text, float scale)
{
    return ScreenToDrawSize(mGameUi.GetSize(text, Vector2(scale, scale)));
}

UiBounds UiCore::GetAbsoluteBounds(IUiControl *control)
{
    UiBounds bounds = control->GetBounds();
    while (control->GetParent())
    {
        control = control->GetParent();
        Vector2 at = control->GetBounds().minimum;
        bounds.minimum += at;
        bounds.maximum += at;
    }
    return bounds;
}

IUiControl *UiCore::CreateText(const char *name, const char *text, const UiBounds &bounds, float alpha, float size, bool center)
{
    UiText *control = new UiText(name, alpha, size, center);
    control->SetText(text);
    control->RequestBounds(bounds);
    return control;
}

IUiControl *UiCore::CreateDialogText(const char *name, const char *text, const UiBounds &bounds, float alpha, float size, bool center)
{
    UiDialogText *control = new UiDialogText(name, alpha, size, center);
    control->SetText(text);
    control->RequestBounds(bounds);
    return control;
}

IUiControl *UiCore::CreateButton(const char *name, const char *filename, const char *text, UiTextJustify justify, const UiBounds &bounds, float alpha, float size)
{
    UiButton *control = new UiButton(name, filename, alpha, size);
    control->SetText(text);
    control->SetJustification(justify);
    control->RequestBounds(bounds);
    return control;
}

IUiControl *UiCore::CreateImageButton(const char *name, const char *filename, const char *text, UiTextJustify justify, const UiBounds &bounds, float alpha, float size, const Vector2 &t0, const Vector2 &t1, float angle)
{
    UiImageButton *control = new UiImageButton(name, filename, alpha, size, t0, t1, angle);
    control->SetText(text);
    control->SetJustification(justify);
    control->RequestBounds(bounds);
    return control;
}

IUiControl *UiCore::CreateRenderableButton(const char *name, const char *item, const char *text, UiTextJustify justify, const UiBounds &bounds, float alpha, float size, float scale, float angle)
{
    UiRenderableButton *control = new UiRenderableButton(name, item, alpha, size, scale, angle);
    control->SetText(text);
    control->SetJustification(justify);
    control->RequestBounds(bounds);
    return control;
}

IUiControl *UiCore::CreateMenu(const char *name, const char *filename, const char *text, const UiBounds &bounds, float alpha, float size)
{
    UiMenu *control = new UiMenu(name);
    control->SetText(text);
    control->RequestBounds(bounds);
    return control;
}

Vector2 UiCore::InputToScreen(const Vector2 &p) const
{
    /*
    Vector2 q(p.y, p.x);
    q.y = mScreenSize.y - q.y; // $TODO we need to normalize these coordinates
    return q;
     */
    return p;
}

void UiCore::Notify(const UiEvent &event)
{
    if (mRootControl)
    {
        UiEvent e(event);
        e.at = InputToScreen(e.at); // $TODO we need to normalize these coordinates
        mRootControl->Notify(*this, e);
    }
}

void UiCore::Destroy(IUiControl *control)
{
    if (control)
    {
        control->DestroyChildren();
        delete control;
    }
}

static int GetVisibleChildCount(IUiControl *control)
{
    int count = 0;
    for (int i = 0, sz = control->GetChildrenCount(); i < sz; i++)
    {
        IUiControl *child = control->GetChildAt(i);
        if (child && child->IsVisible())
            count++;
    }
    return count;
}

static IUiControl *GetVisibleChild(IUiControl *control, int at)
{
    int count = 0;
    for (int i = 0, sz = control->GetChildrenCount(); i < sz; i++)
    {
        IUiControl *child = control->GetChildAt(i);
        if (child && child->IsVisible())
        {
            if (count == at)
                return child;
            count++;
        }
    }
    return 0;
}

void UiCore::LayoutLinear(IUiControl *control, bool vertical)
{
    if (!control || !control->IsCollection())
        return;
    UiBounds parentBounds(control->GetBounds());
    Vector2 start;
    int count = GetVisibleChildCount(control);
    for (int i = 0; i < count; i++)
    {
        IUiControl *child = GetVisibleChild(control, i);
        UiBounds bounds(child->GetBounds());
        Vector2 size(bounds.maximum - bounds.minimum);
        bounds.minimum = start;
        bounds.maximum = start + size;
        if (vertical)
            start.y += size.y; // LayoutVertical
        else
            start.x += size.x; // LayoutHorizontal
        child->RequestBounds(bounds);
    }
//    control->RequestBounds(ComputeBounds(control));
}

UiBounds UiCore::ComputeBounds(IUiControl *control)
{
    UiBounds bounds;
    int count = GetVisibleChildCount(control);
    for (int i = 0; i < count; i++)
    {
        IUiControl *child = GetVisibleChild(control, i);
        UiBounds childBounds(child->GetBounds());
        bounds.minimum = bounds.minimum.minimum(childBounds.minimum);
        bounds.maximum = bounds.maximum.maximum(childBounds.maximum);
    }
    return bounds;
}

void UiCore::Center(IUiControl *control, const Vector2 &at)
{
    UiBounds bounds(control->GetBounds());
    Vector2 center((bounds.minimum + bounds.maximum) * 0.5f);
    Vector2 diff(at - center);
    bounds.minimum += diff;
    bounds.maximum += diff;
    control->RequestBounds(bounds);
}

void UiCore::LayoutCircle(IUiControl *control, float radius, float offset)
{
    if (!control || !control->IsCollection())
        return;
    UiBounds parentBounds(control->GetBounds());
    Vector2 parentCenter((parentBounds.minimum + parentBounds.maximum) * 0.5f);
    int count = GetVisibleChildCount(control);
    float increment = math::TWOPIf / count;
    offset += control->GetAnimationTime();
    for (int i = 0; i < count; i++)
    {
        IUiControl *child = GetVisibleChild(control, i);
        UiBounds bounds(child->GetBounds());
        Vector2 size(bounds.maximum - bounds.minimum);
        Vector2 center((bounds.minimum + bounds.maximum) * 0.5f);
        Vector2 at(Vector2(sinf(increment * i + offset) * radius, cosf(increment * i + offset) * radius) + parentCenter);
        bounds.minimum += at - center;
        bounds.maximum += at - center;
        child->RequestBounds(bounds);
    }
    //UiBounds computed = ComputeBounds(control);
    //parentBounds.maximum = parentBounds.minimum + computed
    //control->RequestBounds(ComputeBounds(control) /*+ parentBounds.minimum*/);
}
