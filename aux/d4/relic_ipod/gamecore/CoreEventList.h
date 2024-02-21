#ifndef _COREEVENTLIST_H
#define _COREEVENTLIST_H

#include "IEventList.h"

#include <vector>

class NullEventList : public IEventList
{
public:

    Event **GetEvents()           { return 0; }
    int     GetEventCount() const { return 0; }

    static NullEventList instance;
};

class CoreEventList : public IEventList
{
public:

    CoreEventList();
    CoreEventList(const std::vector<Event *> &events);

    Event **GetEvents();
    int     GetEventCount() const;

private:

    std::vector<Event *> mEvents;
};


#endif // _COREEVENTLIST_H
