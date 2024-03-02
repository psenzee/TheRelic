#include "UiInstance.h"
#include "render/RenderContext.h"
#include "render/OverheadCamera.h"
#include "render/GraphicsDevice.h"
#include "render/RenderSet.h"
#include "game/GameState.h"
#include "ui/UiCore.h"

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
    uiCamera.SetFovDegrees(30.0f);
    uiCamera.SetTilt(Vector2(0.f, 0.f));
    RenderContext rc(context.device, uiCamera);
    SetGlobalRenderContext(rc);
    GraphicsDevice::GetInstance()->SetProjection(uiCamera.GetProjection());
    GraphicsDevice::ClearZBuffer();
    GetLuaThread()->Execute("UiUpdateAndRender");
    GameState *state = GameState::GetInstance();
    state->GetUiCore()->Update();
    state->GetUiCore()->Render();
    GraphicsDevice::GetInstance()->SetProjection(context.camera.GetProjection());
    SetGlobalRenderContext(context);
}
