#ifndef _LUAFUNCTIONS_H
#define _LUAFUNCTIONS_H

struct lua_State;

// this is called to register all lua functions with the lua interpreter
void RegisterLuaFunctions(lua_State *lua);  // outside the Blaze::Stress::Lua namespace

#endif // _LUAFUNCTIONS_H
