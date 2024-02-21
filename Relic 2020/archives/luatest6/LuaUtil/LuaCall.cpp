#include <stdio.h>
#include <stdlib.h>

extern "C"
{
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

#include "LuaInterpreter.h"
#include "LuaValue.h"

#ifdef WIN32
#include <conio.h>
#endif
#include <assert.h>
#include <vector>

static void LuaWait()
{
#ifdef WIN32
    getch();
#endif
}

static void LuaPushFunction(lua_State *state, const char *function)
{
    lua_getglobal(state, function);
}

static void LuaPushFunction(lua_State *state, LuaFunction function)
{
    lua_State *globalState = LuaInterpreter::GetInstance()->GetState();
    lua_rawgeti(globalState, LUA_REGISTRYINDEX, function);
    if (globalState != state)
        lua_xmove(globalState, state, 1);
}

/*
static void LuaPushArguments(lua_State *state, const std::vector<LuaValue> &arguments)
{
    for (std::vector<LuaValue>::const_iterator i = arguments.begin(), e = arguments.end(); i != e; ++i)
        (*i).Push(state);
}
*/

static void LuaPushArguments(lua_State *state, const LuaValue *arguments, int count)
{
    for (const LuaValue *i = arguments, *e = arguments + count; i < e; ++i)
        i->Push(state);
}

static int LuaCompleteCall(lua_State *state, const LuaValue *arguments, int count)
{
    LuaPushArguments(state, arguments, count);
    int error = lua_resume(state, count);
    assert(lua_gettop(state) >= 0 && "LUA POPPED TOO MUCH FROM THE STACK!!");    
    if (error != LUA_YIELD && error != 0)
    {
        printf("Error: %s\n", lua_tostring(state, -1));
        lua_pop(state, lua_gettop(state)); // pop everything
        // What should I do to handle an error?
        LuaWait();
        assert(0);
        return error;
    }
    return error;
}

int LuaCall(lua_State *state, const char *function, const LuaValue *arguments, int argCount)
{
    LuaPushFunction(state, function);
    return LuaCompleteCall(state, arguments, argCount);
}

int LuaCall(lua_State *state, const char *function, const std::vector<LuaValue> &arguments)
{
    return LuaCall(state, function, &arguments[0], (int)arguments.size());
}

int LuaCall(lua_State *state, LuaFunction function, const LuaValue *arguments, int argCount)
{
    LuaPushFunction(state, function);
    return LuaCompleteCall(state, arguments, argCount);
}

int LuaCall(lua_State *state, LuaFunction function, const std::vector<LuaValue> &arguments)
{
    return LuaCall(state, function, &arguments[0], (int)arguments.size());
}