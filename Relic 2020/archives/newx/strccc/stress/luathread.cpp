/*************************************************************************************************/
/*!
    \file   luathread.cpp

    $Header: //blaze/games/Madden/2010-NG/trunk/component/franchise/stress/luathread.cpp#4 $
    $Change: 44939 $
    $DateTime: 2009/06/05 14:42:23 $

    \attention
        (c) Electronic Arts Inc. 2009
*/
/*************************************************************************************************/

/*************************************************************************************************/
/*!
    \class LuaThread

    Wraps an individual Lua thread.  Note that a Lua thread is not a thread in the OS sense, but more
    akin to a Windows (and Blaze) lightweight fiber.  As such it works very well with Blaze's fibers
    and we provide one LuaThread per connection.
*/
/*************************************************************************************************/

#include "framework/blaze.h"

#include "luathread.h"
#include "luafunctions.h"
#include "luainterpreter.h"

extern "C"
{
#include "external/lua/lua.h"
#include "external/lua/lualib.h"
#include "external/lua/lauxlib.h"
}

#include <stdlib.h>
#include <stdio.h>

#ifdef WIN32
#define strtoll  _strtoi64
#define strtoull _strtoui64
#endif

namespace Blaze
{
namespace Stress
{

// Constructor - create a table for lua state data to be associated with this
// lua 'fiber' and insert an element 'data' that points back to this instance
// of LuaThread.
LuaThread::LuaThread(LuaInterpreter &interpreter) : 
    mTableId(0), mThreadId(0), mResume(false),
    mYieldSize(0), mContainer(0), mInterpreter(interpreter)
{
    lua_State *main = interpreter.GetState();

    mState = lua_newthread(main);
    lua_pushvalue(main, -1);
    mThreadId = luaL_ref(main, LUA_REGISTRYINDEX);
    lua_rawseti(main, LUA_REGISTRYINDEX, mThreadId);

    lua_newtable(mState);
    lua_pushvalue(mState, -1);
    mTableId = luaL_ref(mState, LUA_REGISTRYINDEX);
    lua_rawseti(mState, LUA_REGISTRYINDEX, mTableId);
    lua_rawgeti(mState, LUA_REGISTRYINDEX, mTableId);
    lua_pushlightuserdata(mState, this);
    lua_setfield(mState, 1, "data");
    lua_pop(mState, 1);
}

LuaThread::~LuaThread()
{
    lua_State *main = mInterpreter.GetState();
    luaL_unref(main, LUA_REGISTRYINDEX, mThreadId);
    luaL_unref(mState, LUA_REGISTRYINDEX, mTableId);
}

// Execute() - execute a lua function by name, passing in a table
// with a light user data member 'data' pointing to this LuaThread
int LuaThread::Execute(const char *function)
{
    if (!mResume)
    {
        lua_getglobal(mState, function);
        lua_rawgeti(mState, LUA_REGISTRYINDEX, mTableId);
    }
    int error = lua_resume(mState, mResume ? mYieldSize : 1);
    assert(lua_gettop(mState) >= 0 && "LUA POPPED TOO MUCH FROM THE STACK!!");
    mResume = (error == LUA_YIELD);
    if (!mResume && (error != 0))
    {
        printf("Error: %s\n", lua_tostring(mState, -1));
        lua_pop(mState, lua_gettop(mState)); // pop everything
        // What should I do to handle an error?
        assert(0);
        return error;
    }
    if (mResume)
    {
        mYieldSize = lua_gettop(mState);
    }

    return error;
}

// GetInstanceArgument() - to be called in a Lua c++ wrapper to get the LuaInstance from 
// the Lua 'instance' table that's passed around in the stress tester
LuaThread *LuaThread::GetInstanceArgument(lua_State *state)
{
    luaL_checktype(state, 1, LUA_TTABLE);
    lua_pushvalue(state, 1);
    lua_getfield(state, -1, "data");
    LuaThread *instance = (LuaThread *)lua_touserdata(state, -1);
    lua_pop(state, 2);
    return instance;
}

LuaTable LuaThread::CreateLuaTable()
{
    lua_newtable(mState);
    return (LuaTable)luaL_ref(mState, LUA_REGISTRYINDEX);
}

void LuaThread::ReleaseLuaTable(LuaTable table)
{
    luaL_unref(mState, LUA_REGISTRYINDEX, (int)table);
}

void LuaThread::PrintStackSize()
{
    printf("stack top = %d\n", lua_gettop(mState));
}

static bool StringToInt64(const char *text, int64_t &value)
{
    const int DECIMAL_RADIX = 10;
    const char *ends = text;
    value = (int64_t)strtoll(text, const_cast<char **>(&ends), DECIMAL_RADIX);
    return (ends != NULL && ends != text);
}

static bool StringToUInt64(const char *text, uint64_t &value)
{
    const int DECIMAL_RADIX = 10;
    const char *ends = text;
    value = (uint64_t)strtoull(text, const_cast<char **>(&ends), DECIMAL_RADIX);
    return (ends != NULL && ends != text);
}

static void Int64ToString(int64_t value, char *buffer)
{
#ifdef WIN32
    sprintf(buffer, "%I64d", value);
#else
    sprintf(buffer, "%lld",  value);
#endif
}

static void UInt64ToString(uint64_t value, char *buffer)
{
#ifdef WIN32
    sprintf(buffer, "%I64u", value);
#else
    sprintf(buffer, "%llu",  value);
#endif
}

const char *LuaThread::GetLuaPropertyString(LuaTable table, const char *key)
{
    lua_rawgeti(mState, LUA_REGISTRYINDEX, (int)table);
    luaL_checktype(mState, 1, LUA_TTABLE);
    lua_getfield(mState, (int)table, key);
    luaL_checktype(mState, 1, LUA_TSTRING);
    const char *value = lua_tostring(mState, 1);
    lua_pop(mState, 2);
    return value;
}

int LuaThread::GetLuaPropertyInt(LuaTable table, const char *key)
{
    lua_rawgeti(mState, LUA_REGISTRYINDEX, (int)table);
    luaL_checktype(mState, -1, LUA_TTABLE);
    lua_getfield(mState, -1, key);
    luaL_checktype(mState, -1, LUA_TNUMBER);
    int value = (int)lua_tointeger(mState, -1);
    lua_pop(mState, 2);
    return value;
}

unsigned LuaThread::GetLuaPropertyUInt(LuaTable table, const char *key)
{
    lua_rawgeti(mState, LUA_REGISTRYINDEX, (int)table);
    luaL_checktype(mState, -1, LUA_TTABLE);
    lua_getfield(mState, -1, key);
    luaL_checktype(mState, -1, LUA_TNUMBER);
    unsigned value = (unsigned)lua_tointeger(mState, -1);
    lua_pop(mState, 2);
    return value;
}

int64_t LuaThread::GetLuaPropertyInt64(LuaTable table, const char *key)
{
    lua_rawgeti(mState, LUA_REGISTRYINDEX, (int)table);
    luaL_checktype(mState, 1, LUA_TTABLE);
    lua_getfield(mState, (int)table, key);
    luaL_checktype(mState, 1, LUA_TSTRING);
    int64_t value = 0;
    if (!StringToInt64(lua_tostring(mState, 1), value))
    {
        printf("Warning: Unable to convert Lua string to required 64 bit integer!\n");
    }
    lua_pop(mState, 2);
    return value;
}

uint64_t LuaThread::GetLuaPropertyUInt64(LuaTable table, const char *key)
{
    lua_rawgeti(mState, LUA_REGISTRYINDEX, (int)table);
    luaL_checktype(mState, 1, LUA_TTABLE);
    lua_getfield(mState, (int)table, key);
    luaL_checktype(mState, 1, LUA_TSTRING);
    uint64_t value = 0;
    if (!StringToUInt64(lua_tostring(mState, 1), value))
    {
        printf("Warning: Unable to convert Lua string to required 64 bit integer!\n");
    }
    lua_pop(mState, 2);
    return value;
}

bool LuaThread::GetLuaPropertyBool(LuaTable table, const char *key)
{
    lua_rawgeti(mState, LUA_REGISTRYINDEX, (int)table);
    luaL_checktype(mState, 1, LUA_TTABLE);
    lua_getfield(mState, (int)table, key);
    luaL_checktype(mState, 1, LUA_TBOOLEAN);
    bool value = (lua_toboolean(mState, 1) != 0);
    lua_pop(mState, 2);
    return value;
}

LuaTable LuaThread::GetLuaPropertyTable(LuaTable table, const char *key)
{
    lua_rawgeti(mState, LUA_REGISTRYINDEX, (int)table);
    luaL_checktype(mState, 1, LUA_TTABLE);
    lua_getfield(mState, (int)table, key);
    luaL_checktype(mState, 1, LUA_TTABLE);
    LuaTable value = (LuaTable)luaL_ref(mState, LUA_REGISTRYINDEX);
    lua_pop(mState, 2);
    return value;
}

void LuaThread::SetLuaPropertyString(LuaTable table, const char *key, const char *value)
{
    lua_rawgeti(mState, LUA_REGISTRYINDEX, (int)table);
    lua_pushstring(mState, value);
    lua_setfield(mState, -2, key);
    lua_pop(mState, 1);
}

void LuaThread::SetLuaPropertyInt(LuaTable table, const char *key, int value)
{
    lua_rawgeti(mState, LUA_REGISTRYINDEX, (int)table);
    lua_pushnumber(mState, value);
    lua_setfield(mState, -2, key);
    lua_pop(mState, 1);
}

void LuaThread::SetLuaPropertyUInt(LuaTable table, const char *key, unsigned value)
{
    lua_rawgeti(mState, LUA_REGISTRYINDEX, (int)table);
    lua_pushnumber(mState, value);
    lua_setfield(mState, -2, key);
    lua_pop(mState, 1);
}

void LuaThread::SetLuaPropertyInt64(LuaTable table, const char *key, int64_t value)
{
    lua_rawgeti(mState, LUA_REGISTRYINDEX, (int)table);
    char buffer[64];
    Int64ToString(value, buffer);
    lua_pushstring(mState, buffer);
    lua_setfield(mState, -2, key);
    lua_pop(mState, 1);
}

void LuaThread::SetLuaPropertyUInt64(LuaTable table, const char *key, uint64_t value)
{
    lua_rawgeti(mState, LUA_REGISTRYINDEX, (int)table);
    char buffer[64];
    UInt64ToString(value, buffer);
    lua_pushstring(mState, buffer);
    lua_setfield(mState, -2, key);
    lua_pop(mState, 1);
}

void LuaThread::SetLuaPropertyBool(LuaTable table, const char *key, bool value)
{
    lua_rawgeti(mState, LUA_REGISTRYINDEX, (int)table);
    lua_pushboolean(mState, value);
    lua_setfield(mState, -2, key);
    lua_pop(mState, 1);
}

void LuaThread::SetLuaPropertyTable(LuaTable table, const char *key, LuaTable value)
{
    lua_rawgeti(mState, LUA_REGISTRYINDEX, (int)table);
    lua_rawgeti(mState, LUA_REGISTRYINDEX, (int)value);
    lua_setfield(mState, -2, key);
    lua_pop(mState, 1);
}

void LuaThread::AppendString(LuaTable table, const char *value)
{
    lua_rawgeti(mState, LUA_REGISTRYINDEX, (int)table);
    lua_pushstring(mState, value);
    lua_rawseti(mState, -2, (int)lua_objlen(mState, -2) + 1);
    lua_pop(mState, 1);
}

void LuaThread::AppendInt(LuaTable table, int value)
{
    lua_rawgeti(mState, LUA_REGISTRYINDEX, (int)table);
    lua_pushinteger(mState, value);
    lua_rawseti(mState, -2, (int)lua_objlen(mState, -2) + 1);
    lua_pop(mState, 1);
}

void LuaThread::AppendUInt(LuaTable table, unsigned value)
{
    lua_rawgeti(mState, LUA_REGISTRYINDEX, (int)table);
    lua_pushinteger(mState, value);
    lua_rawseti(mState, -2, (int)lua_objlen(mState, -2) + 1);
    lua_pop(mState, 1);
}

void LuaThread::AppendInt64(LuaTable table, int64_t value)
{
    lua_rawgeti(mState, LUA_REGISTRYINDEX, (int)table);
    char buffer[64];
    Int64ToString(value, buffer);
    lua_pushstring(mState, buffer);
    lua_rawseti(mState, -2, (int)lua_objlen(mState, -2) + 1);
    lua_pop(mState, 1);
}

void LuaThread::AppendUInt64(LuaTable table, uint64_t value)
{
    lua_rawgeti(mState, LUA_REGISTRYINDEX, (int)table);
    char buffer[64];
    UInt64ToString(value, buffer);
    lua_pushstring(mState, buffer);
    lua_rawseti(mState, -2, (int)lua_objlen(mState, -2) + 1);
    lua_pop(mState, 1);
}

void LuaThread::AppendBool(LuaTable table, bool value)
{
    lua_rawgeti(mState, LUA_REGISTRYINDEX, (int)table);
    lua_pushboolean(mState, value);
    lua_rawseti(mState, -2, (int)lua_objlen(mState, -2) + 1);
    lua_pop(mState, 1);
}

void LuaThread::AppendTable(LuaTable table, LuaTable value)
{
    lua_rawgeti(mState, LUA_REGISTRYINDEX, (int)table);
    lua_rawgeti(mState, LUA_REGISTRYINDEX, (int)value);
    lua_rawseti(mState, -2, (int)lua_objlen(mState, -2) + 1);
    lua_pop(mState, 1);
}

    int64_t     GetLuaPropertyInt64(LuaTable table, const char *key);
    uint64_t    GetLuaPropertyUInt64(LuaTable table, const char *key);

    void        SetLuaPropertyInt64(LuaTable table, const char *key, int64_t value);
    void        SetLuaPropertyUInt64(LuaTable table, const char *key, uint64_t value);

    void        AppendInt64(LuaTable table, int64_t value);
    void        AppendUInt64(LuaTable table, uint64_t value);

// SetContainer() - sets the opaque pointer to the instance of
// the module in the stress tester that is using this LuaThread.
void LuaThread::SetContainer(void *container)
{
    mContainer = container;
}

// GetContainer() - returns the opaque pointer to the instance of
// the module in the stress tester that is using this LuaThread.
void *LuaThread::GetContainer()
{
    return mContainer;
}

LuaTableIterator::LuaTableIterator(lua_State *state, LuaTable table) : mState(state), mHasNext(true)
{        
    lua_rawgeti(mState, LUA_REGISTRYINDEX, (int)table); // here we need to push the table onto the stack (at -2)
    lua_pushnil(mState);                                // this is required to prime the iteration       (at -1)
    mHasNext = lua_next(mState, -2) != 0;
}

bool LuaTableIterator::HasNext() const
{
    return mHasNext;    
}

int LuaTableIterator::NextInt()
{
    luaL_checktype(mState, -1, LUA_TNUMBER);
    int value = (int)lua_tointeger(mState, -1);
    lua_pop(mState, 1);
    mHasNext = lua_next(mState, -2) != 0;        
    return value;
}

bool LuaTableIterator::NextBool()
{
    luaL_checktype(mState, -1, LUA_TBOOLEAN);
    int value = lua_toboolean(mState, -1) != 0;
    lua_pop(mState, 1);
    mHasNext = lua_next(mState, -2) != 0;
    return value != 0;
}

const char *LuaTableIterator::NextString()
{
    luaL_checktype(mState, -1, LUA_TSTRING);
    const char *value = lua_tostring(mState, -1);
    lua_pop(mState, 1);
    mHasNext = lua_next(mState, -2) != 0;
    return value;
}

LuaTable LuaTableIterator::NextTable()
{
    luaL_checktype(mState, -1, LUA_TTABLE);
    LuaTable table = (LuaTable)luaL_ref(mState, -1);
    mHasNext = lua_next(mState, -2) != 0;
    return table;
}

} // Stress
} // Blaze
