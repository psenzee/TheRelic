#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

extern "C"
{
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

#include "LuaUtils.h"
#include "LuaThread.h"

const char *GetString(lua_State *lua, int negativeOffset)
{
    luaL_checktype(lua, negativeOffset, LUA_TSTRING); // type
    return (const char *)lua_tostring(lua, negativeOffset);
}

void *GetLightUserData(lua_State *lua, int negativeOffset)
{
    luaL_checktype(lua, -1, LUA_TLIGHTUSERDATA);
    return lua_touserdata(lua, -4);    
}

void *GetFullUserData(lua_State *lua, int negativeOffset)
{
    luaL_checktype(lua, -1, LUA_TUSERDATA);
    return lua_touserdata(lua, -4);    
}

float GetFloat(lua_State *lua, int negativeOffset)
{
    luaL_checktype(lua, negativeOffset, LUA_TNUMBER); // type
    return (float)lua_tonumber(lua, negativeOffset);
}

int GetInt(lua_State *lua, int negativeOffset)
{
    luaL_checktype(lua, negativeOffset, LUA_TNUMBER); // type
    return (int)lua_tointeger(lua, negativeOffset);
}

bool GetBool(lua_State *lua, int negativeOffset)
{
    luaL_checktype(lua, negativeOffset, LUA_TBOOLEAN); // type
    return lua_toboolean(lua, negativeOffset) != 0;
}

Vector3 GetVector3(lua_State *lua, int negativeOffset)
{
    enum {  OFFSET_X = -2, OFFSET_Y = -1, OFFSET_Z = 0 };

    luaL_checktype(lua, OFFSET_X + negativeOffset, LUA_TNUMBER); // x
    luaL_checktype(lua, OFFSET_Y + negativeOffset, LUA_TNUMBER); // y
    luaL_checktype(lua, OFFSET_Z + negativeOffset, LUA_TNUMBER); // z

    return Vector3((float)lua_tonumber(lua, OFFSET_X + negativeOffset),
                   (float)lua_tonumber(lua, OFFSET_Y + negativeOffset),
                   (float)lua_tonumber(lua, OFFSET_Z + negativeOffset));
}

void ReturnVector3(lua_State *lua, const Vector3 &v)
{
    lua_pushnumber(lua, v.x);
    lua_pushnumber(lua, v.y);
    lua_pushnumber(lua, v.z);
}

void PrintStack(lua_State *lua, int direction)
{
    int count = lua_gettop(lua);
    printf("\n");
    for (int i = 1; i <= count; i++)    
        printf("%d typename=%s\n", direction * i, lua_typename(lua, lua_type(lua, direction * i)));
    printf("\n");
}