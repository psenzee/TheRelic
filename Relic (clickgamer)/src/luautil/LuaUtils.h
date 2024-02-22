#ifndef _LUAUTILS_H
#define _LUAUTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

extern "C"
{
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

typedef int LuaRef;

#include "core/core.h"
#include "LuaThread.h"

const char *GetString(lua_State *lua, int negativeOffset);
float       GetFloat(lua_State *lua, int negativeOffset);
int         GetInt(lua_State *lua, int negativeOffset);
bool        GetBool(lua_State *lua, int negativeOffset);
void       *GetFullUserData(lua_State *lua, int negativeOffset);
LuaRef      GetFullUserDataRef(lua_State *lua, int negativeOffset);
void       *GetLightUserData(lua_State *lua, int negativeOffset);
Vector2     GetVector2(lua_State *lua, int negativeOffset);
Vector3     GetVector3(lua_State *lua, int negativeOffset);
Vector4     GetVector4(lua_State *lua, int negativeOffset);
void        ReturnVector2(lua_State *lua, const Vector2 &v);
void        ReturnVector3(lua_State *lua, const Vector3 &v);
void        ReturnVector4(lua_State *lua, const Vector4 &v);

void        PrintStack(lua_State *lua, int direction = 1);

#endif // _LUAUTILS_H