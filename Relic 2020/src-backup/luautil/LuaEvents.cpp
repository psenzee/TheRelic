#include "LuaEvents.h"
#include "luautil/LuaValue.h"
#include "luautil/LuaDefer.h"
#include "events/EventDispatcher.h"
#include "events/EventTimer.h"
#include "LocationEvents.h"

#include "game/GameState.h"

extern "C"
{
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

LuaListener::LuaListener(lua_State *lua, const char *function, LuaValue &data, EventDispatcher *dispatcher) 
    : mLuaState(lua), mFunction(function), mDispatcher(dispatcher), mData(data)
{
}

LuaListener::~LuaListener()
{
    Unregister();    
}

// implements IEventListener::Notify(..)
bool LuaListener::Notify(const char *event, const char *info)
{
    return Fire(info); // event here is irrelevant
}

bool LuaListener::Fire(const char *info)
{
//  if (!LuaDoesFunctionExist(mLuaState, mFunction.c_str()))
//      return false;
    LuaValue value[] = { LuaValue::_String(info), mData };
    LuaCall(mLuaState, mFunction.c_str(), value, 2);
    return true;
}

void LuaListener::Register(const char *event)
{
    mEvent = event;
    mDispatcher->Register(event, HandleEvent, this);
}

void LuaListener::Unregister()
{        
    mDispatcher->Unregister(mEvent.c_str(), HandleEvent, this);
}    

// static
bool LuaListener::HandleEvent(const char *info, void *context)
{
    LuaListener *listener = reinterpret_cast<LuaListener *>(context);
    return !listener ? false : listener->Fire(info);
}

TimerEvent::TimerEvent(EventDispatcher *dispatcher, EventTimer *timer, unsigned elapsed, const char *event) 
    : mDispatcher(dispatcher), mTimer(timer), mElapsed(elapsed)
{
    mEventName = event;
    mTimer->Add(HandleTimerEvent, this, mElapsed);
}

// static    
void TimerEvent::HandleTimerEvent(void *context)
{
    TimerEvent *timer = reinterpret_cast<TimerEvent *>(context);
    if (!timer)
        return;
    timer->Unregister();
    if (timer->mDispatcher) timer->mDispatcher->Fire(timer->mEventName.c_str(), "Elapsed");
    delete timer;
}

void TimerEvent::Unregister()
{
    mTimer->Remove(this);    
}

TimerEvent::~TimerEvent()
{
    Unregister();
}    

EventDispatcher *GetGlobalEventDispatcher()
{
    return GameState::GetInstance()->GetEventDispatcher();
}

EventDispatcher *GetGlobalUiEventDispatcher()
{
    return GameState::GetInstance()->GetUiEventDispatcher();
}

EventTimer *GetGlobalEventTimer()
{
    return GameState::GetInstance()->GetEventTimer();
}

// Lua Interface
int Lua_RegisterListener(lua_State *lua)
{    
    luaL_checktype(lua, -3, LUA_TSTRING); // event name
    luaL_checktype(lua, -2, LUA_TSTRING); // function name
    lua_pushvalue(lua, -1); // duplicate the top element, the luaUserData value
    LuaValue userdata(LuaValue::Pop(lua));
    LuaListener *listener = new LuaListener(lua, lua_tostring(lua, -2), userdata, GetGlobalEventDispatcher());
    listener->Register(lua_tostring(lua, -3));
    lua_pushlightuserdata(lua, listener);
    return 1;
}

int Lua_RegisterUiListener(lua_State *lua)
{    
    luaL_checktype(lua, -3, LUA_TSTRING); // event name
    luaL_checktype(lua, -2, LUA_TSTRING); // function name
    lua_pushvalue(lua, -1); // duplicate the top element, the luaUserData value
    LuaValue userdata(LuaValue::Pop(lua));
    LuaListener *listener = new LuaListener(lua, lua_tostring(lua, -2), userdata, GetGlobalUiEventDispatcher());
    listener->Register(lua_tostring(lua, -3));
    lua_pushlightuserdata(lua, listener);
    return 1;
}

int Lua_UnregisterListener(lua_State *lua)
{    
    luaL_checktype(lua, -1, LUA_TLIGHTUSERDATA); // the listener
    LuaListener *listener = reinterpret_cast<LuaListener *>(lua_touserdata(lua, -1)); // the listener instance
    delete listener; // this automatically unregisters as well..
    return 0;
}

int Lua_FireEvent(lua_State *lua)
{
    luaL_checktype(lua, -2, LUA_TSTRING); // event name
    luaL_checktype(lua, -1, LUA_TSTRING); // info string
    const char *event = lua_tostring(lua, -2);
    const char *info  = lua_tostring(lua, -1);
    GetGlobalEventDispatcher()->Fire(event, info);
    return 0;
}

int Lua_FireUiEvent(lua_State *lua)
{
    luaL_checktype(lua, -2, LUA_TSTRING); // event name
    luaL_checktype(lua, -1, LUA_TSTRING); // info string
    const char *event = lua_tostring(lua, -2);
    const char *info  = lua_tostring(lua, -1);
    GetGlobalUiEventDispatcher()->Fire(event, info);
    return 0;
}

int Lua_SetTimerEvent(lua_State *lua)
{
    luaL_checktype(lua, -2, LUA_TSTRING); // event name
    luaL_checktype(lua, -1, LUA_TNUMBER); // delay in ms
    const char *event = lua_tostring(lua, -2);
    unsigned time = static_cast<unsigned>(lua_tointeger(lua, -1));
    TimerEvent *timer = new TimerEvent(GetGlobalEventDispatcher(), GetGlobalEventTimer(), time, event);
    return 0;
}

void RegisterLuaEventFunctions(lua_State *lua)
{
    lua_register(lua, "RegisterListener",   Lua_RegisterListener);
    lua_register(lua, "RegisterUiListener", Lua_RegisterUiListener);
    lua_register(lua, "UnregisterListener", Lua_UnregisterListener); // use for both regular and UI events
    lua_register(lua, "FireEvent",          Lua_FireEvent);
    lua_register(lua, "FireUiEvent",        Lua_FireUiEvent);
    lua_register(lua, "SetTimerEvent",      Lua_SetTimerEvent);

    lua_register(lua, "DeferByFrames",      LuaDeferByFrames);
    lua_register(lua, "DeferByTimeMs",      LuaDeferByTimeMs);
    lua_register(lua, "DeferByGameStateId", LuaDeferByGameStateId);
    lua_register(lua, "SetGameStateId",     LuaSetGameStateId);
    lua_register(lua, "GetGameStateId",     LuaGetGameStateId);

    RegisterLuaLocationEventFunctions(lua);
}
