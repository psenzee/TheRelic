#include "LuaTable.h"

extern "C"
{
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

LuaTableHandle *LuaTableHandle::Pop(lua_State *lua)
{
    luaL_checktype(lua, -1, LUA_TTABLE);
    return new LuaTableHandle(luaL_ref(lua, LUA_REGISTRYINDEX), lua);
}

LuaTableHandle *LuaTableHandle::New(lua_State *lua)
{
    lua_newtable(lua);
    return Pop(lua);
}

LuaTableHandle *LuaTableHandle::GetGlobal(lua_State *lua, const char *name)
{
    lua_getglobal(lua, name);
    return Pop(lua);
}

void LuaTableHandle::Push()
{
    core_assert(mReference > 0);
    lua_rawgeti(mLua, LUA_REGISTRYINDEX, mTable);
    luaL_checktype(mLua, -1, LUA_TTABLE);
}

int LuaTableHandle::Size()
{
    core_assert(mReference > 0);
    lua_rawgeti(mLua, LUA_REGISTRYINDEX, mTable);
    int size = static_cast<int>(lua_objlen(mLua, -1));
    lua_pop(mLua, 1);
    return size;
}

#include "luautils.h"

void LuaTableHandle::AddFromStack()
{
    Push();
    lua_insert(mLua, -2);
    lua_rawseti(mLua, -2, static_cast<int>(lua_objlen(mLua, -2)) + 1);
    lua_pop(mLua, 1);
}

LuaTableHandle::~LuaTableHandle()
{
    core_assert(mReference == 0);
    luaL_unref(mLua, LUA_REGISTRYINDEX, mTable);
}

LuaTableHandle::LuaTableHandle(int table, lua_State *lua) : mTable(table), mLua(lua), mReference(1)
{
}