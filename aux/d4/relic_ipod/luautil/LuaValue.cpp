#include "LuaInterpreter.h"
#include "LuaValue.h"
#include "LuaTable.h"

extern "C"
{
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#ifdef WIN32
#define strtoll  _strtoi64
#define strtoull _strtoui64
#endif

const LuaValueString LuaValue::EMPTY("");

LuaValue::LuaFunctionToName_fn  LuaValue::sFunctionToNameTranslator     = 0;
void                           *LuaValue::sFunctionToNameTranslatorUser = 0;

const char *GetLuaFunctionName(const void *function, lua_State *lua, void *user)
{
    if (!user || !function)
        return 0;
    LuaFunctionMap *map = reinterpret_cast<LuaFunctionMap *>(user);
    return map->GetName(lua, function);
}

LuaFunctionMap _gLuaFunctionMap;

void StartLuaValueSerialization(lua_State *lua)
{
    _gLuaFunctionMap.Clear();
    _gLuaFunctionMap.Create(lua);
}

bool IsLuaValueSerializationStarted()
{
    return _gLuaFunctionMap.IsStarted();
}

LuaValue::LuaValue() : mType(LUATYPE_NONE), mTable(LuaTable::Null())
{
    if (!sFunctionToNameTranslator)
    {
        // go ahead and set these up with the LuaFunctionMap stuff
        SetFunctionToNameTranslator(GetLuaFunctionName, &_gLuaFunctionMap);
    }
}

LuaValue::LuaValue(const LuaValue &other) : mType(other.mType), mString(other.mString), mTable(other.mTable)
{
    if (!sFunctionToNameTranslator)
    {
        // go ahead and set these up with the LuaFunctionMap stuff
        SetFunctionToNameTranslator(GetLuaFunctionName, &_gLuaFunctionMap);
    }
    memcpy(mData, other.mData, sizeof(mData));
}

LuaValue::~LuaValue()
{
    SetNull();
}

void LuaValue::SetNull()
{
    memset(mData, 0, sizeof(mData));
    mString.clear();
    mTable = LuaTable::Null();
    mType = LUATYPE_NONE;
}

LuaValue &LuaValue::operator=(const LuaValue &other)
{
    if (this != &other)
    {
        SetNull();
        mType   = other.mType;
        mString = other.mString;
        mTable  = other.mTable;
        memcpy(mData, other.mData, sizeof(mData));
    }
    return *this;
}

bool LuaValue::Verify(Type type) const
{
    if (mType != type)
    {
        assert(mType == type && "types are not equal!");
    }
    return mType == type;
}

void LuaValue::SetTable(const LuaTable &value)
{
    SetNull();
    mTable = value;
    mType  = LUATYPE_TABLE;
}

LuaTable LuaValue::GetTable() const
{
    if (!Verify(LUATYPE_TABLE))
    {
        return LuaTable::Null();
    }
    return mTable;
}

void LuaValue::SetString(const LuaValueString &value)
{
    SetNull();
    mString = value;
    mType = LUATYPE_STRING;
}

const LuaValueString &LuaValue::GetString() const
{
    if (!Verify(LUATYPE_STRING))
    {
        return EMPTY;
    }
    return mString;
}

void LuaValue::SetInt(int value)
{
    SetNull();
    mNumber = static_cast<double>(value);
    mType   = LUATYPE_NUMBER;
}

int LuaValue::GetInt() const
{
    return static_cast<int>(GetNumber());
}

void LuaValue::SetUInt(unsigned int value)
{
    SetNull();
    mNumber = static_cast<double>(value);
    mType   = LUATYPE_NUMBER;
}

unsigned int LuaValue::GetUInt() const
{
    return static_cast<unsigned int>(GetNumber());
}

void LuaValue::SetNumber(double value)
{
    SetNull();
    mNumber = value;
    mType = LUATYPE_NUMBER;
}

double LuaValue::GetNumber() const
{
    if (!Verify(LUATYPE_NUMBER))
    {
        return 0.0;
    }
    return mNumber;
}

void LuaValue::SetBool(bool value)
{
    SetNull();
    mBool = value;
    mType = LUATYPE_BOOL;
}

bool LuaValue::GetBool() const
{
    if (!Verify(LUATYPE_BOOL))
    {
        return false;
    }
    return mBool;
}

void LuaValue::SetFunction(const char *value)
{
    SetNull();
    mString = value;
    mType   = LUATYPE_FUNCTION;
}

void LuaValue::SetFunctionValue(const void *value)
{
    SetNull();
    mFunction = value;
    mType     = LUATYPE_FUNCTION;
}

const char *LuaValue::GetFunction() const
{
    if (!Verify(LUATYPE_FUNCTION))
    {
        return false;
    }
    return mString.c_str();
}

const void *LuaValue::GetFunctionValue() const
{
    if (!Verify(LUATYPE_FUNCTION))
    {
        return false;
    }
    return mFunction;
}

void LuaValue::SetFunctionToNameTranslator(LuaFunctionToName_fn translator, void *user)
{
    sFunctionToNameTranslator     = translator;
    sFunctionToNameTranslatorUser = user;
}

static bool IsInt64(const char *text)
{
    return strncmp(text, "#i64:", 4) == 0;
}

static bool IsUInt64(const char *text)
{
    return strncmp(text, "#u64:", 4) == 0;
}

static bool StringToInt64(const char *text, int64_t &value)
{
    if (!IsInt64(text))
    {
        return false;
    }
    const int DECIMAL_RADIX = 10;
    text += 4; // get rid of the type prefix
    const char *ends = text;
    value = (int64_t)strtoll(text, const_cast<char **>(&ends), DECIMAL_RADIX);
    return (ends != NULL && ends != text);
}

static bool StringToUInt64(const char *text, uint64_t &value)
{
    if (!IsUInt64(text))
    {
        return false;
    }
    const int DECIMAL_RADIX = 10;
    text += 4; // get rid of the type prefix
    const char *ends = text;
    value = (uint64_t)strtoull(text, const_cast<char **>(&ends), DECIMAL_RADIX);
    return (ends != NULL && ends != text);
}

static void Int64ToString(int64_t value, char *buffer)
{
#ifdef WIN32
    sprintf(buffer, "#i64:%I64d", value);
#else
    sprintf(buffer, "#i64:%lld",  value);
#endif
}

static void UInt64ToString(uint64_t value, char *buffer)
{
#ifdef WIN32
    sprintf(buffer, "#u64:%I64u", value);
#else
    sprintf(buffer, "#u64:%llu",  value);
#endif
}

void LuaValue::SetInt64(int64_t value)
{
    SetNull();
    char buffer[MAX_INT64_STRING_SIZE];
    Int64ToString(value, buffer);
    mString = buffer;
    mType = LUATYPE_INT64;
}

int64_t LuaValue::GetInt64() const
{
    int64_t value = 0;
    if (Verify(LUATYPE_INT64) && !StringToInt64(mString.c_str(), value))
    {
        assert(false && "value is not an int64!");
    }
    return value;
}

void LuaValue::SetUInt64(uint64_t value)
{
    char buffer[MAX_INT64_STRING_SIZE];
    UInt64ToString(value, buffer);
    mString = buffer;
    mType = LUATYPE_UINT64;
}

uint64_t LuaValue::GetUInt64() const
{
    uint64_t value = 0;
    if (Verify(LUATYPE_UINT64) && !StringToUInt64(mString.c_str(), value))
    {
        assert(false && "value is not a uint64!");
    }
    return value;
}

LuaValueString LuaValue::ToString(lua_State *lua) const
{
    Push(lua);
    size_t size = 0;
    const char *str = lua_tolstring(lua, -1, &size);
    lua_pop(lua, 1);
    return LuaValueString(!str ? "" : str, size);
}

void LuaValue::Push(lua_State *lua) const
{
    switch (mType)
    {
    case LUATYPE_NONE:
        lua_pushnil(lua);
        break;
    case LUATYPE_BOOL:
        lua_pushboolean(lua, mBool ? 1 : 0);
        break;
    case LUATYPE_UINT64:
    case LUATYPE_INT64:
    case LUATYPE_STRING:
        lua_pushstring(lua, mString.c_str());
        break;
    case LUATYPE_NUMBER:
        lua_pushnumber(lua, mNumber);
        break;
    case LUATYPE_TABLE:
        mTable.Push();
        break;
    case LUATYPE_FUNCTION:
        lua_getglobal(lua, mString.c_str());
        luaL_checktype(lua, -1, LUA_TFUNCTION); 
        break;
    default:
        // $TODO assert/error
        break;
    }
}

bool LuaValue::ResolveFunctionName(lua_State *lua)
{
    if (!Verify(LUATYPE_FUNCTION) || !mString.empty())
        return false;
    const char *name = GetFunctionName(lua, mFunction);
    if (!name)
        return false;
    mString = name;
    return true;
}

LuaValue LuaValue::Pop(lua_State *lua)
{
    LuaValue value; // starts off null
    switch (lua_type(lua, -1))
    {
    case LUA_TNONE:
        // assert/error here?
        break;
    case LUA_TNIL:
        lua_pop(lua, 1);
        break;
    case LUA_TNUMBER:
        value.SetNumber(lua_tonumber(lua, -1));
        lua_pop(lua, 1);
        break;
    case LUA_TBOOLEAN:
        value.SetBool(lua_toboolean(lua, -1) != 0);
        lua_pop(lua, 1);
        break;
    case LUA_TSTRING:
        {
            const char *text = lua_tostring(lua, -1);
            uint64_t       uint64 = 0;
            int64_t        int64 = 0;
            if (*text == '#' && IsUInt64(text) && StringToUInt64(text, uint64))
            {
                value.SetUInt64(uint64);
            }
            else if (*text == '#' && IsUInt64(text) && StringToInt64(text, int64))
            {
                value.SetInt64(int64);
            }
            else
            {
                value.SetString(text);
            }
            lua_pop(lua, 1);
        }
        break;
    case LUA_TTABLE:
        value.SetTable(LuaTable::Pop(lua));
        break;
    case LUA_TFUNCTION:
        value.SetFunctionValue(lua_topointer(lua, -1));
        lua_pop(lua, 1);
        break;
    default:
        // we don't support the other types here
        // $TODO assert/error
        lua_pop(lua, 1); // $TODO THIS WAS ADDED!
        break;
    }
    return value;
}

int LuaValue::GetTableSize(lua_State *lua, int table)
{
    lua_rawgeti(lua, LUA_REGISTRYINDEX, static_cast<int>(table));
    int size = static_cast<int>(lua_objlen(lua, -1));
    lua_pop(lua, 1);
    return size;
}

const void *LuaValue::GetFunctionPointer(lua_State *lua, const char *name)
{
    lua_getglobal(lua, name);
    const void *fptr = lua_topointer(lua, -1);
    lua_pop(lua, 1);
    return fptr;
}

const char *LuaValue::GetFunctionName(lua_State *lua, const void *pointer)
{
    if (sFunctionToNameTranslator)
    {
        return sFunctionToNameTranslator(pointer, 
                                         lua, sFunctionToNameTranslatorUser);
    }
    return 0;
}

int LuaValue::GetGlobalTable(lua_State *lua)
{
    lua_getglobal(lua, "_G");
    return luaL_ref(lua, LUA_REGISTRYINDEX);
}

int LuaValue::CreateTable(lua_State *lua)
{
    lua_newtable(lua);
    return luaL_ref(lua, LUA_REGISTRYINDEX);
}

void LuaValue::ReleaseTable(lua_State *lua, int table)
{
    luaL_unref(lua, LUA_REGISTRYINDEX, static_cast<int>(table));
}

LuaValue LuaValue::GetValue(lua_State *lua, const LuaTable &table, const char *key)
{
    lua_rawgeti(lua, LUA_REGISTRYINDEX, table.GetId());
    luaL_checktype(lua, -1, LUA_TTABLE);
    lua_getfield(lua, -1, key);
    LuaValue value(LuaValue::Pop(lua));
    lua_pop(lua, 1);
    return value;
}

void LuaValue::Append(lua_State *lua, const LuaTable &table, const LuaValue &value)
{
    lua_rawgeti(lua, LUA_REGISTRYINDEX, table.GetId());
    luaL_checktype(lua, -1, LUA_TTABLE); // $NOTE CHANGED THIS FROM 1 to -1
    value.Push(lua);
    lua_rawseti(lua, -2, static_cast<int>(lua_objlen(lua, -2)) + 1);
    lua_pop(lua, 1);
}

void LuaValue::SetPair(lua_State *lua, const LuaTable &table, const LuaValue &key, const LuaValue &value)
{
    LuaValueString keyString(key.ToString(lua));
    SetPair(lua, table, keyString.c_str(), value);
}

void LuaValue::SetPair(lua_State *lua, const LuaTable &table, const char *key, const LuaValue &value)
{
    lua_rawgeti(lua, LUA_REGISTRYINDEX, table.GetId());
    value.Push(lua);
    lua_setfield(lua, -2, key);
    lua_pop(lua, 1);
}

void LuaValue::SetPair(lua_State *lua, const LuaTable &table, int key, const LuaValue &value)
{
    lua_rawgeti(lua, LUA_REGISTRYINDEX, table.GetId());
    value.Push(lua);
    lua_rawseti(lua, -2, key);
}

/*************************************************************************************************/
/*!
    \class LuaTableIterator

    Provides an easy to use iterator for LuaTables.  Is able to iterate over just values (for a
    list-type table) or key-value pairs (as an associative table).

*/
/*************************************************************************************************/

LuaTableIterator::LuaTableIterator(const LuaTable &table) : mTable(table), mHasNext(true), mCount(0)
{
    lua_State *state = mTable.GetState();
    lua_rawgeti(state, LUA_REGISTRYINDEX, mTable.GetId()); // here we need to push the table onto the stack (at -2)
    mCount = static_cast<int>(lua_objlen(state, -1));      // if it has a length, then it is a list
    lua_pushnil(state);                                    // this is required to prime the iteration       (at -1)
    mHasNext = lua_next(state, -2) != 0;
}

LuaTableIterator::~LuaTableIterator()
{
    if (mHasNext) lua_pop(mTable.GetState(), 1);
}

LuaValue LuaTableIterator::Next()
{
    lua_State *state = mTable.GetState();
    LuaValue value(LuaValue::Pop(state));
    mHasNext = lua_next(state, -2) != 0;
    if (!mHasNext) lua_pop(state, 1);
    return value;
}

LuaKeyValue LuaTableIterator::NextPair()
{
    LuaKeyValue keyValue;
    lua_State *state = mTable.GetState();
    keyValue.value = LuaValue::Pop(state);
    lua_pushvalue(state, -1); // duplicate the top element
    keyValue.key = LuaValue::Pop(state);
    mHasNext = lua_next(state, -2) != 0;
    if (!mHasNext) lua_pop(state, 1);
    return keyValue;
}

const char *LuaFunctionMap::GetName(lua_State *lua, const void *function)
{
    Create(lua);
    std::map<const void *, LuaValueString>::const_iterator i = mMap.find(function);
    if (i != mMap.end())
        return (*i).second.c_str();
    return 0;
}

void LuaFunctionMap::Clear()
{ 
    mMap.clear();
}

void LuaFunctionMap::Create(lua_State *lua)
{
    if (mMap.empty())
    {
        LuaTable global = LuaTable::GetGlobal(lua);
        PopulateFunctionMap(lua, global, mMap);
    }
}

// ok, i forgot, we're going to have to prepend the table key from the previous pass here
// for now, let's do global functions only
void LuaFunctionMap::PopulateFunctionMap(lua_State *lua, const LuaTable &table, std::map<const void *, LuaValueString> &map)
{
    LuaTableIterator iterator(table);
    if (!iterator.IsList())
    {
        while (iterator.HasNext())
        {
            LuaKeyValue pair(iterator.NextPair());
            if (pair.key.GetType() == LuaValue::LUATYPE_STRING && 
                pair.value.GetType() == LuaValue::LUATYPE_FUNCTION)
                map[pair.value.GetFunctionValue()] = pair.key.GetString();
        }
    }
}