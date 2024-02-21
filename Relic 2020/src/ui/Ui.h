#ifndef _UI_H
#define _UI_H

#include "core/core.h"

enum UiTextJustify { JUSTIFY_LEFT, JUSTIFY_CENTER, JUSTIFY_RIGHT };

struct UiBounds
{
    Vector2 minimum,
            maximum;

    inline UiBounds() : minimum(FLT_MAX, FLT_MAX), maximum(-FLT_MAX, -FLT_MAX) {}
    inline UiBounds(const Vector2 &minimum, const Vector2 &maximum) : minimum(minimum), maximum(maximum) {}
    inline UiBounds(const UiBounds &other) : minimum(other.minimum), maximum(other.maximum) {}

    inline bool Contains(const Vector2 &at) const
    {
        return at.x >= minimum.x && at.y >= minimum.y && 
               at.x <= maximum.x && at.y <= maximum.y;
    }
    inline UiBounds &operator=(const UiBounds &other)
    {
        if (this == &other)
            return *this;
        minimum = other.minimum;
        maximum = other.maximum;
        return *this;
    }
};

struct UiEvent
{
    enum Type { CLICK_BEGAN, CLICK_STATIONARY, CLICK_MOVED, CLICK_ENDED, CLICK_ENTERED, CLICK_LEAVE, EVENT_NONE };

    Type    type;
    Vector2 at;
    String  info;

    inline UiEvent(Type type, const Vector2 &at, const char *info) : type(type), at(at), info(info) {}
    inline UiEvent(const UiEvent &other) : type(other.type), at(other.at), info(other.info) {}
    inline UiEvent &operator=(const UiEvent &other)
    {
        if (this == &other)
            return *this;
        type  = other.type;
        at    = other.at;
        info  = other.info;
        return *this;
    }

    static inline bool IsFundamentalEvent(Type type)
    {
        return type == CLICK_BEGAN || type == CLICK_STATIONARY ||
               type == CLICK_MOVED || type == CLICK_ENDED;
    }
    
    static inline const char *GetEventTypeName(Type type)
    {
        switch (type)
        {
        case CLICK_BEGAN:      return "CLICK_BEGAN";
        case CLICK_STATIONARY: return "CLICK_STATIONARY";
        case CLICK_MOVED:      return "CLICK_MOVED";
        case CLICK_ENDED:      return "CLICK_ENDED";
        case CLICK_ENTERED:    return "CLICK_ENTERED";
        case CLICK_LEAVE:      return "CLICK_LEAVE";
        }
        return "EVENT_NONE";
    }
    static inline Type GetEventTypeFromName(const char *name)
    {
        if      (strcmp(name, "CLICK_BEGAN")      == 0) return CLICK_BEGAN;
        else if (strcmp(name, "CLICK_STATIONARY") == 0) return CLICK_STATIONARY;
        else if (strcmp(name, "CLICK_MOVED")      == 0) return CLICK_MOVED;
        else if (strcmp(name, "CLICK_ENDED")      == 0) return CLICK_ENDED;
        else if (strcmp(name, "CLICK_ENTERED")    == 0) return CLICK_ENTERED;
        else if (strcmp(name, "CLICK_LEAVE")      == 0) return CLICK_LEAVE;
        return EVENT_NONE;
    }
};

class UiState
{
public:
    enum State { INACTIVE, FOCUSED, SELECTED /*, DOUBLE_SELECTED*/ };

    inline UiState() : mState(INACTIVE) {}

    inline bool IsActive() const   { return mState != INACTIVE; }
    inline bool IsFocused() const  { return mState == FOCUSED; }
    inline bool IsSelected() const { return mState == SELECTED; }

    void Notify(const UiEvent &e)
    {
        switch (e.type)
        {
        case UiEvent::CLICK_BEGAN:
            switch (mState)
            {
            case INACTIVE:
            case SELECTED:  mState = FOCUSED;  break;
            case FOCUSED:   mState = FOCUSED;  break;
            default:        mState = INACTIVE; break;
            }
        case UiEvent::CLICK_MOVED:
            switch (mState)
            {
            case INACTIVE:
          //case SELECTED:  mState = SELECTED; break;
            case SELECTED:  mState = FOCUSED;  break;
            case FOCUSED:   mState = FOCUSED;  break;
            default:        mState = INACTIVE; break;
            }
            break;
        case UiEvent::CLICK_ENDED:
            switch (mState)
            {
            case FOCUSED:   mState = SELECTED; break;
            default:        mState = INACTIVE; break;
            }
            break;
        case UiEvent::CLICK_LEAVE:
            mState = INACTIVE;
            break;
        case UiEvent::CLICK_ENTERED:
            mState = FOCUSED;
            break;
        }
    }

    inline State GetState() const { return mState; }

private:
    State mState;
};

#endif // _UI_H