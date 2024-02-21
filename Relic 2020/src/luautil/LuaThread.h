#ifndef _LUATHREAD_H
#define _LUATHREAD_H

struct lua_State;

#include "LuaInterpreter.h"
#include "LuaValue.h"
#include "fast/Allocator.h"

typedef int LuaFunction;

class LuaThread
{
public:

    CLASS_NEW_DELETE()

    explicit LuaThread(LuaInterpreter &interpreter);
    explicit LuaThread(LuaThread *copy);    
    ~LuaThread();

    void        PrintStackSize();

    // Set/GetContainer() - sets/gets the opaque pointer to the instance of whatever is using this
    void        SetContainer(void *container);
    void       *GetContainer();

    void        SetUserData(int ref)       { mUserData = ref; }
    int         GetUserData()        const { return mUserData; }

    // Execute() - execute a lua function by name, passing in a table
    // with a light user data member 'data' pointing to this LuaThread
    int         Execute(const char *function);

    void        Collect(); 

    lua_State  *GetLuaState() { return mState; }

    /*
    LuaTable    CreateLuaTable();
    int         GetLuaTableSize(LuaTable table);
    void        ReleaseLuaTable(LuaTable table);

    LuaValue    GetLuaProperty(LuaTable table, const char *key);
    void        SetLuaProperty(LuaTable table, const char *key, const LuaValue &value);
    
    void        Append(LuaTable table, const LuaValue &value);

    void        SetPair(LuaTable table, const LuaValue &key, const LuaValue &value);
    void        SetPair(LuaTable table, const char *key, const LuaValue &value);
    void        SetPair(LuaTable table, int key, const LuaValue &value);
*/
    //! GetInstanceArgument() - to be called in a Lua c++ wrapper to get the
    /// LuaInstance from the Lua 'instance' table that's passed around in
    /// the stress tester
    static LuaThread *GetInstanceArgument(lua_State *state);
   
private:

    int             mTableId,
                    mThreadId,
                    mUserData;
    lua_State      *mState;
    bool            mResume;
    int             mYieldSize;
    void           *mContainer;
    LuaInterpreter &mInterpreter;

    LuaThread &operator=(const LuaThread &);
};

#endif // _LUATHREAD_H
