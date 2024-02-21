#ifndef _EVENTTIMER_H
#define _EVENTTIMER_H

#include <vector>

class EventTimer
{
public:

    typedef void (*function_t)(void *context);

    EventTimer();

    void Add(function_t function, void *context, unsigned when);

    void Remove(function_t f);
    void Remove(void *context);

    void Update(unsigned elapsed);

    struct Event
    {
        function_t  function;
        void       *context;
        unsigned    time;

        inline Event(function_t f, void *c, unsigned t) : function(f), context(c), time(t) {}
    };

private:

    void Remove(function_t f, void *context);
    void RemoveAfter(unsigned time);
    void RemoveBefore(unsigned time);

    std::vector<Event> events;
    unsigned           elapsed;
};

#endif // _EVENTTIMER_H