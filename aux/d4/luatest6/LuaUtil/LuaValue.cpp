#include "LuaInterpreter.h"
#include "LuaValue.h"

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

const std::string LuaValue::EMPTY("");

LuaValue::LuaValue(const LuaValue &other) : mType(other.mType), mString(other.mString)
{
    memcpy(mData, other.mData, sizeof(mData));
}

LuaValue &LuaValue::operator=(const LuaValue &other)
{
    if (this != &other)
    {
        mType   = other.mType;
        mString = other.mString;
        memcpy(mData, other.mData, sizeof(mData));
    }
    return *this;
}

bool LuaValue::Verify(Type type) const
{
    // $TODO USE BLAZE ASSERT!
    if (mType != type)
    {
        assert(mType == type && "types are not equal!");
    }
    return mType == type;
}

void LuaValue::SetTable(LuaTable value)
{ 
    mTable = value;
    mType = LUATYPE_TABLE;
}

LuaTable LuaValue::GetTable() const
{
    if (!Verify(LUATYPE_TABLE))
    {
        return LUA_NOREF;
    }
    return mTable;
}

void LuaValue::SetString(const std::string &value)
{
    mString = value;
    mType = LUATYPE_STRING;
}

const std::string &LuaValue::GetString() const
{
    if (!Verify(LUATYPE_STRING))
    {
        return EMPTY;
    }
    return mString;
}

void LuaValue::SetInt(int value)
{
    mNumber = static_cast<double>(value);
    mType   = LUATYPE_NUMBER;
}

int LuaValue::GetInt() const
{
    return static_cast<int>(GetNumber());
}

void LuaValue::SetUInt(unsigned int value)
{
    mNumber = static_cast<double>(value);
    mType   = LUATYPE_NUMBER;
}

unsigned int LuaValue::GetUInt() const
{
    return static_cast<unsigned int>(GetNumber());
}

void LuaValue::SetNumber(double value)
{
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

std::string LuaValue::ToString(lua_State *lua) const
{
    Push(lua);
    const char *str = lua_tostring(lua, -1);
    lua_pop(lua, 1);
    return std::string(str == NULL ? "" : str);
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
        lua_rawgeti(lua, LUA_REGISTRYINDEX, static_cast<int>(mTable));
        break;
    default:
        // $TODO assert/error
        break;
    }
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
        value.SetTable(static_cast<LuaTable>(luaL_ref(lua, LUA_REGISTRYINDEX)));
        break;
    default:
        // we don't support the other types here
        // $TODO assert/error
        break;
    }
    return value;
}

int LuaValue::GetTableSize(lua_State *lua, LuaTable table)
{
    lua_rawgeti(lua, LUA_REGISTRYINDEX, static_cast<int>(table));
    int size = static_cast<int>(lua_objlen(lua, -1));
    lua_pop(lua, 1);
    return size;
}

LuaTable LuaValue::CreateTable(lua_State *lua)
{
    lua_newtable(lua);
    return static_cast<LuaTable>(luaL_ref(lua, LUA_REGISTRYINDEX));
}

void LuaValue::ReleaseTable(lua_State *lua, LuaTable table)
{
    luaL_unref(lua, LUA_REGISTRYINDEX, static_cast<int>(table));
}

LuaValue LuaValue::GetValue(lua_State *lua, LuaTable table, const char *key)
{
    lua_rawgeti(lua, LUA_REGISTRYINDEX, static_cast<int>(table));
    luaL_checktype(lua, -1, LUA_TTABLE);
    lua_getfield(lua, -1, key);
    LuaValue value(LuaValue::Pop(lua));
    lua_pop(lua, 1);
    return value;
}

void LuaValue::Append(lua_State *lua, LuaTable table, const LuaValue &value)
{
    lua_rawgeti(lua, LUA_REGISTRYINDEX, static_cast<int>(table));
    luaL_checktype(lua, -1, LUA_TTABLE); // $NOTE CHANGED THIS FROM 1 to -1
    value.Push(lua);
    lua_rawseti(lua, -2, static_cast<int>(lua_objlen(lua, -2)) + 1);
    lua_pop(lua, 1);
}

void LuaValue::SetPair(lua_State *lua, LuaTable table, const LuaValue &key, const LuaValue &value)
{
    std::string keyString(key.ToString(lua));
    SetPair(lua, table, keyString.c_str(), value);
}

void LuaValue::SetPair(lua_State *lua, LuaTable table, const char *key, const LuaValue &value)
{
    lua_rawgeti(lua, LUA_REGISTRYINDEX, static_cast<int>(table));
    value.Push(lua);
    lua_setfield(lua, -2, key);
    lua_pop(lua, 1);
}

void LuaValue::SetPair(lua_State *lua, LuaTable table, int key, const LuaValue &value)
{
    lua_rawgeti(lua, LUA_REGISTRYINDEX, static_cast<int>(table));
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

LuaTableIterator::LuaTableIterator(lua_State *state, LuaTable table) : mState(state), mHasNext(true)
{        
    lua_rawgeti(mState, LUA_REGISTRYINDEX, (int)table); // here we need to push the table onto the stack (at -2)
    lua_pushnil(mState);                                // this is required to prime the iteration       (at -1)
    mHasNext = lua_next(mState, -2) != 0;
}

LuaTableIterator::~LuaTableIterator()
{
    if (mHasNext) lua_pop(mState, 1);
}

bool LuaTableIterator::HasNext() const
{
    return mHasNext;    
}

LuaValue LuaTableIterator::Next()
{
    LuaValue value(LuaValue::Pop(mState));
    mHasNext = lua_next(mState, -2) != 0;
    if (!mHasNext) lua_pop(mState, 1);
    return value;
}

LuaKeyValue LuaTableIterator::NextPair()
{
    LuaKeyValue keyValue;
    keyValue.value = LuaValue::Pop(mState);
    lua_pushvalue(mState, -1); // duplicate the top element
    keyValue.key = LuaValue::Pop(mState);
    mHasNext = lua_next(mState, -2) != 0;
    if (!mHasNext) lua_pop(mState, 1);
    return keyValue;
}
