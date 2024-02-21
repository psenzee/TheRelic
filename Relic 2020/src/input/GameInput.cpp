#include "GameInput.h"

#include <stdio.h>

#include "luautil/LuaInterpreter.h"
#include "luautil/LuaCall.h"
#include "luautil/LuaValue.h"

static GameInput *INSTANCE = NULL;
GameInput *GameInput::GetInstance()
{
    return INSTANCE;
}

GameInput::GameInput() : enabled(false), move(Vector2(1.0f, 1.0f), Vector2(200.0f, 200.0f)),
    camera(Vector2(1.0f, 1.0f), Vector2(1.0f, 1.0f), Vector4(0.1f, 100.0f, 0.1f, 100.0f)),
    touched(false), touch(-1, -1)
{
    memset(flags, 0, sizeof(flags));
    transform.identity();
    transform.set(0.f,  -1.f, 0.f, 0.f,
                  1.f,   0.f, 0.f, 0.f,
                  0.f,   0.f, 1.f, 0.f,
                -44.f, 320.f, 0.f, 1.f);
    if (!INSTANCE) {
        INSTANCE = this;
    }
}

void GameInput::GetInput()
{
    if (touched)
    {
        start.SetState(true);
        touched = false;
    }
}

const Vector3 GameInput::GetTouchPoint(TouchAction ta) const
{
    return Vector3(float(points[ta].x), float(points[ta].y), 1.f);
}

const Vector3 GameInput::GetTransformedTouchPoint(TouchAction ta) const
{
    return transform * GetTouchPoint(ta);
}

void GameInput::ScreenTouch(int index, const core::Point &at, GameInput::TouchAction ta, const char *function)
{
    touched = true; touch = at;
    enum { ARG_COUNT = 3 };
    Vector3 value = GetTransformedTouchPoint(ta);
    static LuaValue arguments[ARG_COUNT];
    arguments[0].SetNumber(index); arguments[1].SetNumber(value.x); arguments[2].SetNumber(value.y);
    LuaCall(LuaInterpreter::GetInstance()->GetState(), function, arguments, ARG_COUNT);
}

void GameInput::SetTouchAction(TouchAction action, int index, const core::Point &at)
{
    flags[action] = true;
    points[action] = at;
}

void GameInput::ScreenTouchBegan(int index, const core::Point &at)
{
    SetTouchAction(TouchAction_Began, index, at);
    ScreenTouch(index, at, TouchAction_Began, "TouchBegan");
}

void GameInput::ScreenTouchMoved(int index, const core::Point &at)
{
    SetTouchAction(TouchAction_Moved, index, at);
    ScreenTouch(index, at, TouchAction_Moved, "TouchMoved");
}

void GameInput::ScreenTouchEnded(int index, const core::Point &at)
{
    SetTouchAction(TouchAction_Ended, index, at);
    ScreenTouch(index, at, TouchAction_Ended, "TouchEnded");
}

void GameInput::ScreenTouchStationary(int index, const core::Point &at)
{ 
    ScreenTouch(index, at, TouchAction_Stationary, "TouchStationary");
}

void GameInput::ClearTouches()
{
    for (int i = 0; i < TouchAction_MAX; i++)
    {
        flags[i] = false;
        points[i] = core::Point(0, 0);
    }
}
