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
#include "core/core_assert.h"
#include <vector>

static void LuaWait()
{
#ifdef WIN32
    getch();
#endif
}

static bool g_hasLuaError = false;

bool HasLuaErrorOccurred()
{
    return g_hasLuaError;    
}

void LuaReportError(const char *string)
{
    printf("Error: %s\n", string);
    g_hasLuaError = true;
    LuaWait();
    core_assert(0);
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

static void LuaPushArguments(lua_State *state, const LuaValue *arguments, int count)
{
    for (const LuaValue *i = arguments, *e = arguments + count; i < e; ++i)
        i->Push(state);
}

static int LuaCompleteCall(lua_State *state, const LuaValue *arguments, int count)
{
    LuaPushArguments(state, arguments, count);
    int error = lua_resume(state, count);
    core_assert(lua_gettop(state) >= 0 && "LUA POPPED TOO MUCH FROM THE STACK!!");    
    if (error != LUA_YIELD && error != 0)
    {
        LuaReportError(lua_tostring(state, -1));
        lua_pop(state, lua_gettop(state)); // pop everything
        return error;
    }
    return error;
}

bool LuaDoesFunctionExist(lua_State *state, const char *function)
{
    LuaPushFunction(state, function);
    bool exists = lua_isnil(state, 1) == 0;
    lua_pop(state, 1);
    return exists;
}

int LuaCall(lua_State *state, const char *function, const LuaValue *arguments, int argCount)
{
    LuaPushFunction(state, function);
    bool exists = !lua_isnil(state, 1);
    if (!exists)
    {
        lua_pop(state, 1);
        printf("Unable to call function '%s' - doesn't exist!\n", function);
        return -1;
    }
    return LuaCompleteCall(state, arguments, argCount);
}

int LuaCall(lua_State *state, const char *function)
{
   return LuaCall(state, function, 0, 0);
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

int LuaCall(lua_State *state, LuaFunction function)
{
   return LuaCall(state, function, 0, 0);
}

int LuaCall(lua_State *state, LuaFunction function, const std::vector<LuaValue> &arguments)
{
    return LuaCall(state, function, &arguments[0], (int)arguments.size());
}