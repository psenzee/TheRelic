#ifndef _LUATABLE_H
#define _LUATABLE_H

struct lua_State;

#include "LuaInterpreter.h"
#include "fast/Allocator.h"
#include <assert.h>

class LuaTable;

class LuaTableHandle
{
public:

    CLASS_NEW_DELETE()

protected:

    static LuaTableHandle *Pop(lua_State *lua);
    static LuaTableHandle *New(lua_State *lua);
    static LuaTableHandle *GetGlobal(lua_State *lua, const char *name = "_G");
    
    void                   Push();
    int                    Size();

    void                   AddFromStack();

    inline int             GetId() const { return mTable; }
    inline lua_State      *GetState()    { return mLua; }

    inline void            Retain()      { mReference++; }
    inline void            Release()     { assert(mReference >= 0); if (--mReference == 0) delete this; }

private:

    int         mTable;
    lua_State  *mLua;
    int         mReference;

    LuaTableHandle(int table, lua_State *lua);         // the real constructor

    LuaTableHandle(const LuaTableHandle &);            // intentionally unimplemented
    LuaTableHandle &operator=(const LuaTableHandle &); // intentionally unimplemented

    ~LuaTableHandle();

    friend class LuaTable;
};

class LuaTable
{
public:

    inline LuaTable(const LuaTable &other);
    inline LuaTable &operator=(const LuaTable &other);
    inline ~LuaTable();

    static LuaTable        Null()                                                   { return LuaTable(0); }
    static LuaTable        Pop(lua_State *lua)                                      { return LuaTable(LuaTableHandle::Pop(lua)); }
    static LuaTable        New(lua_State *lua)                                      { return LuaTable(LuaTableHandle::New(lua)); }
    static LuaTable        GetGlobal(lua_State *lua, const char *name = "_G")       { return LuaTable(LuaTableHandle::GetGlobal(lua, name)); }
    
    inline bool            IsNull()                                           const { return mTable == 0; }
    inline void            Push()                                             const { assert(mTable); if (mTable) mTable->Push(); }
    inline int             Size()                                             const { assert(mTable); return mTable ? mTable->Size() : 0; }

    inline void            AddFromStack()                                           { assert(mTable); if (mTable) mTable->AddFromStack(); }

    inline int             GetId()                                            const { assert(mTable); return mTable ? mTable->GetId() : -1; }
    inline lua_State      *GetState()                                         const { assert(mTable); return mTable ? mTable->GetState() : 0; }

private:

    inline LuaTable(LuaTableHandle *table) : mTable(table) {}
    mutable LuaTableHandle *mTable;
};

inline LuaTable::LuaTable(const LuaTable &other) : mTable(other.mTable)
{ 
    if (mTable) mTable->Retain();
}

inline LuaTable &LuaTable::operator=(const LuaTable &other)
{
    if (this != &other)
    {
        if (other.mTable) other.mTable->Retain();
        if (mTable)        mTable->Release();
        mTable = other.mTable;
    }
    return *this;
}

inline LuaTable::~LuaTable()
{
    if (mTable) mTable->Release();
}

#endif // _LUATABLE_H