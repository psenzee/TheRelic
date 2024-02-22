#include "LuaThread.h"
#include "LuaInterpreter.h"

#include "core/core_assert.h"

extern "C"
{
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

#include <stdlib.h>
#include <stdio.h>

#ifdef WIN32
#define strtoll  _strtoi64
#define strtoull _strtoui64
#endif

#ifdef WIN32
#include <conio.h>
static void wait() { getch(); }
#else
static void wait() {}
#endif

extern void LuaReportError(const char *string);

// Constructor - create a table for lua state data to be associated with this
// lua 'fiber' and insert an element 'data' that points back to this instance
// of LuaThread.
LuaThread::LuaThread(LuaInterpreter &interpreter) : 
    mTableId(0), mThreadId(0), mResume(false),
    mYieldSize(0), mContainer(0), mInterpreter(interpreter), mUserData(LUA_NOREF)
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

LuaThread::LuaThread(LuaThread *other) : 
    mTableId(0), mThreadId(other->mThreadId), mResume(other->mResume),
    mYieldSize(0), mContainer(0), mInterpreter(other->mInterpreter), mUserData(LUA_NOREF)
{
    lua_State *main = mInterpreter.GetState();
    mState = other->GetLuaState();

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
    if (mUserData != LUA_NOREF)
        luaL_unref(main, LUA_REGISTRYINDEX, mUserData);
    luaL_unref(mState, LUA_REGISTRYINDEX, mTableId);
    Collect();
}

void LuaThread::Collect()
{
    lua_gc(mState, LUA_GCCOLLECT, 0);
}

// Execute() - execute a lua function by name, passing in a table
// with a light user data member 'data' pointing to this LuaThread
int LuaThread::Execute(const char *function)
{
    if (!mResume)
    {
        lua_getglobal(mState, function);
        if (mUserData == LUA_NOREF)
            lua_rawgeti(mState, LUA_REGISTRYINDEX, mTableId);
        else
        {
            lua_rawgeti(mInterpreter.GetState(), LUA_REGISTRYINDEX, mUserData);
            lua_xmove(mInterpreter.GetState(), mState, 1);
        }
    }
    int error = lua_resume(mState, mResume ? mYieldSize : 1);
    core_assert(lua_gettop(mState) >= 0 && "LUA POPPED TOO MUCH FROM THE STACK!!");
    mResume = (error == LUA_YIELD);
    if (!mResume && (error != 0))
    {
        LuaReportError(lua_tostring(mState, -1));
        lua_pop(mState, lua_gettop(mState)); // pop everything
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
/*
LuaTable LuaThread::CreateLuaTable()
{
    lua_newtable(mState);
    return (LuaTable)luaL_ref(mState, LUA_REGISTRYINDEX);
}

int LuaThread::GetLuaTableSize(LuaTable table)
{
    lua_rawgeti(mState, LUA_REGISTRYINDEX, static_cast<int>(table));
    int size = static_cast<int>(lua_objlen(mState, -1));
    lua_pop(mState, 1);
    return size;
}

void LuaThread::ReleaseLuaTable(LuaTable table)
{
    luaL_unref(mState, LUA_REGISTRYINDEX, static_cast<int>(table));
}
*/

void LuaThread::PrintStackSize()
{
    printf("stack top = %d\n", lua_gettop(mState));
}
/*
LuaValue LuaThread::GetLuaProperty(LuaTable table, const char *key)
{
    lua_rawgeti(mState, LUA_REGISTRYINDEX, static_cast<int>(table));
    luaL_checktype(mState, -1, LUA_TTABLE);
    lua_getfield(mState, -1, key);
    LuaValue value(LuaValue::Pop(mState));
    lua_pop(mState, 1);
    return value;
}

void LuaThread::SetLuaProperty(LuaTable table, const char *key, const LuaValue &value)
{
    SetPair(table, key, value);
}

void LuaThread::Append(LuaTable table, const LuaValue &value)
{
    lua_rawgeti(mState, LUA_REGISTRYINDEX, static_cast<int>(table));
    luaL_checktype(mState, 1, LUA_TTABLE);
    value.Push(mState);
    lua_rawseti(mState, -2, static_cast<int>(lua_objlen(mState, -2)) + 1);
    lua_pop(mState, 1);
}

void LuaThread::SetPair(LuaTable table, const LuaValue &key, const LuaValue &value)
{
    std::string keyString(key.ToString(mState));
    SetPair(table, keyString.c_str(), value);
}

void LuaThread::SetPair(LuaTable table, const char *key, const LuaValue &value)
{
    lua_rawgeti(mState, LUA_REGISTRYINDEX, static_cast<int>(table));
    value.Push(mState);
    lua_setfield(mState, -2, key);
    lua_pop(mState, 1);
}

void LuaThread::SetPair(LuaTable table, int key, const LuaValue &value)
{
    lua_rawgeti(mState, LUA_REGISTRYINDEX, static_cast<int>(table));
    value.Push(mState);
    lua_rawseti(mState, -2, key);
}
*/

//! SetContainer() - sets the opaque pointer to the instance of
/// the module in the stress tester that is using this LuaThread.
void LuaThread::SetContainer(void *container)
{
    mContainer = container;
}

//! GetContainer() - returns the opaque pointer to the instance of
/// the module in the stress tester that is using this LuaThread.
void *LuaThread::GetContainer()
{
    return mContainer;
}