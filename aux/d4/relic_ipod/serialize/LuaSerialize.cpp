#include "LuaSerialize.h"

extern "C"
{
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

#include <assert.h>

#include "Pack.h"
#include "LuaUtil/LuaValue.h"
#include "core/Dictionary.h"

namespace Serialize
{

enum { MAX_BUFFER_SIZE = 65536 };

const char RPC_PREFIX[]    = "RPC";
const int  RPC_PREFIX_SIZE = sizeof(RPC_PREFIX) - 1;

static Dictionary *_gSerializeDictionary = 0;

static int _gThisUserId = 0;

const Dictionary *GetDictionary()
{
    return _gSerializeDictionary;
}

void SetDictionary(Dictionary *dictionary)
{
    _gSerializeDictionary = dictionary;
}

void AddToDictionary(const char *text)
{
    if (_gSerializeDictionary)
        _gSerializeDictionary->Insert(text);
}

int GetThisUserId()
{
    return _gThisUserId;
}

void SetThisUserId(int id)
{
    _gThisUserId = id;
}

int LuaSetThisUserId(lua_State *lua)
{
    luaL_checktype(lua, -1, LUA_TNUMBER);  // argsTable
    SetThisUserId(static_cast<int>(lua_tointeger(lua, -1)));
    return 0;
}

int LuaGetThisUserId(lua_State *lua)
{
    lua_pushinteger(lua, GetThisUserId());
    return 1;
}

int LuaStartSerialization(lua_State *state)
{
    StartLuaValueSerialization(state);
    return 0;
}

int LuaIsSerializationStarted(lua_State *state)
{
    lua_pushboolean(state, IsLuaValueSerializationStarted());
    return 1;
}

int LuaSerialize(lua_State *state)
{
    static char data[MAX_BUFFER_SIZE];
    char *p = data;
    lua_pushvalue(state, -1);
    PackLuaValue(&p, state, LuaValue::Pop(state), GetDictionary());
    lua_pushlstring(state, data, p - data);
    return 1;
}

int LuaDeserialize(lua_State *state)
{
    luaL_checktype(state, -1, LUA_TSTRING);
    size_t size = 0;
    const char *p = lua_tolstring(state, -1, &size);
    LuaValue v = UnpackLuaValue(&p, state, GetDictionary());
    v.Push(state);
    return 1;
}

int LuaDictionaryAdd(lua_State *state)
{
    luaL_checktype(state, -1, LUA_TSTRING);
    const char *p = lua_tostring(state, -1);
    AddToDictionary(p);
    return 0;
}

// function PackRpc(toUserId, method, argsTable) [void]
int PackRpc(lua_State *lua, char *buffer)
{
    luaL_checktype(lua, -3, LUA_TNUMBER); // toUserId
    luaL_checktype(lua, -2, LUA_TFUNCTION); // method
    luaL_checktype(lua, -1, LUA_TTABLE);  // argsTable

    char *p = buffer;
    memcpy(p, RPC_PREFIX, RPC_PREFIX_SIZE);
    p += RPC_PREFIX_SIZE;
    PackChar(&p, /*lua_tointeger(lua, -4)*/GetThisUserId()); // thisUserId
    PackChar(&p, static_cast<int>(lua_tointeger(lua, -3))); // toUserId

    lua_pushvalue(lua, -2);             // method (a function)
    PackLuaValue(&p, lua, LuaValue::Pop(lua), GetDictionary()); // serialize it
    lua_pushvalue(lua, -1);             // argsTable
    PackLuaValue(&p, lua, LuaValue::Pop(lua), GetDictionary()); // serialize it

    return static_cast<int>(p - buffer);
}

// function PackRpc(thisUserId, toUserId, method, argsTable) [void]
int LuaPackRpc(lua_State *lua)
{
    char buffer[MAX_BUFFER_SIZE];    
    lua_pushlstring(lua, buffer, PackRpc(lua, buffer));
    return 1;
}

void SendLuaMessage(const char *buffer, int size)
{
    // $TODO
}

// function SendRpc(thisUserId, toUserId, method, argsTable) [void]
int LuaSendRpc(lua_State *lua)
{
    char buffer[MAX_BUFFER_SIZE];    
    SendLuaMessage(buffer, PackRpc(lua, buffer));
    return 0;
}

// function ReadRpc(data) [success, fromUserId, method, argsTable]
int LuaReadRpc(lua_State *lua)
{
    luaL_checktype(lua, -1, LUA_TSTRING);  // data
    size_t size = 0;
    const char *data = lua_tolstring(lua, -1, &size);
    bool success = false;
    int fromUserId = -1, toUserId = -1;
    LuaValue method, argsTable;
    if (memcmp(data, RPC_PREFIX, RPC_PREFIX_SIZE) == 0)
    {
        data += RPC_PREFIX_SIZE;
        fromUserId = UnpackChar(&data);
        toUserId   = UnpackChar(&data);
        if (toUserId == 0 || toUserId == GetThisUserId())
        {
            method    = UnpackLuaValue(&data, lua, GetDictionary());
            argsTable = UnpackLuaValue(&data, lua, GetDictionary());
            success   = true;
        }
    }
    lua_pushboolean(lua, success);
    if (!success)
        return 1;
    lua_pushinteger(lua, fromUserId);
    method.Push(lua);
    argsTable.Push(lua);
    return 4;
}

void RegisterLuaSerializeFunctions(lua_State *state)
{
    lua_register(state, "StartSerialization",     LuaStartSerialization);
    lua_register(state, "IsSerializationStarted", LuaIsSerializationStarted);
    lua_register(state, "Serialize",         LuaSerialize);
    lua_register(state, "Deserialize",       LuaDeserialize);
    lua_register(state, "DictionaryAdd",     LuaDictionaryAdd);
    lua_register(state, "SetThisUserId",     LuaSetThisUserId);
    lua_register(state, "GetThisUserId",     LuaGetThisUserId);
    lua_register(state, "SendRpc",           LuaSendRpc);
    lua_register(state, "PackRpc",           LuaPackRpc);
    lua_register(state, "ReadRpc",           LuaReadRpc);
}

int UnpackLuaValueString(const char **from, LuaValueString &str, const Dictionary *dictionary)
{
    int size = GetUnpackStringSize(*from, dictionary);
    if (size < 0)
        return -1;
    str.resize(size, '\0');
    size = UnpackString(from, const_cast<char *>(str.data()), size, dictionary); // really evil hack, but safe and fast
    if (size < 0)
        return -1;
    return size;
}

// pack/unpack functions below //

static void PackLuaMap(char **to, lua_State *lua, const LuaTable &table, const Dictionary *dictionary)
{
    PackChar(to, PACK_MAP);
    LuaTableIterator iterator(table);
    while (iterator.HasNext())
    {
        LuaKeyValue pair(iterator.NextPair());
        PackLuaValue(to, lua, pair.key,   dictionary);
        PackLuaValue(to, lua, pair.value, dictionary);    
    }
    PackChar(to, '\xff'); // signify the end
}

static void PackLuaList(char **to, lua_State *lua, const LuaTable &table, int count, const Dictionary *dictionary)
{
    PackChar(to, PACK_LIST);
    PackNumber(to, count);
    if (count)
    {
        LuaTableIterator iterator(table);
        while (iterator.HasNext())
            PackLuaValue(to, lua, iterator.Next(), dictionary);
    }
}

void PackLuaTable(char **to, lua_State *lua, const LuaTable &table, const Dictionary *dictionary)
{
    int count = table.Size();
    if (count) PackLuaList(to, lua, table, count, dictionary);
    else       PackLuaMap(to, lua, table, dictionary);
}

void PackLuaValue(char **to, lua_State *lua, const LuaValue &value, const Dictionary *dictionary)
{
    float number = 0.f;
    switch (value.GetType())
    {
    case LuaValue::LUATYPE_NONE:     PackChar(to, PACK_NULL); break;
    case LuaValue::LUATYPE_BOOL:     PackChar(to, value.GetBool() ? PACK_BOOL_TRUE : PACK_BOOL_FALSE); break;
    case LuaValue::LUATYPE_NUMBER:   PackNumber(to, value.GetNumber()); break;   
    case LuaValue::LUATYPE_STRING:
        {
            const LuaValueString &s = value.GetString();
            PackString(to, s.data(), static_cast<int>(s.size()), dictionary);
        }
        break;
    case LuaValue::LUATYPE_FUNCTION:
        {
        LuaValue fn(value);
        PackChar(to, PACK_FUNCTION);
        fn.ResolveFunctionName(lua);
        PackString(to, fn.GetFunction(), dictionary);
        break;
        }
    case LuaValue::LUATYPE_TABLE:    PackLuaTable(to, lua, value.GetTable(), dictionary); break;    
    default:
    case LuaValue::LUATYPE_INT64: // don't use
    case LuaValue::LUATYPE_UINT64: // don't use    
        assert(false && "type not supported!");
        break;
    }
}

static LuaTable UnpackLuaList(const char **from, lua_State *lua, const Dictionary *dictionary)
{
    float number = 0.f;
    if (UnpackChar(from) != PACK_LIST || !UnpackNumber(from, &number))
    {
        assert(false && "bad serialized lua table (list)!");
        return LuaTable::Null();    
    }
    LuaTable table = LuaTable::New(lua);
    int count = static_cast<int>(number);
    for (int i = 0; i < count; i++)
        LuaValue::Append(lua, table, UnpackLuaValue(from, lua, dictionary));
    return table;
}

static LuaTable UnpackLuaMap(const char **from, lua_State *lua, const Dictionary *dictionary)
{
    if (UnpackChar(from) != PACK_MAP)
    {
        assert(false && "bad serialized lua table (map)!");
        return LuaTable::Null();    
    }
    LuaTable table = LuaTable::New(lua);
    while (**from != '\xff')
    {
        LuaValue key  (UnpackLuaValue(from, lua, dictionary));
        LuaValue value(UnpackLuaValue(from, lua, dictionary));
        LuaValue::SetPair(lua, table, key, value);
    }
    (*from)++;
    return table;
}

LuaTable UnpackLuaTable(const char **from, lua_State *lua, const Dictionary *dictionary)
{
    char c = **from;
    if      (c == PACK_LIST)
        return UnpackLuaList(from, lua, dictionary);
    else if (c == PACK_MAP)
        return UnpackLuaMap(from, lua, dictionary);
    return LuaTable::Null();
}

LuaValue UnpackLuaValue(const char **from, lua_State *lua, const Dictionary *dictionary)
{
    char c = **from;
    switch (GetAbstractPackType(static_cast<PackType>(c)))
    {
    case ABSTRACT_PACK_NULL:   (*from)++; return LuaValue();
    case ABSTRACT_PACK_BOOL:   (*from)++; return LuaValue::Bool(c == PACK_BOOL_TRUE);
    case ABSTRACT_PACK_NUMBER: 
        {
            float number = 0.f;        
            if (UnpackNumber(from, &number))
                return LuaValue::Number(number);
            assert(false && "bad serialized lua number!");
            break;
        }
    case ABSTRACT_PACK_STRING:
        {
            LuaValueString s;
            if (UnpackLuaValueString(from, s, dictionary) >= 0)
                return LuaValue::String(s);
            assert(false && "bad serialized lua string!");
            break;
        }
    case ABSTRACT_PACK_FUNCTION:
        {
            UnpackChar(from); // this will be PACK_FUNCTION
            LuaValueString s;
            if (UnpackLuaValueString(from, s, dictionary) >= 0)
                return LuaValue::Function(s.c_str());
            assert(false && "bad serialized lua function (string)!");
            break;
        }
    case ABSTRACT_PACK_TABLE:
        {
            LuaTable table = UnpackLuaTable(from, lua, dictionary);
            if (!table.IsNull())
                return LuaValue::Table(table);
            assert(false && "bad serialized lua table!");
            break;        
        }
    default: break;
    }
    assert(false && "bad serialized lua value!");
    return LuaValue();
}

}