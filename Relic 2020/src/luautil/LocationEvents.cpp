#include "LocationEvents.h"
#include "LuaEvents.h"
#include "LuaValue.h"
#include "events/EventDispatcher.h"
#include "game/GameState.h"

extern "C"
{
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

LocationEvents *LocationEvents::sInstance = 0;

void LocationEvents::AddEvent(const char *event, const Vector3 &location, float radius)
{
    std::vector<Location>::iterator i = FindLocation(event);
    Location loc;
    loc.name     = event;
    loc.location = location;
    loc.radius   = radius;
    loc.inside   = false;
    if (i != mLocations.end())
        (*i) = loc;
    else
        mLocations.push_back(loc);
}

void LocationEvents::RemoveEvent(const char *event)
{
    std::vector<Location>::iterator i = FindLocation(event);
    if (i != mLocations.end())
        mLocations.erase(i);
}

std::vector<LocationEvents::Location>::iterator LocationEvents::FindLocation(const char *event)
{
    for (std::vector<Location>::iterator i = mLocations.begin(), e = mLocations.end(); i != e; ++i)
        if (strcmp((*i).name.c_str(), event) == 0)
            return i;
     return mLocations.end();
}
    
void LocationEvents::NotifyLocation(const Vector3 &location)
{
    for (std::vector<Location>::iterator i = mLocations.begin(), e = mLocations.end(); i != e; ++i)
    {
        float rsq = (*i).radius * (*i).radius,
              dsq = (*i).location.distancesq(location);
        if (!(*i).inside && dsq < rsq)
        {
            mDispatcher->Fire((*i).name.c_str(), "Enter");
            (*i).inside = true;
        }
        else if ((*i).inside && dsq > rsq)
        {
            mDispatcher->Fire((*i).name.c_str(), "Leave");
            (*i).inside = false;
        }
    }
}

int Lua_SetLocationEvent(lua_State *lua)
{
    luaL_checktype(lua, -5, LUA_TSTRING); // event name
    luaL_checktype(lua, -4, LUA_TNUMBER); // loc x
    luaL_checktype(lua, -3, LUA_TNUMBER); // loc y
    luaL_checktype(lua, -2, LUA_TNUMBER); // loc z
    luaL_checktype(lua, -1, LUA_TNUMBER); // radius            
    const char *event = lua_tostring(lua, -5);
    Vector3     location(float(lua_tointeger(lua, -4)), float(lua_tointeger(lua, -3)), float(lua_tointeger(lua, -2)));
    float       radius = float(lua_tointeger(lua, -1));
    LocationEvents::GetInstance()->AddEvent(event, location, radius);
    return 0;
}

int Lua_RemoveLocationEvent(lua_State *lua)
{
    luaL_checktype(lua, -1, LUA_TSTRING); // event name
    const char *event = lua_tostring(lua, -1);
    LocationEvents::GetInstance()->RemoveEvent(event);
    return 0;
}

void RegisterLuaLocationEventFunctions(lua_State *lua)
{
    lua_register(lua, "SetLocationEvent",    Lua_SetLocationEvent);
    lua_register(lua, "RemoveLocationEvent", Lua_RemoveLocationEvent);    
}