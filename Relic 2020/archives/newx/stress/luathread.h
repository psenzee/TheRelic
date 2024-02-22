/*************************************************************************************************/
/*!
    \file   luathread.h

    $Header: //blaze/games/Madden/2010-NG/trunk/component/franchise/stress/luathread.h#2 $
    $Change: 44737 $
    $DateTime: 2009/06/04 14:33:39 $

    \attention
        (c) Electronic Arts Inc. 2009
*/
/*************************************************************************************************/

#ifndef _LUATHREAD_H
#define _LUATHREAD_H

struct lua_State;

#include "luainterpreter.h"

typedef int LuaTable;

namespace Blaze
{
namespace Stress
{

/*************************************************************************************************/
/*!
    \class LuaThread

    Wraps an individual Lua thread.  Note that a Lua thread is not a thread in the OS sense, but more
    akin to a Windows (and Blaze) lightweight fiber.  As such it works very well with Blaze's fibers
    and we provide one LuaThread per connection.

    Note that for each LuaThread instance, we create a table for lua state data to be associated
    with this lua 'fiber' and insert an element 'data' that points back to this C++ instance
    of LuaThread.  We then pass this instance to all of our RPCs like so -

     function MyNewFunction(instance)
      local fid = GetGMFranchiseIdForUser(instance)
      GetRegularSchedule(instance, fid)
      GetPlayoffSchedule(instance, fid)
     end

     function Main(instance)
 
      local fid = GetGMFranchiseIdForUser(instance)
      MyNewFunction(instance)
      ..
     end

    In the above case, the Execute method of LuaThread would be called with "Main" as the function
    to execute.

*/
/*************************************************************************************************/
class LuaThread
{
public:

    LuaThread(LuaInterpreter &interpreter);
    ~LuaThread();

    void PrintStackSize();

    // Set/GetContainer() - sets/gets the opaque pointer to the instance of
    // the module (in this case FranchiseInstance) in the stress tester that
    // is using this LuaThread.
    void  SetContainer(void *container);
    void *GetContainer();

    // Execute() - execute a lua function by name, passing in a table
    // with a light user data member 'data' pointing to this LuaThread
    int   Execute(const char *function);

    lua_State  *GetLuaState() { return mState; }

    LuaTable    CreateLuaTable();
    void        ReleaseLuaTable(LuaTable table);

    const char *GetLuaPropertyString(LuaTable table, const char *key);
    int         GetLuaPropertyInt(LuaTable table, const char *key);
    uint32_t    GetLuaPropertyUInt(LuaTable table, const char *key);
    int64_t     GetLuaPropertyInt64(LuaTable table, const char *key);
    uint64_t    GetLuaPropertyUInt64(LuaTable table, const char *key);
    bool        GetLuaPropertyBool(LuaTable table, const char *key);
    LuaTable    GetLuaPropertyTable(LuaTable table, const char *key);

    void        SetLuaPropertyString(LuaTable table, const char *key, const char *value);
    void        SetLuaPropertyInt(LuaTable table, const char *key, int value);
    void        SetLuaPropertyUInt(LuaTable table, const char *key, unsigned value);
    void        SetLuaPropertyInt64(LuaTable table, const char *key, int64_t value);
    void        SetLuaPropertyUInt64(LuaTable table, const char *key, uint64_t value);
    void        SetLuaPropertyBool(LuaTable table, const char *key, bool value);
    void        SetLuaPropertyTable(LuaTable table, const char *key, LuaTable value);

    void        AppendString(LuaTable table, const char *value);
    void        AppendInt(LuaTable table, int value);
    void        AppendUInt(LuaTable table, unsigned value);
    void        AppendInt64(LuaTable table, int64_t value);
    void        AppendUInt64(LuaTable table, uint64_t value);
    void        AppendBool(LuaTable table, bool value);
    void        AppendTable(LuaTable table, LuaTable value);

    // GetInstanceArgument() - to be called in a Lua c++ wrapper to get the
    // LuaInstance from the Lua 'instance' table that's passed around in
    // the stress tester
    static LuaThread *GetInstanceArgument(lua_State *state);
   
private:

    int             mTableId,
                    mThreadId;
    lua_State      *mState;
    bool            mResume;
    int             mYieldSize;
    void           *mContainer;
    LuaInterpreter &mInterpreter;

    LuaThread &operator=(const LuaThread &);
};

class LuaTableIterator
{
public:

    LuaTableIterator(lua_State *state, LuaTable table);
    
    bool        HasNext() const;

    int         NextInt();
    bool        NextBool();
    const char *NextString();
    LuaTable    NextTable();
    
private:

    lua_State  *mState;
    bool        mHasNext;
};

} // Stress
} // Blaze

#endif // _LUATHREAD_H
