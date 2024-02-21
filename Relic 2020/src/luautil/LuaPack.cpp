#include "LuaPack.h"

extern "C"
{
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

#include <stdio.h>
#include "core/core_assert.h"

#include "core/strs.h"

// Pack:
// &c - character (8 bit)
// &i - int (32 bit)
// &i16 - int (16 bit)
// &f   - float (32 bit)
// &f8.8 - fixed point 8/8
// &z - null terminated string
// &s - size prefixed string (32 bit)
// &s16 - size prefixed string (16 bit)

static float GetFloat(lua_State *state, int count)
{
    luaL_checktype(state, count, LUA_TNUMBER);
    return static_cast<float>(lua_tonumber(state, count));
}

static int GetInteger(lua_State *state, int count)
{
    luaL_checktype(state, count, LUA_TNUMBER);
    return static_cast<int>(lua_tointeger(state, count));
}

static const char *GetString(lua_State *state, int count, int *size = 0)
{
    luaL_checktype(state, count, LUA_TSTRING);
    if (size)
        *size = static_cast<int>(lua_objlen(state, count));
    return static_cast<const char *>(lua_tostring(state, count));
}

static char *CopyInt(int value, int bits, char *out)
{
    short i16 = static_cast<short>(value);
    switch (bits)
    {
    case 0:
    case 32: memcpy(out, &value, 4);          out += 4; break; // default
    case 8:  *out = static_cast<char>(value); out++;    break;
    case 16: memcpy(out, &i16, 2);            out += 2; break;
    default: // $TODO error..
        printf("Pack: bit width %d for integer not supported!\n", bits);
        break;
    }
    return out;
}

static char *CopyFloatOrFixed(float value, int wholebits, int fractionbits, char *out)
{
    int bits = wholebits + fractionbits;
    if (bits == 32 || bits == 0)
    {
        memcpy(out, &value, sizeof(float));
        return out + sizeof(float);
        //return CopyInt(*(int *)&value, bits, out);
    }
    else if (bits == 16) // as 8.8
    {
        float multiplier = static_cast<float>(1 << fractionbits);
        return CopyInt(static_cast<int>(value * multiplier + 0.5f), bits, out);
    }
    printf("Pack ERROR: bit width %d for floats/fixed not supported!\n", bits);
    return out;
}

static int ReadInteger(const char **data, int size)
{
    short i16 = 0;
    int value = 0;
    switch (size)
    {
    case 0:
    case 32: memcpy(&value, *data, 4);        (*data) += 4; break; // default
    case 8:  value = static_cast<char>(**data); (*data)++; break;
    case 16: memcpy(&i16,   *data, 2);        (*data) += 2; value = i16; break;
    default: // $TODO error..
        printf("Unpack: bit width %d for integer not supported!\n", size);
        break;
    }
    return value;
}

static float ReadFloatOrFixed(const char **data, int wholebits, int fractionbits)
{
    int bits = wholebits + fractionbits;
    if (bits == 32 || bits == 0)
    {
        float value = 0;
        memcpy(&value, *data, sizeof(float));
        (*data) += sizeof(float);
        return value;
    }
    else if (bits == 16) // as 8.8
    {
        float multiplier = static_cast<float>(1 << fractionbits);
        short svalue = 0;
        memcpy(&svalue, *data, 2);
        (*data) += 2;
        return svalue / multiplier;
    }
    printf("Unpack ERROR: bit width %d for floats/fixed not supported!\n", bits);
    return 0.0f;
}

static char *CopyString(const char *value, int size, char *out)
{
    memcpy(out, value, size);
    return out + size;
}

static char *CopyNzString(const char *value, char *out)
{
    return CopyString(value, static_cast<int>(strlen(value)) + 1, out);
}

static int Pack(lua_State *state, char *out)
{
    const char *original = out;
    *out = '\0';
    luaL_checktype(state, 1, LUA_TSTRING);
    const char *format = lua_tostring(state, 1);
    if (!format) return 0;
    int maxCount = lua_gettop(state);
    int count = 2; // two because we assume a format string
    int wholesz = 0, fractionsz = 0;
    while (*format && count <= maxCount)
    {
        while (*format && *format != '&')
            *out++ = *format++;
        if (!*format)
            break;
        format++; // pass the ampersand
        if (!*format)
        {
            *out++ = '&';
            break;
        }
        int size = 32, strsz = 0;
        char ch = *format++;
        switch (ch)
        {
        case '&': *out++ = '&'; break; // escaped '&'
        case 'c': // handle 8-bit char
            // no size here
            out = CopyInt(GetInteger(state, count++), 8, out);
            break;
        case 'i': // handle integers
            if (!read(&format, size))
                size = 32;
            out = CopyInt(GetInteger(state, count++), size, out);
            break;                 
        case 'f': // handle floats/fixed
            // float doesn't have a size (always 32), fixed point values do
            if (!read(&format, size))
                size = 32;
            wholesz = size; fractionsz = 0;
            if (size != 32 && size != 0)
            {
                if (*format++ != '.')
                {
                    printf("Pack ERROR: expected a '.' after the whole part in fixed value size descriptor.\n");
                    break;
                }
                if (!read(&format, fractionsz))
                {
                    printf("Pack ERROR: expected a fractional size after the '.' in fixed value size descriptor.\n");
                    break;
                }
            }
            out = CopyFloatOrFixed(GetFloat(state, count++), wholesz, fractionsz, out);
            break;
        case 'z': // handle null terminated strings
            out = CopyNzString(GetString(state, count++), out);
            break;
        case 's': // handle size prefixed strings
            if (!read(&format, size))
                size = 32;
            {
                int strsz = 0;
                const char *str = GetString(state, count++, &strsz);
                out = CopyInt(strsz, size, out);
                out = CopyString(str, strsz, out);
            }
            break;
        }
    }
    *out = '\0';
    return static_cast<int>(out - original); // don't include the terminating zero
}

int LuaPack(lua_State *state)
{
    static char data[64 * 1024];
    int size = Pack(state, data);
    lua_pushlstring(state, data, size);
    return 1;
}

int LuaUnpack(lua_State *state)
{
    luaL_checktype(state, 1, LUA_TSTRING);
    luaL_checktype(state, 2, LUA_TSTRING);
    const char *format = lua_tostring(state, 1);
    size_t datasz = 0;
    const char *data = lua_tolstring(state, 2, &datasz);
    int wholesz = 0, fractionsz = 0, count = 0;
    while (*format)
    {
        while (*format && *format != '&')
            format++;
        if (!*format)
            break;
        format++; // pass the ampersand
        if (!*format)
            break;
        int size = 32, strsz = 0;
        char ch = *format++;
        switch (ch)
        {
        case '&': break; // escaped '&'
        case 'c': // handle 8-bit char, no size here
            lua_pushnumber(state, static_cast<int>(*data));
            data++;
            count++;
            break;
        case 'i': // handle integers
            if (!read(&format, size))
                size = 32;
            lua_pushnumber(state, ReadInteger(&data, size));
            count++;
            break;                 
        case 'f': // handle floats/fixed
            // float doesn't have a size (always 32), fixed point values do
            if (!read(&format, size))
                size = 32;
            wholesz = size; fractionsz = 0;
            if (size != 32 && size != 0)
            {
                if (*format++ != '.')
                {
                    printf("Unpack ERROR: expected a '.' after the whole part in fixed value size descriptor.\n");
                    break;
                }
                if (!read(&format, fractionsz))
                {
                    printf("Unpack ERROR: expected a fractional size after the '.' in fixed value size descriptor.\n");
                    break;
                }
            }
            lua_pushnumber(state, ReadFloatOrFixed(&data, wholesz, fractionsz));
            count++;
            break;
        case 'z': // handle null terminated strings
            lua_pushstring(state, data);
            data += strlen(data) + 1;
            count++;
            break;
        case 's': // handle size prefixed strings
            if (!read(&format, size))
                size = 32;
            size = ReadInteger(&data, size);
            lua_pushlstring(state, data, size);
            data += size;
            count++;
            break;
        }
    }
    return count;
}
