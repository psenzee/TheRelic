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

lua_datastring_t GetDataString(lua_State *lua, int negativeOffset)
{
    luaL_checktype(lua, negativeOffset, LUA_TSTRING);
    lua_datastring_t data;
    size_t size[3] = { 0, 0, 0 };
    // ^ this is to give the return value some space around it on the stack,
    // because otherwise lua_tolstring seems to overwrite neighboring values
    data.str = lua_tolstring(lua, negativeOffset, (size_t *)&size[1]);
    data.size = size[1];
    return data;
}

const char *GetString(lua_State *lua, int negativeOffset)
{
    luaL_checktype(lua, negativeOffset, LUA_TSTRING); // type
    return (const char *)lua_tostring(lua, negativeOffset);
}

void *GetLightUserData(lua_State *lua, int negativeOffset)
{
    luaL_checktype(lua, negativeOffset, LUA_TLIGHTUSERDATA);
    return lua_touserdata(lua, negativeOffset);
}

void *GetFullUserData(lua_State *lua, int negativeOffset)
{
    luaL_checktype(lua, negativeOffset, LUA_TUSERDATA);
    return lua_touserdata(lua, negativeOffset);
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

Vector2 GetVector2(lua_State *lua, int negativeOffset)
{
    enum { OFFSET_X = -1, OFFSET_Y = 0 };

    luaL_checktype(lua, OFFSET_X + negativeOffset, LUA_TNUMBER); // x
    luaL_checktype(lua, OFFSET_Y + negativeOffset, LUA_TNUMBER); // y

    return Vector2((float)lua_tonumber(lua, OFFSET_X + negativeOffset),
                   (float)lua_tonumber(lua, OFFSET_Y + negativeOffset));
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

Vector4 GetVector4(lua_State *lua, int negativeOffset)
{
    enum {  OFFSET_X = -3, OFFSET_Y = -2, OFFSET_Z = -1, OFFSET_W = 0 };

    luaL_checktype(lua, OFFSET_X + negativeOffset, LUA_TNUMBER); // x
    luaL_checktype(lua, OFFSET_Y + negativeOffset, LUA_TNUMBER); // y
    luaL_checktype(lua, OFFSET_Z + negativeOffset, LUA_TNUMBER); // z
    luaL_checktype(lua, OFFSET_W + negativeOffset, LUA_TNUMBER); // w

    return Vector4((float)lua_tonumber(lua, OFFSET_X + negativeOffset),
                   (float)lua_tonumber(lua, OFFSET_Y + negativeOffset),
                   (float)lua_tonumber(lua, OFFSET_Z + negativeOffset),
                   (float)lua_tonumber(lua, OFFSET_W + negativeOffset));
}

void ReturnVector2(lua_State *lua, const Vector2 &v)
{
    lua_pushnumber(lua, v.x);
    lua_pushnumber(lua, v.y);
}

void ReturnVector3(lua_State *lua, const Vector3 &v)
{
    lua_pushnumber(lua, v.x);
    lua_pushnumber(lua, v.y);
    lua_pushnumber(lua, v.z);
}

void ReturnVector4(lua_State *lua, const Vector4 &v)
{
    lua_pushnumber(lua, v.x);
    lua_pushnumber(lua, v.y);
    lua_pushnumber(lua, v.z);
    lua_pushnumber(lua, v.w);
}

void PrintStack(lua_State *lua, int direction)
{
    int count = lua_gettop(lua);
    printf("\n");
    for (int i = 1; i <= count; i++)    
        printf("%d typename=%s\n", direction * i, lua_typename(lua, lua_type(lua, direction * i)));
    printf("\n");
}
