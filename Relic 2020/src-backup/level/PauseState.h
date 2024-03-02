#ifndef _PAUSE_STATE_H
#define _PAUSE_STATE_H

struct PauseState
{
    bool render,
         update,
         characters;

    inline explicit PauseState(bool render = false, bool update = false, bool characters = false) 
        : render(render), update(update), characters(characters) {}
};

#endif // _PAUSE_STATE_H
