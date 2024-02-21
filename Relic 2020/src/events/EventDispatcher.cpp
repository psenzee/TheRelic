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
    for (std::map<String, std::vector<EventHandler> >::iterator i = map.begin(), e = map.end(); i != e; ++i)
    {
        std::vector<EventHandler> &v = (*i).second;
        v.erase(std::remove_if(v.begin(), v.end(), m), v.end());
    }
}

void EventDispatcher::Unregister(EventDispatcher::function_t f)
{
    MatchFunction m(EventHandler(f, 0));
    for (std::map<String, std::vector<EventHandler> >::iterator i = map.begin(), e = map.end(); i != e; ++i)
    {
        std::vector<EventHandler> &v = (*i).second;
        v.erase(std::remove_if(v.begin(), v.end(), m), v.end());
    }
}

void EventDispatcher::Unregister(void *context)
{
    MatchContext m(EventHandler(0, context));
    for (std::map<String, std::vector<EventHandler> >::iterator i = map.begin(), e = map.end(); i != e; ++i)
    {
        std::vector<EventHandler> &v = (*i).second;
        v.erase(std::remove_if(v.begin(), v.end(), m), v.end());
    }
}

static void PrintEvents(const std::vector< std::pair<String, String> > &events, const EventDispatcher *ed)
{
    printf("\nevents = %d (%p) -----------\n", (int)events.size(), ed);
    for (std::vector< std::pair<String, String> >::const_iterator i = events.begin(), e = events.end(); i != e; ++i)
        printf("%s/%s\n", (*i).first.c_str(), (*i).second.c_str());
    printf("-----------\n");
}

void EventDispatcher::Fire(const char *event, const char *info)
{
    mEvents.push_back(std::pair<String, String>(event, info));
}

bool EventDispatcher::FireInternal(const String &event, const char *info)
{
    if (map.find(event) != map.end())
    {
        std::vector<EventHandler> &v = map[event];
        for (std::vector<EventHandler>::iterator i = v.begin(), e = v.end(); i != e; ++i)
            if ((*i).function && (*i).function(info, (*i).context))
                return true;
    }
    return false;
}

void EventDispatcher::Update()
{
    std::vector< std::pair<String, String> > events(mEvents);
    mEvents.clear();
    for (std::vector< std::pair<String, String> >::iterator i = events.begin(), e = events.end(); i != e; ++i)
        FireInternal((*i).first, (*i).second.c_str());
}