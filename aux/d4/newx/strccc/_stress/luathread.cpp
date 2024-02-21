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

namespace Blaze
{
namespace Stress
{

int LuaThread::mTableCount = 0;

// Constructor - create a table for lua state data to be associated with this
// lua 'fiber' and insert an element 'data' that points back to this instance
// of LuaThread.
LuaThread::LuaThread(LuaInterpreter &interpreter) : 
    mTableId(0), mThreadId(0), mResume(false),
    mYieldSize(0), mContainer(0), mInterpreter(interpreter)
{
    mTableId  = mTableCount++;
    mThreadId = mTableCount++;
    lua_State *main = interpreter.GetState();

    mState = lua_newthread(main);
    lua_rawseti(main, LUA_REGISTRYINDEX, mThreadId);

    lua_newtable(mState);
    lua_rawseti(mState, LUA_REGISTRYINDEX, mTableId);
    lua_rawgeti(mState, LUA_REGISTRYINDEX, mTableId);
    lua_pushlightuserdata(mState, this);
    lua_setfield(mState, 1, "data");
    lua_pop(mState, 1);
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

} // Stress
} // Blaze
