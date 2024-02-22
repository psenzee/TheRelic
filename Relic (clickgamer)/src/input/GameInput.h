#ifndef _GAMEINPUT_H
#define _GAMEINPUT_H

#include "core/core.h"

class Toggle
{
public:
    
    typedef void (*OnStateChanged_t)(Toggle *toggle, void *context);

    inline Toggle(bool v = false) : state(v), changed(0), context(0) {}
    
    inline bool GetState()       const { return state; }
    inline void SetState(bool v)       { if (state != v) { state = v; if (changed) changed(this, context); } }
    
    inline void SetStateChangedFunction(OnStateChanged_t fn, void *contxt) { changed = fn; context = contxt; }
    
private:    
    
    bool              state;
    OnStateChanged_t  changed;
    void             *context;
};


class InputVector
{
public:
    
    inline Vector2 GetVector()
    {
        return Vector2(_clamp(vector.x * scale.x, clamp.x, clamp.y),
                       _clamp(vector.y * scale.y, clamp.z, clamp.w));        
    }
    
    inline void SetVector(const Vector2 &v)
    {
        vector = v;
    }
    
    inline const Vector2 &GetVector() const
    {
        return vector;
    }    
    
    inline InputVector(const Vector2 &scale, const Vector2 &clamp) 
        : scale(scale), clamp(-clamp.x, clamp.x, -clamp.y, clamp.y) {}
    
private:
    
    inline static float _clamp(float v, float min, float max)
    {
        return (v < min) ? min : ((v > max) ? max : v);
    }
    
    Vector2 vector,
            scale;
    Vector4 clamp;
};


class GameInput
{
public:

    InputVector move;
    Toggle      resume,
                back,
                start;
    
    bool        enabled;
        
    void Update(const GameTime &time)
    {
        if (enabled)
            GetInput();
    }
    
    inline GameInput() : enabled(false), move(Vector2(1.0f, 1.0f), Vector2(200.0f, 200.0f)),
                         touched(false), touch(-1, -1) {}

    void ScreenTouch(int index, const core::Point &at, const char *function);    
    void ScreenTouchBegan(int index, const core::Point &at);
    void ScreenTouchMoved(int index, const core::Point &at);
    void ScreenTouchEnded(int index, const core::Point &at);    
    void ScreenTouchStationary(int index, const core::Point &at);        
    
    const core::Point &GetTouch() const { return touch; }
    
private:
    
    bool        touched;
    core::Point touch;
    
    void GetInput();
};

#endif // _GAMEINPUT_H