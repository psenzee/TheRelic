#include "LuaNetPack.h"

extern "C"
{
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

#include <assert.h>

static void PackLuaMap(char **to, lua_State *lua, LuaTable table, const NetDictionary *dictionary)
{
    PackChar(to, PACK_MAP);
    LuaTableIterator iterator(lua, table);
    while (iterator.HasNext())
    {
        LuaKeyValue pair(iterator.NextPair());
        PackLuaValue(to, lua, pair.key,   dictionary);
        PackLuaValue(to, lua, pair.value, dictionary);    
    }
    PackChar(to, '\xff'); // signify the end
}

static void PackLuaList(char **to, lua_State *lua, LuaTable table, int count, const NetDictionary *dictionary)
{
    PackChar(to, PACK_LIST);
    PackNumber(to, count);
    if (count)
    {
        LuaTableIterator iterator(lua, table);
        while (iterator.HasNext())
            PackLuaValue(to, lua, iterator.Next(), dictionary);
    }
}

void PackLuaTable(char **to, lua_State *lua, LuaTable table, const NetDictionary *dictionary)
{
    int count = LuaValue::GetTableSize(lua, table);
    if (count) PackLuaList(to, lua, table, count, dictionary);
    else       PackLuaMap(to, lua, table, dictionary);
}

void PackLuaValue(char **to, lua_State *lua, const LuaValue &value, const NetDictionary *dictionary)
{
    float number = 0.f;
    switch (value.GetType())
    {
    case LuaValue::LUATYPE_NONE:   PackChar(to, PACK_NULL); break;
    case LuaValue::LUATYPE_BOOL:   PackChar(to, value.GetBool() ? PACK_BOOL_TRUE : PACK_BOOL_FALSE); break;
    case LuaValue::LUATYPE_NUMBER: PackNumber(to, value.GetNumber()); break;   
    case LuaValue::LUATYPE_STRING: PackString(to, value.GetString(), dictionary); break;
    case LuaValue::LUATYPE_TABLE:  PackLuaTable(to, lua, value.GetTable(), dictionary); break;    
    default:
    case LuaValue::LUATYPE_INT64: // don't use
    case LuaValue::LUATYPE_UINT64: // don't use    
        assert(false && "type not supported!");
        break;
    }
}

static LuaTable UnpackLuaList(const char **from, lua_State *lua, const NetDictionary *dictionary)
{
    float number = 0.f;
    if (UnpackChar(from) != PACK_LIST || !UnpackNumber(from, &number))
    {
        assert(false && "bad serialized lua table (list)!");
        return -1;    
    }
    LuaTable table = LuaValue::CreateTable(lua);
    int count = static_cast<int>(number);
    for (int i = 0; i < count; i++)
        LuaValue::Append(lua, table, UnpackLuaValue(from, lua, dictionary));
    return table;
}

static LuaTable UnpackLuaMap(const char **from, lua_State *lua, const NetDictionary *dictionary)
{
    if (UnpackChar(from) != PACK_MAP)
    {
        assert(false && "bad serialized lua table (map)!");
        return -1;    
    }
    LuaTable table = LuaValue::CreateTable(lua);
    while (**from != '\xff')
    {
        LuaValue key  (UnpackLuaValue(from, lua, dictionary));
        LuaValue value(UnpackLuaValue(from, lua, dictionary));
        LuaValue::SetPair(lua, table, key, value);
    }
    (*from)++;
    return table;
}

LuaTable UnpackLuaTable(const char **from, lua_State *lua, const NetDictionary *dictionary)
{
    char c = **from;
    if (c == PACK_LIST)
        return UnpackLuaList(from, lua, dictionary);
    else if (c == PACK_MAP)
        return UnpackLuaMap(from, lua, dictionary);
    return -1;
}

LuaValue UnpackLuaValue(const char **from, lua_State *lua, const NetDictionary *dictionary)
{
    char c = **from;
    switch (GetAbstractPackType(static_cast<PackType>(c)))
    {
    case ABSTRACT_PACK_NULL:   return LuaValue();
    case ABSTRACT_PACK_BOOL:   return LuaValue::Bool(c == PACK_BOOL_TRUE);
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
            std::string s;
            if (UnpackString(from, s, dictionary) >= 0)
                return LuaValue::String(s);
            assert(false && "bad serialized lua string!");
            break;
        }
    case ABSTRACT_PACK_TABLE:
        {
            LuaTable table = UnpackLuaTable(from, lua, dictionary);
            if (table >= 0)
                return LuaValue::Table(table);
            assert(false && "bad serialized lua table!");
            break;        
        }
    default: break;
    }
    assert(false && "bad serialized lua value!");
    return LuaValue();
}