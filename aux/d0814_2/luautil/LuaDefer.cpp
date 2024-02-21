#include "core/core.h"
#include "time/Timer.h"
#include "LuaValue.h"
#include "LuaCall.h"

#include <string.h>

extern "C"
{
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

struct LuaDeferredCall
{
    enum { ARGS = 16, FUNCTION_NAME_SIZE = 128 };
    
    char        function[FUNCTION_NAME_SIZE];
    LuaValue    arguments[ARGS];
    int         argCount, 
                time;
    
    inline LuaDeferredCall(const char *functionName, const LuaValue *args, int argCount, int time) : argCount(argCount), time(time)
    {
        memset(function, 0, sizeof(function));
        if (functionName)
            strcpy(function, functionName);
        for (int i = 0; i < argCount; i++)
            arguments[i] = args[i];
    }
};

bool ComparePredicateEq(int a, int b)   { return a == b; }
bool ComparePredicateGrEq(int a, int b) { return a >= b; }

class LuaDeferrer
{
public:

    typedef bool (*ComparePredicate)(int, int);

    LuaDeferrer(ComparePredicate comparator = ComparePredicateGrEq) : mComparePredicate(comparator) {}

    void DeferCall(const char *function, LuaValue *args, int argCount, int time)
    {
        mCalls.push_back(LuaDeferredCall(function, args, argCount, time));
    }

    void Update(lua_State *lua, int time)
    {
        if (!AnyToUpdate(time))
            return;
        std::vector<LuaDeferredCall> calls, copy;
        for (std::vector<LuaDeferredCall>::iterator i = mCalls.begin(), e = mCalls.end(); i != e; ++i)
        {
            LuaDeferredCall &call = *i;
            if (mComparePredicate(time, call.time))
                calls.push_back(call);
            else
                copy.push_back(call);
        }
        if (mCalls.size() != copy.size()) // don't copy if no change
            mCalls = copy;
        for (std::vector<LuaDeferredCall>::iterator i = calls.begin(), e = calls.end(); i != e; ++i)
        {
            LuaDeferredCall &call = *i;
            LuaCall(lua, call.function, call.arguments, call.argCount);
        }
    }

private:

    bool AnyToUpdate(int time) const
    {
        if (mCalls.empty())
            return false;
        int count = 0;
        for (std::vector<LuaDeferredCall>::const_iterator i = mCalls.begin(), e = mCalls.end(); i != e; ++i)
            if (mComparePredicate(time, (*i).time)) count++;
        return count != 0;
    }

    std::vector<LuaDeferredCall> mCalls;
    ComparePredicate             mComparePredicate;
};

inline LuaValue GetLuaValueAt(lua_State *lua, int index)
{
    lua_pushvalue(lua, index);
    return LuaValue::Pop(lua);
}

int LuaDefer(lua_State *lua, LuaDeferrer &deferrer, int currentTime)
{
    int maxCount = lua_gettop(lua),
        count    = 2; // the time, and function must be present..
    LuaValue arguments[LuaDeferredCall::ARGS];
    for (int i = 0, e = maxCount - count; i < e; i++)
        arguments[i] = GetLuaValueAt(lua, i + count + 1);
    int time = (int)lua_tointeger(lua, 1);
    const char *function = lua_tostring(lua, 2);
    deferrer.DeferCall(function, arguments, maxCount - count, currentTime + time);
    return 0;
}

class LuaDeferredCaller
{
public:

    static void               CreateInstance()  { if (!sInstance) sInstance = new LuaDeferredCaller; }
    static LuaDeferredCaller *GetInstance()     { return sInstance; }
    static void               DestroyInstance() { if (sInstance) delete sInstance; sInstance = 0; }

    int DeferByFrames(lua_State *lua)
    {
        return LuaDefer(lua, mDeferrerByFrames, GetCurrentFrames());
    }
    int DeferByTimeMs(lua_State *lua)
    {
        return LuaDefer(lua, mDeferrerByTimeMs, GetCurrentTimeMs());
    }
    int DeferByGameStateId(lua_State *lua)
    {
        return LuaDefer(lua, mDeferrerByGameStateId, 0);
    }
    void Update(lua_State *lua)
    {
        mDeferrerByFrames.Update(lua,      GetCurrentFrames());
        mDeferrerByTimeMs.Update(lua,      GetCurrentTimeMs());
        mDeferrerByGameStateId.Update(lua, GetGameStateId());
    }
    inline void SetGameStateId(int value)    
    { 
        printf("**GAMESTATE ID WAS %d NOW %d**\n", mGameStateId, value);
        mGameStateId = value;
    }
    inline int  GetGameStateId()          const { return mGameStateId; }

private:

    inline int  GetCurrentFrames()        const { return mFrames; }
    inline int  GetCurrentTimeMs()        const { return ::GetCurrentTimeMs(); }

    static LuaDeferredCaller *sInstance;

    LuaDeferredCaller() : mFrames(0), mDeferrerByGameStateId(ComparePredicateEq), mGameStateId(-1) {}
    ~LuaDeferredCaller() {}

    LuaDeferrer mDeferrerByFrames,
                mDeferrerByTimeMs,
                mDeferrerByGameStateId;
    int         mFrames,
                mGameStateId;
};

LuaDeferredCaller *LuaDeferredCaller::sInstance = 0;

int LuaDeferByTimeMs(lua_State *lua)
{
    if (!LuaDeferredCaller::GetInstance())
        LuaDeferredCaller::CreateInstance();
    LuaDeferredCaller *caller = LuaDeferredCaller::GetInstance();
    return caller->DeferByTimeMs(lua);
}

int LuaDeferByFrames(lua_State *lua)
{
    if (!LuaDeferredCaller::GetInstance())
        LuaDeferredCaller::CreateInstance();
    LuaDeferredCaller *caller = LuaDeferredCaller::GetInstance();
    return caller->DeferByFrames(lua);
}

int LuaDeferByGameStateId(lua_State *lua)
{
    if (!LuaDeferredCaller::GetInstance())
        LuaDeferredCaller::CreateInstance();
    LuaDeferredCaller *caller = LuaDeferredCaller::GetInstance();
    return caller->DeferByGameStateId(lua);
}

void LuaDeferUpdate(lua_State *lua)
{
    if (!LuaDeferredCaller::GetInstance())
        LuaDeferredCaller::CreateInstance();
    LuaDeferredCaller *caller = LuaDeferredCaller::GetInstance();
    return caller->Update(lua);
}

void LuaSetGameStateId(int value)
{
    if (!LuaDeferredCaller::GetInstance())
        LuaDeferredCaller::CreateInstance();
    LuaDeferredCaller *caller = LuaDeferredCaller::GetInstance();
    caller->SetGameStateId(value);
}

int  LuaGetGameStateId()
{
    if (!LuaDeferredCaller::GetInstance())
        LuaDeferredCaller::CreateInstance();
    LuaDeferredCaller *caller = LuaDeferredCaller::GetInstance();
    return caller->GetGameStateId();
}

int  LuaSetGameStateId(lua_State *lua)
{
    luaL_checktype(lua, -1, LUA_TNUMBER);
    int id = (int)lua_tointeger(lua, -1);
    LuaSetGameStateId(id);
    return 0;
}

int  LuaGetGameStateId(lua_State *lua)
{
    lua_pushinteger(lua, LuaGetGameStateId());
    return 1;
}