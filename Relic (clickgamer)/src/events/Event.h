#ifndef _EVENT_H
#define _EVENT_H

#include "core/core.h"
#include "fast/Allocator.h"

class Event
{
public:
    
    CLASS_NEW_DELETE()
    
    String text;

    Event(const char *text) : text(text) {}
};

#endif // _EVENT_H