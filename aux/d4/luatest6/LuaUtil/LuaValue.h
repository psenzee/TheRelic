#ifndef _LUAVALUE_H
#define _LUAVALUE_H

typedef __int64          int64_t;
typedef unsigned __int64 uint64_t;

struct lua_State;

#include "luainterpreter.h"

typedef int LuaTable;

#include <string>

/*************************************************************************************************/
/*!
    \class LuaValue

    Provides a variant-type class for easily handling Lua values in C++ having types that
    may not be known.

*/
/*************************************************************************************************/
class LuaValue
{
public:

    enum Type { LUATYPE_NONE, LUATYPE_BOOL, LUATYPE_INT64, LUATYPE_UINT64,
                LUATYPE_NUMBER, LUATYPE_STRING, LUATYPE_TABLE };

    inline LuaValue() : mType(LUATYPE_NONE) {}
   
    LuaValue(const LuaValue &other);

    LuaValue &operator=(const LuaValue &other);

    inline Type          GetType() const { return mType; }

    void                 SetTable(LuaTable value);
    LuaTable             GetTable() const;
    void                 SetString(const std::string &value);
    const std::string   &GetString() const;
    void                 SetInt(int value);
    int                  GetInt() const;
    void                 SetUInt(unsigned int value);
    unsigned int         GetUInt() const;
    void                 SetNumber(double value);
    double               GetNumber() const;
    void                 SetInt64(int64_t value);
    int64_t              GetInt64() const;
    void                 SetUInt64(uint64_t value);
    uint64_t             GetUInt64() const;
    void                 SetBool(bool value);
    bool                 GetBool() const;
    
    // Thread, Function, UserData types could easily be added, if desired
    
    std::string          ToString(lua_State *lua) const;    

    void                 Push(lua_State *lua) const;
    
    static LuaValue      Pop(lua_State *lua);
    
    static LuaTable      CreateTable(lua_State *lua);
    static int           GetTableSize(lua_State *lua, LuaTable table);
    static void          ReleaseTable(lua_State *lua, LuaTable table);
    static LuaValue      GetValue(lua_State *lua, LuaTable table, const char *key);
    static void          Append(lua_State *lua, LuaTable table, const LuaValue &value);
    static void          SetPair(lua_State *lua, LuaTable table, const LuaValue &key, const LuaValue &value);
    static void          SetPair(lua_State *lua, LuaTable table, const char *key, const LuaValue &value);
    static void          SetPair(lua_State *lua, LuaTable table, int key, const LuaValue &value);    

    inline static LuaValue Table(LuaTable value)              { LuaValue lua; lua.SetTable(value);  return lua; }
    inline static LuaValue Int(int value)                     { LuaValue lua; lua.SetInt(value);    return lua; }
    inline static LuaValue UInt(unsigned int value)           { LuaValue lua; lua.SetUInt(value);   return lua; }
    inline static LuaValue Int64(int64_t value)               { LuaValue lua; lua.SetInt64(value);  return lua; }
    inline static LuaValue UInt64(uint64_t value)             { LuaValue lua; lua.SetUInt64(value); return lua; }
    inline static LuaValue Bool(bool value)                   { LuaValue lua; lua.SetBool(value);   return lua; }
    inline static LuaValue Number(double value)               { LuaValue lua; lua.SetNumber(value); return lua; }
    inline static LuaValue String(const char *value)          { LuaValue lua; lua.SetString(value); return lua; }
    inline static LuaValue String(const std::string &value)   { LuaValue lua; lua.SetString(value); return lua; }

private:

    bool Verify(Type type) const;

    Type          mType;
    union
    {
        bool      mBool;
        double    mNumber;
        LuaTable  mTable;
        char      mData[sizeof(double)]; // mNumber is the largest, mData is for copying
    };
    std::string mString;
        
    enum { MAX_INT64_STRING_SIZE = 128 };

    static const std::string EMPTY;
};

/*************************************************************************************************/
/*!
    \class LuaKeyValue

    A simple key-value pair of LuaValues, especially useful for associative array (map/hash table)
    traversal.

*/
/*************************************************************************************************/
struct LuaKeyValue
{
    LuaValue key,
             value;
};

/*************************************************************************************************/
/*!
    \class LuaTableIterator

    Provides an easy to use iterator for LuaTables.  Is able to iterate over just values (for a
    list-type table) or key-value pairs (as an associative table).

*/
/*************************************************************************************************/
class LuaTableIterator
{
public:

    LuaTableIterator(lua_State *state, LuaTable table);
    ~LuaTableIterator();
    
    bool        HasNext() const;
    LuaValue    Next();
    LuaKeyValue NextPair();

private:

    lua_State  *mState;
    bool        mHasNext;
};

#endif // _LUAVALUE_H