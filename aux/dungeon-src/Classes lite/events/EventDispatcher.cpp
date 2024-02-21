#include "eventdispatcher.h"

#include <algorithm>

EventDispatcher::EventDispatcher()
{
}

void EventDispatcher::Register(const char *event, function_t function, void *context)
{
    map[event].push_back(EventHandler(function, context));
}

void EventDispatcher::Unregister(const char *event)
{
    map[event].clear();
}

// horrible, but effective
#define MATCH_TYPE(NAME, EXPRESSION) \
struct Match##NAME\
{\
    EventDispatcher::EventHandler e;\
    inline Match##NAME(const EventDispatcher::EventHandler &e) : e(e) {}\
    inline bool operator()(const EventDispatcher::EventHandler &a) const { return (EXPRESSION); }\
};

namespace // keep these local to this file
{
MATCH_TYPE(Function,            (a.function == e.function));
MATCH_TYPE(Context,             (a.context  == e.context));
MATCH_TYPE(FunctionAndContext,  (a.function == e.function && a.context == e.context));
}

#undef MATCH_TYPE

void EventDispatcher::Unregister(const char *event, EventDispatcher::function_t f)
{
    std::vector<EventHandler> &eh = map[event];
    MatchFunction m(EventHandler(f, 0));
    eh.erase(std::remove_if(eh.begin(), eh.end(), m), eh.end());
}

void EventDispatcher::Unregister(const char *event, void *context)
{
    std::vector<EventHandler> &eh = map[event];
    MatchContext m(EventHandler(0, context));
    eh.erase(std::remove_if(eh.begin(), eh.end(), m), eh.end());
}

void EventDispatcher::Unregister(const char *event, EventDispatcher::function_t f, void *context)
{
    std::vector<EventHandler> &eh = map[event];
    MatchFunctionAndContext m(EventHandler(f, context));
    eh.erase(std::remove_if(eh.begin(), eh.end(), m), eh.end());
}

void EventDispatcher::Unregister(EventDispatcher::function_t f, void *context)
{
    MatchFunctionAndContext m(EventHandler(f, context));
    for (std::map<std::string, std::vector<EventHandler> >::iterator i = map.begin(), e = map.end(); i != e; ++i)
    {
        std::vector<EventHandler> &v = (*i).second;
        v.erase(std::remove_if(v.begin(), v.end(), m), v.end());
    }
}

void EventDispatcher::Unregister(EventDispatcher::function_t f)
{
    MatchFunction m(EventHandler(f, 0));
    for (std::map<std::string, std::vector<EventHandler> >::iterator i = map.begin(), e = map.end(); i != e; ++i)
    {
        std::vector<EventHandler> &v = (*i).second;
        v.erase(std::remove_if(v.begin(), v.end(), m), v.end());
    }
}

void EventDispatcher::Unregister(void *context)
{
    MatchContext m(EventHandler(0, context));
    for (std::map<std::string, std::vector<EventHandler> >::iterator i = map.begin(), e = map.end(); i != e; ++i)
    {
        std::vector<EventHandler> &v = (*i).second;
        v.erase(std::remove_if(v.begin(), v.end(), m), v.end());
    }
}

bool EventDispatcher::Fire(const char *event)
{
    std::vector<EventHandler> &v = map[event];
    for (std::vector<EventHandler>::iterator i = v.begin(), e = v.end(); i != e; ++i)
        if ((*i).function && (*i).function((*i).context))
            return true;
    return false;
}