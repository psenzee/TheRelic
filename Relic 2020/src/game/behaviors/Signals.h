#ifndef _SIGNALS_H
#define _SIGNALS_H

#include <map>
#include <vector>

enum Signal
{
    SIGNAL_NONE = 0,
    SIGNAL_DELETE = 1,
    
    SIGNAL_DIED = 2,
    SIGNAL_DEATH_COMPLETE = 3,
    SIGNAL_ABSORBED = 4,
    SIGNAL_RECEIVED_HIT = 5,
    SIGNAL_ATTACK_START = 6,
    SIGNAL_ATTACK_COMPLETE = 7,

    SIGNAL_LANDED_HIT = 8,
    SIGNAL_EVADED_HIT = 9,    
};

class SignalHandler
{
public:

    typedef bool (*function_t)(void *user);

    void Add(int signal, function_t handler, void *user) { mHandlers.push_back(Item(signal, handler, user)); }
    void Clear() { mHandlers.clear(); }
    void Signal(int signal)
    {
        for (std::vector<Item>::iterator i = mHandlers.begin(), e = mHandlers.end(); i != e; ++i)
            if ((*i).signal == signal)
                (*i).Call();
    }

private:

    struct Item
    {
        int         signal;
        function_t  handler;
        void       *user;

        inline Item(int signal, function_t handler, void *user) : signal(signal), handler(handler), user(user) {}

        inline bool Call() { if (handler) return handler(user); return false; }
    };

    std::vector<Item> mHandlers;

};

#endif // _SIGNALS_H