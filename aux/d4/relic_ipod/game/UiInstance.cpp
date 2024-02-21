#include "UiInstance.h"
#include "render/RenderContext.h"
#include "render/OverheadCamera.h"
//#include "render/QuadRenderer.h"
//#include "game/GameState.h"

extern void SetGlobalRenderContext(const RenderContext &rc);

UiInstance::UiInstance() 
  : mLuaThread(0), mLuaReference(0), mLuaTable(LuaTable::Null())
{
    mLuaThread = LuaInterpreter::GetInstance()->CreateThread(this);
}

UiInstance::~UiInstance()
{
    LuaInterpreter::GetInstance()->DestroyThread(mLuaThread);
}

void UiInstance::UpdateAndRender(const GameTime &time, RenderContext &context)
{
    OverheadCamera uiCamera(context.device, 90.f);
    uiCamera.SetDepthScale(1.0f);
    uiCamera.SetLookAt(Vector3(0.f, 0.f, 0.f));
    uiCamera.SetFovDegrees(90.0f);
    uiCamera.SetTilt(Vector2(0.f, 0.f));
    RenderContext rc(context.device, uiCamera);
    SetGlobalRenderContext(rc);
//    state->quads->SetRenderContext(rc);
    GetLuaThread()->Execute("UiUpdateAndRender");
    SetGlobalRenderContext(context);
//    state->quads->SetRenderContext(context);
}