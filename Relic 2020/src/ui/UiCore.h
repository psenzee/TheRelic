#ifndef _UICORE_H
#define _UICORE_H

#include "core/core.h"
#include "Ui.h"
#include "GameDimensions.h"

class  GameUI;
class  QuadRenderer;
class  EventDispatcher;
class  IUiControl;
class  RenderContext;
class  XmlReadContext;
struct UiEvent;
struct UiBounds;

class UiCore
{
public:
    
    UiCore(const GameDimensions &dimensions, GameUI &gameUi, QuadRenderer &quads, EventDispatcher &dispatcher);
   
    void             SetRootControl(IUiControl *control);
    IUiControl      *GetRootControl();

    Vector2          InputToScreen(const Vector2 &p) const;

    IUiControl      *CreateText(const char *name, const char *text, const UiBounds &bounds, float alpha, float size, bool center);
    IUiControl      *CreateDialogText(const char *name, const char *text, const UiBounds &bounds, float alpha, float size, bool center);
    IUiControl      *CreateButton(const char *name, const char *filename, const char *text, UiTextJustify justify, const UiBounds &bounds, float alpha, float size);
    IUiControl      *CreateImageButton(const char *name, const char *filename, const char *text, UiTextJustify justify, const UiBounds &bounds, float alpha, float size, const Vector2 &t0, const Vector2 &t1, float angle);
    IUiControl      *CreateMenu(const char *name, const char *filename, const char *text, const UiBounds &bounds, float alpha, float size);
    IUiControl      *CreateRenderableButton(const char *name, const char *item, const char *text, UiTextJustify justify, const UiBounds &bounds, float alpha, float size, float scale, float angle);

    void             Notify(const UiEvent &event);
    void             Render();
    void             Update();

    // ----------

    RenderContext   &GetRenderContext();
    XmlReadContext  *GetRenderables()                              { return mRenderables; }
    void             SetRenderables(XmlReadContext *renderables)   { mRenderables = renderables; }
    
    EventDispatcher &GetEventDispatcher()                          { return mDispatcher; }
    GameUI          &GetGameUi()                                   { return mGameUi; }

    static void      Destroy(IUiControl *control);
    static UiBounds  GetAbsoluteBounds(IUiControl *control);
    static void      LayoutLinear(IUiControl *control, bool vertical);
    static void      LayoutCircle(IUiControl *control, float radius, float offset = 0.f);

    inline Vector2   DrawToScreenPosition(const Vector2 &at) const { return mScreenSize - ((at / mDrawToScreenSizeRatio) + mScreenSize * 0.5f); }
    inline Vector2   ScreenToDrawPosition(const Vector2 &at) const { return ((mScreenSize - at) - mScreenSize * 0.5f) * mDrawToScreenSizeRatio; }
    inline Vector2   FullScreenToDrawPosition(const Vector2 &at) const { return ((mDimensions.GetBounds().size().xy() - at) - mScreenSize * 0.5f) * mDrawToScreenSizeRatio + mDimensions.GetBounds().minimum.xy(); }
    inline Vector2   ScreenToDrawSize(const Vector2 &size)   const { return size * mDrawToScreenSizeRatio; }    
    /*
    void             DrawString(const char *text, const Vector2 &at, float alpha, float size);
    void             DrawStringGlow(const char *text, const Vector2 &at, float alpha, float size);
    void             DrawStringCentered(const char *text, const Vector2 &at, float alpha, float size);
    void             DrawStringCenteredGlow(const char *text, const Vector2 &at, float alpha, float size);
    */
    void             DrawString(const char *text, const Vector2 &at, UiTextJustify justify, float alpha, float size);
    void             DrawStringGlow(const char *text, const Vector2 &at, UiTextJustify justify, float alpha, float size);

    Vector2          GetStringSize(const char *text, float scale);

    void             DrawQuad(
                         const char *filename, const Vector4 &color,
                         const Vector2 &start, const Vector2 &end,
                         const Vector2 &t0,    const Vector2 &t1, float angle = 0.f);
    void             DrawScreenQuad(
                         const char *filename, const Vector4 &color,
                         const Vector2 &t0,    const Vector2 &t1);

    void             DrawRing(
						 const char *filename, const Vector4 &color, const Vector3 &center,
					     float innerRadius, float outerRadius, float angle = 0.f);

    void             DrawPanorama(
                         const char *filename, const Vector4 &color,
                         const Vector2 &start, const Vector2 &end,
                         float angle = 0.f);
    void             DrawScreenPanorama(const char *filename, const Vector4 &color, float offset, float angle = 0.f);
    
    void             Draw9SliceQuad(
                         const char *filename, const Vector4 &color,
                         const Vector2 &start, const Vector2 &end,
                         float cornerSize, float middleRange);
    void             DrawButton(
                         const char *filename, const char *text, UiTextJustify justify,
                         float alpha, float size,
                         const Vector2 &start, const Vector2 &end,
                         float cornerSize, float middleRange);
    void             DrawImageButton(
                         const char *filename, const char *text, UiTextJustify justify,
                         float alpha, float size,
                         const Vector2 &start, const Vector2 &end,
                         const Vector2 &t0, const Vector2 &t1, float angle = 0.f);
    void             DrawRenderable(
                         const char *item, const Vector3 &at,
                         float scale = 1.f, float angle = 0.f);

    static UiBounds  ComputeBounds(IUiControl *control);
    static void      Center(IUiControl *control, const Vector2 &at);

private:

    IUiControl      *mRootControl;

    GameUI          &mGameUi;
    QuadRenderer    &mQuadRenderer;
    Vector2          mDrawLeftTop;
    Vector2          mDrawRightBottom;
    Vector2          mDrawSize;
    Vector2          mScreenSize;
    Vector2          mDrawToScreenSizeRatio;
    EventDispatcher &mDispatcher;
    XmlReadContext  *mRenderables;
    GameDimensions   mDimensions;
};

#endif // _UICORE_H
