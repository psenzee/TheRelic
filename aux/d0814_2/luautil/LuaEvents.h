#ifndef _LUAEVENTS_H
#define _LUAEVENTS_H

#include "luautil/LuaValue.h"
#include "luautil/LuaCall.h"
#include "events/IEventListener.h"
#include "fast/Allocator.h"

// a lua listener looks like:
// -------------------------
// function MyListener(infoString, luaUserData)
//
// registering it looks like:
// --------------------------
// MY_LISTENER = RegisterListener("<EventName>", "MyListener", luaUserData)
//
// unregistering it looks like:
// ----------------------------
// UnregisterListener(MY_LISTENER)
// MY_LISTENER = nil -- * note that MY_LISTENER is no longer valid
//
// firing an event (visible to both C++ and lua code):
// --------------------------------------------------
// FireEvent("<EventName>", "<Info>")
//
// set a timer event:
// ------------------
// SetTimerEvent("<EventName>", timeMs)
//

struct lua_State;
class EventDispatcher;
class EventTimer;

class LuaListener : public IEventListener
{
public:

    CLASS_NEW_DELETE()
   
    LuaListener(lua_State *lua, const char *function, LuaValue &data, EventDispatcher *dispatcher);
    ~LuaListener();

    // implements IEventListener::Notify(..) for convenience..
    bool Notify(const char *event, const char *info);
    bool Fire(const char *info);
    void Register(const char *event);
    void Unregister();
    
private:    

    static bool HandleEvent(const char *info, void *context);
    
    lua_State       *mLuaState;
    String           mFunction,
                     mEvent;
    EventDispatcher *mDispatcher;
    LuaValue         mData;    
};

// do not allocate on the stack! because this class deletes itself
class TimerEvent // only fires once, then removes & deletes itself
{
public:

    CLASS_NEW_DELETE()
    
    TimerEvent(EventDispatcher *dispatcher, EventTimer *timer, unsigned elapsed, const char *event);
        
private:

    static void HandleTimerEvent(void *context);
    
    void Unregister();
    
    ~TimerEvent();

    String           mEventName;
    EventDispatcher *mDispatcher;
    EventTimer      *mTimer;
    unsigned         mElapsed;
};

EventDispatcher *GetGlobalEventDispatcher();
EventTimer      *GetGlobalEventTimer();

void RegisterLuaEventFunctions(lua_State *lua);

#endif // _LUAEVENTS_H