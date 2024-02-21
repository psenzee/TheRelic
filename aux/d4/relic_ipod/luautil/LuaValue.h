#ifndef _LUAVALUE_H
#define _LUAVALUE_H

typedef __int64          int64_t;
typedef unsigned __int64 uint64_t;

struct lua_State;

#include "LuaInterpreter.h"
#include "fast/FastString.h"

#include <string>
#include <map> // for LuaFunctionMap

/*************************************************************************************************/
/*!
    \class LuaValue

    Provides a variant-type class for easily handling Lua values in C++ having types that
    may not be known.

*/
/*************************************************************************************************/

typedef FastString<64> LuaValueString;
//typedef std::string LuaValueString;

class LuaValue
{
public:

    enum Type { LUATYPE_NONE, LUATYPE_BOOL, LUATYPE_INT64, LUATYPE_UINT64,
                LUATYPE_NUMBER, LUATYPE_STRING, LUATYPE_TABLE, LUATYPE_FUNCTION };

    typedef const char *(*LuaFunctionToName_fn)(const void *function, lua_State *lua, void *user);

    LuaValue();
    ~LuaValue();
    LuaValue(const LuaValue &other);

    LuaValue &operator=(const LuaValue &other);

    inline Type           GetType() const { return mType; }

    void                  SetTable(const LuaTable &value);
    LuaTable              GetTable() const;
    void                  SetFunction(const char *value);
    void                  SetFunctionValue(const void *value);
    const char           *GetFunction() const;
    const void           *GetFunctionValue() const;
    void                  SetString(const LuaValueString &value);
    const LuaValueString &GetString() const;
    void                  SetInt(int value);
    int                   GetInt() const;
    void                  SetUInt(unsigned int value);
    unsigned int          GetUInt() const;
    void                  SetNumber(double value);
    double                GetNumber() const;
    void                  SetInt64(int64_t value);
    int64_t               GetInt64() const;
    void                  SetUInt64(uint64_t value);
    uint64_t              GetUInt64() const;
    void                  SetBool(bool value);
    bool                  GetBool() const;
    void                  SetNull();

    bool                  ResolveFunctionName(lua_State *lua);
    
    // Thread, Function, UserData types could easily be added, if desired
    
    LuaValueString        ToString(lua_State *lua) const;

    void                  Push(lua_State *lua) const;
   
    static LuaValue       Pop(lua_State *lua);
   
    static int            CreateTable(lua_State *lua);
    static int            GetTableSize(lua_State *lua, int table);
    static void           ReleaseTable(lua_State *lua, int table);
    static int            GetGlobalTable(lua_State *lua);

    static LuaValue       GetValue(lua_State *lua, const LuaTable &table, const char *key);
    static void           Append(lua_State *lua, const LuaTable &table, const LuaValue &value);
    static void           SetPair(lua_State *lua, const LuaTable &table, const LuaValue &key, const LuaValue &value);
    static void           SetPair(lua_State *lua, const LuaTable &table, const char *key, const LuaValue &value);
    static void           SetPair(lua_State *lua, const LuaTable &table, int key, const LuaValue &value);    
    static const void    *GetFunctionPointer(lua_State *lua, const char *name);
    static const char    *GetFunctionName(lua_State *lua, const void *pointer);
    
    static void           SetFunctionToNameTranslator(LuaFunctionToName_fn translator, void *user);

    inline static LuaValue Table(const LuaTable &value)        { LuaValue lua; lua.SetTable(value);    return lua; }
    inline static LuaValue Int(int value)                      { LuaValue lua; lua.SetInt(value);      return lua; }
    inline static LuaValue UInt(unsigned int value)            { LuaValue lua; lua.SetUInt(value);     return lua; }
    inline static LuaValue Int64(int64_t value)                { LuaValue lua; lua.SetInt64(value);    return lua; }
    inline static LuaValue UInt64(uint64_t value)              { LuaValue lua; lua.SetUInt64(value);   return lua; }
    inline static LuaValue Bool(bool value)                    { LuaValue lua; lua.SetBool(value);     return lua; }
    inline static LuaValue Number(double value)                { LuaValue lua; lua.SetNumber(value);   return lua; }
    inline static LuaValue String(const char *value)           { LuaValue lua; lua.SetString(value);   return lua; }
    inline static LuaValue String(const LuaValueString &value) { LuaValue lua; lua.SetString(value);   return lua; }
    inline static LuaValue Function(const char *value)         { LuaValue lua; lua.SetFunction(value); return lua; }

private:

    static LuaFunctionToName_fn  sFunctionToNameTranslator;
    static void                 *sFunctionToNameTranslatorUser;

    bool Verify(Type type) const;

    enum { MAX_SIZE = sizeof(double) };

    Type              mType;
    union
    {
        bool          mBool;
        double        mNumber;
        const void   *mFunction;
        char          mData[MAX_SIZE]; // mData is for copying
    };
    LuaValueString    mString;
    LuaTable          mTable;
        
    enum { MAX_INT64_STRING_SIZE = 128 };

    static const LuaValueString EMPTY;
};

void StartLuaValueSerialization(lua_State *lua);
bool IsLuaValueSerializationStarted();

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

    LuaTableIterator(const LuaTable &table);
    ~LuaTableIterator();
    
    inline int   Count()   const { return mCount; }
    inline bool  IsList()  const { return mCount != 0; }
    
    inline bool  HasNext() const { return mHasNext; }

    LuaValue     Next();
    LuaKeyValue  NextPair();

private:

    LuaTable    mTable;
    lua_State  *mState;
    bool        mHasNext;
    int         mCount;
};


class LuaFunctionMap
{
public:

    void        Create(lua_State *lua);
    const char *GetName(lua_State *lua, const void *function);
    void        Clear();
    bool        IsStarted() const { return !mMap.empty(); }

private:

    //std::hash_map<const void *, LuaValueString> mMap;
    std::map<const void *, LuaValueString> mMap;

    // ok, i forgot, we're going to have to prepend the table key from the previous pass here
    // for now, let's do global functions only
    //static void PopulateFunctionMap(lua_State *lua, LuaTable table, std::hash_map<const void *, LuaValueString> &map);
    static void PopulateFunctionMap(lua_State *lua, const LuaTable &table, std::map<const void *, LuaValueString> &map);
};

#endif // _LUAVALUE_H