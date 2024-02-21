#ifndef _LUANETPACK_H
#define _LUANETPACK_H

#include "NetPack.h"
#include "LuaValue.h"

class  NetDictionary;
struct lua_State;

void     PackLuaValue(char **to, lua_State *lua, const LuaValue &value, const NetDictionary *dictionary);
LuaValue UnpackLuaValue(const char **from, lua_State *lua, const NetDictionary *dictionary);

void     PackLuaTable(char **to, lua_State *lua, LuaTable table, const NetDictionary *dictionary);
LuaTable UnpackLuaTable(const char **from, lua_State *lua, const NetDictionary *dictionary);

#endif  // _LUANETPACK_H