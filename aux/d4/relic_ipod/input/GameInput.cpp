#include "GameInput.h"

#include <stdio.h>

#include "luautil/LuaInterpreter.h"
#include "luautil/LuaCall.h"
#include "luautil/LuaValue.h"

void GameInput::GetInput()
{
    if (touched)
    {
        printf("Screen touched at (%u, %u)!\n", touch.x, touch.y);
//      resume.SetState(true);
        start.SetState(true);
        touched = false;
    }
}

void GameInput::ScreenTouch(const core::Point &at)
{ 
    touched = true; touch = at;
    enum { ARG_COUNT = 2 };
    static LuaValue arguments[ARG_COUNT];
    arguments[0].SetNumber(at.x); arguments[1].SetNumber(at.y);
    LuaCall(LuaInterpreter::GetInstance()->GetState(), "Touch", arguments, ARG_COUNT);
}