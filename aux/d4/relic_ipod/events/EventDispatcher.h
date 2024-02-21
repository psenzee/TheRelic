#ifndef _EVENTDISPATCHER_H
#define _EVENTDISPATCHER_H

#include <map>
#include <string>
#include <vector>

class EventDispatcher
{
public:

    typedef bool (*function_t)(void *context);

    EventDispatcher();

    void Register  (const char *event, function_t function, void *context);

    void Unregister(const char *event);
    void Unregister(const char *event, function_t function, void *context);
    void Unregister(const char *event, function_t function);
    void Unregister(const char *event, void *context);

    void Unregister(function_t function, void *context);
    void Unregister(function_t function);
    void Unregister(void *context);

    bool Fire(const char *event);

    struct EventHandler
    {
        function_t  function;
        void       *context;

        inline EventHandler(function_t f, void *c) : function(f), context(c) {}
    };

private:

    std::map<std::string, std::vector<EventHandler> > map;
};

#endif // _EVENTDISPATCHER_H