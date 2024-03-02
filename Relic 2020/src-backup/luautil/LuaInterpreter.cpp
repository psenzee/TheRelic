#include "LuaInterpreter.h"
#include "LuaThread.h"
#include "fast/Allocator.h"

extern "C"
{
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

#include "core/core_assert.h"

#ifdef WIN32
#include <conio.h>
static void wait() { getch(); }
#else
static void wait() {}
#endif

static int LuaWait(lua_State *state)
{
    wait();
    return 0;
}

extern void RegisterLuaFunctions(lua_State *state);
extern int  luaopen_Character(lua_State *L);
extern void LuaReportError(const char *string);

LuaInterpreter::LuaInterpreter() : mState(0), mThreadCount(0), mLastThreadCreated(0), mShareThreads(false)
{   
    mState = //lua_open();
        lua_newstate (lua_soa_allocate, 0);

    // load the libs  
    luaL_openlibs(mState);  
    luaopen_Character(mState);
    lua_register(mState, "Wait", LuaWait);

    RegisterLuaFunctions(mState);
}

void LuaInterpreter::SetGlobalData(const char *name, void *data)
{
    if (mState)
    {
        lua_pushlightuserdata(mState, data);
        lua_setglobal(mState, name);
    }
}

LuaFunction LuaInterpreter::CreateFunctionReference(const char *name)
{
    if (mState)
    {
        lua_getglobal(mState, name);
        return (LuaFunction)luaL_ref(mState, LUA_REGISTRYINDEX);
    }
    return 0;
}

void LuaInterpreter::ReleaseFunctionReference(LuaFunction function)
{
    luaL_unref(mState, LUA_REGISTRYINDEX, static_cast<int>(function));
}

// GetState() returns the underlying lua_State instance.
lua_State *LuaInterpreter::GetState()
{
    return mState;
}

// CreateThread() creates a new lua 'fiber' and initializes it.
LuaThread *LuaInterpreter::CreateThread(void *userContainer)
{
    LuaThread *thread = 0;
    if (mState)
    {
        if (mShareThreads && mThreadCount >= 2)
        {
            thread = new LuaThread(mLastThreadCreated);
        }
        else
        {
            thread = new LuaThread(*this);
            mLastThreadCreated = thread;
            mThreadCount++;
        }
        thread->SetContainer(userContainer);        
    }
    return thread;
}

void LuaInterpreter::DestroyThread(LuaThread *thread)
{
    delete thread;
}

static void lua_message(const char *pname, const char *msg)
{
    if (pname) fprintf(stderr, "%s: ", pname);
    fprintf(stderr, "%s\n", msg);
    fflush(stderr);
}

static int lua_report(lua_State *L, int status)
{
    if (status && !lua_isnil(L, -1))
    {
        const char *msg = lua_tostring(L, -1);
        if (msg == NULL) msg = "(error object is not a string)";
        lua_message(0, msg);
        lua_pop(L, 1);
    }
    return status;
}

LuaInterpreter *LuaInterpreter::mInstance = 0;

// RunFile() initializes, runs the lua script and sets LuaStats::SetLuaFile(..)
bool LuaInterpreter::RunFile(const char *filename)
{
    if (!filename)
        return false;

    if (luaL_dofile(mState, filename) != 0)
    {
        int top = lua_gettop(mState);
        LuaReportError(lua_tostring(mState, -1));
        lua_pop(mState, lua_gettop(mState));
        //what should I do if the script is invalid?
        //how do I terminate?
        return false;
    }
    return true;
}

LuaInterpreter::~LuaInterpreter()
{
    if (mState)
    {
        // Verify the stack
        core_assert(lua_gettop(mState) == 0);
        lua_close(mState);  
    }
}
