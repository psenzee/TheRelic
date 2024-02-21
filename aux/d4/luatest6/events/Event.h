#ifndef _EVENT_H
#define _EVENT_H

#include <string>

class Event
{
public:
    std::string text;

    Event(const char *text) : text(text) {}
};

#endif // _EVENT_H