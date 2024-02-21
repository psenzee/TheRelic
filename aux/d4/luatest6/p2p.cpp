#include <stdio.h>
#include <stdlib.h>

extern "C"
{
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

#include <conio.h>
#include <assert.h>

#include "LuaUtil/LuaPack.h"
#include "LuaUtil/LuaInterpreter.h"
#include "LuaUtil/LuaThread.h"
#include "LuaUtil/LuaCall.h"
#include "LuaUtil/LuaNetPack.h"
#include "LuaUtil/LuaValue.h"
#include "LuaUtil/LuaUtils.h"

#include "NetDictionary.h"

#include <vector>

NetDictionary g_Dictionary;

const NetDictionary *GetDictionary()
{
    return &g_Dictionary;
}

int LuaSerialize(lua_State *state)
{
    static char data[64 * 1024];
    char *p = data;
    lua_pushvalue(state, -1);
    PackLuaValue(&p, state, LuaValue::Pop(state), GetDictionary());
    lua_pushlstring(state, data, p - data);
    return 1;
}

int LuaDeserialize(lua_State *state)
{
    luaL_checktype(state, -1, LUA_TSTRING);
    size_t size = 0;
    const char *p = lua_tolstring(state, -1, &size);
    LuaValue v = UnpackLuaValue(&p, state, GetDictionary());
    v.Push(state);
    return 1;
}

void RegisterLuaFunctions(lua_State *state)
{
    lua_register(state, "Pack",        LuaPack);
    lua_register(state, "Unpack",      LuaUnpack);    
    lua_register(state, "Serialize",   LuaSerialize);
    lua_register(state, "Deserialize", LuaDeserialize);
}

void test(LuaThread *lua)
{
    LuaTable table_Cards = lua->CreateLuaTable();
    int count = 0;
    for (int i = 0; i < 1000; i++)
    {
        LuaTable table = lua->CreateLuaTable();
        char key[1024];
        sprintf(key, "k%d", i);
        lua->SetLuaProperty(table, key, LuaValue::Int(10));
        lua->PrintStackSize();
        lua->Append(table_Cards, LuaValue::Table(table));
        lua->ReleaseLuaTable(table);
        printf("count=%d\n", count++);
        lua->Collect();
    }
}

#include "wordlist.h"

int main(int argc, const char **argv)
{
    g_Dictionary.InsertAll(wordlist);

    LuaInterpreter::CreateInstance();
    LuaInterpreter *lua = LuaInterpreter::GetInstance();
    lua->RunFile("serialization.lua");

    LuaThread *thread = lua->CreateThread(0);
    /*
    std::vector<LuaValue> args;
    LuaFunction function = lua->CreateFunctionReference("PackAndPrint");
    args.push_back(LuaValue::String("&i8&s8&f6.10&z&f"));
    args.push_back(LuaValue::Int(12));
    args.push_back(LuaValue::String("hi!"));
    args.push_back(LuaValue::Number(23.44));
    args.push_back(LuaValue::String("hi!"));
    args.push_back(LuaValue::Number(23.44));
    LuaCall(thread->GetLuaState(), function, args);
    */

    LuaInterpreter::DestroyInstance();
    return 0;
}